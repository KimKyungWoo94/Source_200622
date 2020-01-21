/* 
 * ASN1 PER decoder
 * Copyright (C) 2011-2018 Fabrice Bellard
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "asn1defs_int.h"

//#define DEBUG
//#define DEBUG_GET_BITS

typedef struct ASN1DecodeState {
    const uint8_t *buf;
    size_t buf_len;
    size_t buf_index;
    int bit_count; /* current number of bits in bit_buf */
    uint32_t bit_buf; /* bit buffer, starting from MSB */
    BOOL aligned_per;
    ASN1ValueStack *top_value;

    ASN1Error error;
} ASN1DecodeState;

static __attribute__((format(printf, 2, 3))) 
    int asn1_decode_error(ASN1DecodeState *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    s->error.bit_pos = s->buf_index * 8 - s->bit_count;
    vsnprintf(s->error.msg, sizeof(s->error.msg), fmt, ap);
    va_end(ap);
    return -1;
}
                              
static int mem_error(ASN1DecodeState *s)
{
    return asn1_decode_error(s, "not enough memory");
}

static void asn1_get_bits_init(ASN1DecodeState *s, 
                               const uint8_t *buf, size_t buf_len,
                               BOOL aligned_per)
{
    s->buf = buf;
    s->buf_len = buf_len;
    s->aligned_per = aligned_per;
    s->bit_count = 0;
    s->buf_index = 0;
    s->bit_buf = 0;
}

/* 1 <= n <= 32 */
/* XXX: not optimized. Return 0 if OK, -1 if not enough data. */
static no_inline int asn1_get_bits(ASN1DecodeState *s, int n, uint32_t *pval)
{
    uint32_t val;
    int l, i, n1;

#ifdef DEBUG_GET_BITS
    printf("[bitbuf=0x%08x cnt=%d] ", s->bit_buf, s->bit_count);
    printf("get_bits: n=%d", n);
#endif
    if (unlikely(s->bit_count < n)) {
        val = s->bit_buf >> (32 - n); /* get the remaining bits */
        n1 = n - s->bit_count;
        l = s->buf_len - s->buf_index;
        if (likely(l >= 4)) {
            /* fast case */
            s->bit_buf = to_be32(s->buf + s->buf_index);
            s->buf_index += 4;
            s->bit_count = 32;
        } else {
            /* slow case */
            if (l * 8 + s->bit_count < n) 
                return asn1_decode_error(s, "reading after end of stream");
            s->bit_count = l * 8;
            s->bit_buf = 0;
            for(i = 0; i < l; i++)
                s->bit_buf |= (uint32_t)s->buf[s->buf_index++] << (24 - i * 8);
        }
        n = n1;
        val |= s->bit_buf >> (32 - n);
        if (n == 32)
            s->bit_buf = 0;
        else
            s->bit_buf <<= n;
        s->bit_count -= n;
    } else {
        val = s->bit_buf >> (32 - n);
        if (n == 32)
            s->bit_buf = 0;
        else
            s->bit_buf <<= n;
        s->bit_count -= n;
    }
#ifdef DEBUG_GET_BITS
    printf(" val=0x%x\n", val);
#endif
    *pval = val;
    return 0;
}

static void asn1_align(ASN1DecodeState *s)
{
    int n;
    uint32_t val;

    if (s->aligned_per) {
        n = s->bit_count & 7;
        if (n != 0) {
            asn1_get_bits(s, n, &val);
        }
    }
}

/* constrained signed or unsigned 32 bit number */
static int asn1_per_decode_constrained_whole_number(ASN1DecodeState *s, 
                                             int range_min, int range_max,
                                             uint32_t *pval)
{
    int n;
    uint32_t diff, v, val;
    
    diff = range_max - range_min;
    if (diff == 0) {
        val = 0;
        /* nothing to decode */
    } else {
        if (diff <= 254 || !s->aligned_per) {
            n = 32 - clz32(diff);
            if (asn1_get_bits(s, n, &val))
                return -1;
        } else if (diff == 255) {
            asn1_align(s);
            if (asn1_get_bits(s, 8, &val))
                return -1;
        } else if (diff <= 0xffff) {
            asn1_align(s);
            if (asn1_get_bits(s, 16, &val))
                return -1;
        } else {
            if (asn1_get_bits(s, 2, &v))
                return -1;
            n = v + 1; /* number of bytes */
            asn1_align(s);
            if (asn1_get_bits(s, n * 8, &val))
                return -1;
        }
        if (val > diff)
            return asn1_decode_error(s, "overflow in constrained whole number");
    }
    *pval = val + range_min;
    return 0;
}

typedef int DecodeItemsFunc(ASN1DecodeState *, uint32_t base, uint32_t len, void *opaque);

/* Note: decode_buf is not called for zero length. */
static int asn1_per_decode_ulength_items(ASN1DecodeState *s, 
                                         DecodeItemsFunc *decode_buf,
                                         void *opaque, uint32_t *plen)
{
    uint32_t val, v, len, base;
    int ret;

    base = 0;
    for(;;) {
        asn1_align(s);
        if (asn1_get_bits(s, 8, &val))
            return -1;
        if (val >= 0xc0) {
            /* fragmented case */
            val -= 0xc0;
            if (val < 1 || val > 4)
                return -1;
            len = val * 16384;
            ret = decode_buf(s, base, len, opaque);
            if (ret)
                return ret;
            base += len;
        } else {
            if (val >= 0x80) {
                if (asn1_get_bits(s, 8, &v))
                    return -1;
                len = ((val << 8) | v) & 0x3fff;
            } else {
                len = val;
            }
            if (len != 0) {
                ret = decode_buf(s, base, len, opaque);
                if (ret)
                    return ret;
            }
            base += len;
            break;
        }
    }
    if (plen)
        *plen = base;
    return 0;
}

static int asn1_per_decode_unconstrained_length(ASN1DecodeState *s,
                                                uint32_t *pval)
{
    uint32_t val, v;

    asn1_align(s);
    if (asn1_get_bits(s, 8, &val))
        return -1;
    if (val >= 0x80) {
        if (val < 0xc0) {
            if (asn1_get_bits(s, 8, &v))
                return -1;
            val = ((val << 8) | v) & 0x3fff;
        } else {
            return asn1_decode_error(s, "fragmentation is not supported");
        }
    }
    *pval = val;
    return 0;
}

static int asn1_per_decode_semi_constrained_whole_number(ASN1DecodeState *s, 
                                                         int range_min,
                                                         uint32_t *pval)
{
    uint32_t val, n, max_val;


    if (asn1_per_decode_unconstrained_length(s, &n))
        return -1;
    if (n == 0 || n > 4)
        return -1;
    if (asn1_get_bits(s, 8 * n, &val))
        return -1;
    /* check that the encoding is canonical */
    if (n > 1 && (val >> (8 * (n - 1))) == 0)
        return asn1_decode_error(s, "integer encoding not canonical");
    if (range_min < 0) {
        /* assume signed 32 bit */
        max_val = INT32_MAX - range_min;
    } else {
        max_val = UINT32_MAX - range_min;
    }
    if (val > max_val)
        return asn1_decode_error(s, "32 bit integer overflow");
    *pval = val + range_min;
    return 0;
}

static int asn1_per_decode_unconstrainted_whole_number(ASN1DecodeState *s, 
                                                       uint32_t *pval)
{
    uint32_t v, a, n;
    int val, n_bits;

    if (asn1_per_decode_unconstrained_length(s, &n))
        return -1;
    if (n == 0 || n > 4)
        return -1;
    n_bits = n * 8;
    if (asn1_get_bits(s, n_bits, &v))
        return -1;
    /* check canonical encoding */
    if (n > 1) {
        a = v >> (n_bits - 9);
        if (a == 0 || a == 0x1ff)
            return -1;
    }
    val = ((int)v << (32 - n_bits)) >> (32 - n_bits);
    *pval = val;
    return 0;
}


static int asn1_per_decode_normally_small_non_neg_whole_number(ASN1DecodeState *s, 
                                                               uint32_t *pval)
{
    uint32_t v, val;

    if (asn1_get_bits(s, 1, &v))
        return -1;
    if (v == 0) {
        if (asn1_get_bits(s, 6, &val))
            return -1;
    } else {
        if (asn1_per_decode_semi_constrained_whole_number(s, 0, &val))
            return -1;
    }
    *pval = val;
    return 0;
}

/* return val >= 1 */
static int asn1_per_decode_normally_small_length(ASN1DecodeState *s,
                                                 uint32_t *pval)
{
    uint32_t val, v;

    if (asn1_get_bits(s, 1, &v))
        return -1;
    if (v == 0) {
        if (asn1_get_bits(s, 6, &val))
            return -1;
        val++;
    } else {
        if (asn1_per_decode_unconstrained_length(s, &val))
            return -1;
        if (val == 0)
            return -1;
    }
    *pval = val;
    return 0;
}

static int asn1_per_decode_boolean(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    uint32_t val;

    if (asn1_get_bits(s, 1, &val))
        return -1;
    *(BOOL *)data = val;
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
static int decode_large_positive_integer(ASN1DecodeState *s, 
                                         ASN1Integer *r, int nb_bytes)
{
    int len, l, i;
    uint32_t b;

    len = (nb_bytes + ASN1_LIMB_BYTES - 1) / ASN1_LIMB_BYTES;
    l = nb_bytes & (ASN1_LIMB_BYTES - 1);
    if (l == 0)
        l = ASN1_LIMB_BYTES;
    if (asn1_integer_resize(r, len))
        return mem_error(s);
    if (asn1_get_bits(s, l * 8, &b))
        return -1;
    r->data[len - 1] = b;
    for(i = len - 2; i >= 0; i--) {
        if (asn1_get_bits(s, 32, &b))
            return -1;
        r->data[i] = b;
    }
    asn1_integer_renorm(r);
    return 0;
}

static int decode_large_integer(ASN1DecodeState *s, 
                                ASN1Integer *r)
{
    uint32_t nb_bytes, b;
    int len, l, i;
    if (asn1_per_decode_unconstrained_length(s, &nb_bytes))
        return -1;
    if (nb_bytes == 0)
        return asn1_decode_error(s, "integer cannot have zero length");
    len = (nb_bytes + ASN1_LIMB_BYTES - 1) / ASN1_LIMB_BYTES;
    if (asn1_integer_resize(r, len))
        return mem_error(s);
    l = nb_bytes & (ASN1_LIMB_BYTES - 1);
    if (l == 0) {
        l = ASN1_LIMB_BYTES;
    }
    if (asn1_get_bits(s, l * 8, &b))
        return -1;
    b = ((int)b << (32 - l * 8)) >> (32 - l * 8);
    r->data[len - 1] = b;
    for(i = len - 2; i >= 0; i--) {
        if (asn1_get_bits(s, 32, &b))
            return -1;
        r->data[i] = b;
    }
    if (asn1_integer_from_2comp(r))
        return mem_error(s);
    return 0;
}

static int asn1_per_decode_large_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                         uint8_t *data)
{
    const ASN1Integer *range_min, *range_max;
    ASN1Integer *val, diff_s, *diff = &diff_s;
    int flags;
    uint32_t b;

    flags = *p++;
    range_min = NULL;
    range_max = NULL;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = (const ASN1Integer *)*p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = (const ASN1Integer *)*p++;
    val = (ASN1Integer *)data;
    asn1_integer_init(val);

    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b) {
            if (decode_large_integer(s, val))
                return -1;
            goto the_end;
        }
    }
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        asn1_integer_init(diff);
        if (asn1_integer_sub(diff, range_max, range_min))
            goto fail1_mem;
        if (diff->len == 0) {
            /* nothing to encode */
        } else if (!s->aligned_per || asn1_integer_cmp_si(diff, 254) <= 0) {
            int nb_bits, l, i, len;

            nb_bits = asn1_integer_get_bit_size(diff);
            len = (nb_bits + ASN1_LIMB_BITS - 1) >> ASN1_LIMB_BITS_LOG2;
            if (asn1_integer_resize(val, len))
                goto fail1_mem;
            l = nb_bits & (ASN1_LIMB_BITS - 1);
            if (l == 0) 
                l = ASN1_LIMB_BITS;
            if (asn1_get_bits(s, l, &b))
                goto fail1;
            val->data[len - 1] = b;
            for(i = len - 2; i >= 0; i--) {
                if (asn1_get_bits(s, 32, &b))
                    goto fail1;
                val->data[i] = b;
            }
            asn1_integer_renorm(val);
        } else if (asn1_integer_cmp_si(diff, 255) == 0) {
            asn1_align(s);
            if (asn1_get_bits(s, 8, &b))
                goto fail1;
            if (asn1_integer_set_ui(val, b))
                goto fail1_mem;
        } else if (asn1_integer_cmp_si(diff, 0xffff) <= 0) {
            asn1_align(s);
            if (asn1_get_bits(s, 16, &b))
                goto fail1;
            if (asn1_integer_set_ui(val, b)) {
            fail1_mem:
                mem_error(s);
                goto fail1;
            }
        } else {
            int max_bytes, length_bits, nb_bytes;
            max_bytes = (asn1_integer_get_bit_size(diff) + 7) / 8;
            length_bits = 32 - clz32(max_bytes - 1);
            /* XXX: spec not clear */
            if (asn1_get_bits(s, length_bits, &b))
                goto fail1;
            nb_bytes = b + 1;
            asn1_align(s);
            if (decode_large_positive_integer(s, val, nb_bytes)) {
            fail1:
                asn1_integer_clear(diff);
                return -1;
            }
        }
        if (asn1_integer_cmp(val, diff) > 0) {
            asn1_integer_clear(diff);
            return asn1_decode_error(s, "overflow in constrained whole number");
        }
        asn1_integer_clear(diff);
        if (asn1_integer_add(val, val, range_min))
            return mem_error(s);
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        if (asn1_per_decode_unconstrained_length(s, &b))
            return -1;
        if (b == 0) 
            return asn1_decode_error(s, "integer cannot have zero length");
        if (decode_large_positive_integer(s, val, b))
            return -1;
        if (asn1_integer_add(val, val, range_min))
            return mem_error(s);
    } else {
        if (decode_large_integer(s, val))
            return -1;
    }
 the_end:
#if defined(DEBUG)
    {
        int ret;
        printf("large integer: ");
        ret = asn1_integer_out_str(stdout, val);
        (void)ret;
        printf("\n");
    }  
#endif
    return 0;
}
#endif

/* constrainted signed or unsigned 32 bit number */
static int asn1_per_decode_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    int range_min, range_max, flags;
    uint32_t val;

    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_per_decode_large_integer(s, p, data);
#else
        return asn1_decode_error(s, "decoding of large integers is not supported");
#endif        
    }
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_get_bits(s, 1, &val))
            return -1;
        if (val) {
            if (asn1_per_decode_unconstrainted_whole_number(s, &val))
                return -1;
            goto the_end;
        }
    }
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        range_min = p[1];
        range_max = p[2];
        if (asn1_per_decode_constrained_whole_number(s, 
                                                     range_min, range_max, 
                                                     &val))
            return -1;
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        range_min = p[1];
        if (asn1_per_decode_semi_constrained_whole_number(s, range_min, &val))
            return -1;
    } else {
        if (asn1_per_decode_unconstrainted_whole_number(s, &val))
            return -1;
        
        if (flags & ASN1_CTYPE_HAS_HIGH) {
            range_max = p[1];
            if ((int)val > range_max)
                return asn1_decode_error(s, "integer < higher end (%d > %d)",
                                         val, range_max);
        }
    }
 the_end:
    *(int *)data = val;
    return 0;
}

static int asn1_per_decode_bit_string1(ASN1DecodeState *s, 
                                       uint32_t base, uint32_t len, 
                                       void *opaque)
{
    ASN1BitString *str = opaque;
    uint8_t *buf;
    uint32_t i, v, n, k;

    buf = asn1_realloc(str->buf, (base + len + 7) / 8);
    if (!buf)
        return mem_error(s);
    str->buf = buf;
    str->len = base + len;

    buf += base >> 3;
    n = len >> 3;
    for(i = 0; i < n; i++) {
        if (asn1_get_bits(s, 8, &v))
            return -1;
        buf[i] = v;
    }
    k = len & 7;
    if (k != 0) {
        if (asn1_get_bits(s, k, &v))
            return -1;
        buf[n] = v << (8 - k);
    }
    return 0;
}

static int asn1_per_decode_bit_string(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    uint32_t range_min, range_max, len, b;
    ASN1BitString *str;
    int flags, ret;

    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    str = (ASN1BitString *)data;
    str->buf = NULL;
    str->len = 0;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b) {
            range_min = 0;
            goto unconstrained_length;
        }
    }
    
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        if (range_max >= 65536) 
            goto unconstrained_length;
        if (asn1_per_decode_constrained_whole_number(s, range_min, range_max, 
                                                     &len))
            return -1;
        if (range_min == range_max && range_max <= 16) {
            /* no alignment */
        } else {
            asn1_align(s);
        }
        ret = asn1_per_decode_bit_string1(s, 0, len, str);
        if (ret)
            return ret;
    } else {
 unconstrained_length:
        ret = asn1_per_decode_ulength_items(s, 
                                            asn1_per_decode_bit_string1, str,
                                            NULL);
        if (ret)
            return ret;
        if ((uint32_t)str->len < range_min)
            return -1;
    }
#ifdef DEBUG
    printf("bit string: len=%d\n", (int)str->len);
#endif
    return 0;
}

static int asn1_per_decode_octet_string1(ASN1DecodeState *s, 
                                         uint32_t base, uint32_t len, 
                                         void *opaque)
{
    ASN1String *str = opaque;
    uint8_t *buf;
    uint32_t i, v;

    buf = asn1_realloc(str->buf, base + len);
    if (!buf)
        return mem_error(s);
    str->buf = buf;
    str->len = base + len;

    buf += base;
    for(i = 0; i < len; i++) {
        if (asn1_get_bits(s, 8, &v))
            return -1;
        buf[i] = v;
    }
    return 0;
}

static int asn1_per_decode_octet_string(ASN1DecodeState *s, const ASN1CType *p, 
                                        uint8_t *data)
{
    uint32_t range_min, range_max, len, b;
    ASN1String *str;
    int flags, ret;

    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    str = (ASN1String *)data;
    str->buf = NULL;
    str->len = 0;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b) {
            range_min = 0;
            goto unconstrained_length;
        }
    }
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
        if (range_max >= 65536) 
            goto unconstrained_length;
        if (asn1_per_decode_constrained_whole_number(s, range_min, range_max, 
                                                     &len))
            return -1;
        if (range_min == range_max && range_max <= 2) {
            /* no alignment */
        } else {
            asn1_align(s);
        }
        ret = asn1_per_decode_octet_string1(s, 0, len, str);
        if (ret)
            return ret;
    } else {
 unconstrained_length:
        ret = asn1_per_decode_ulength_items(s, 
                                            asn1_per_decode_octet_string1, str,
                                            NULL);
        if (ret)
            return ret;
        if ((uint32_t)str->len < range_min)
            return -1;
    }
#ifdef DEBUG
    printf("octet string: len=%d\n", (int)str->len);
#endif
    return 0;
}

static int asn1_per_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data);


static int asn1_per_skip_open_type1(ASN1DecodeState *s, 
                                    uint32_t base, uint32_t len, void *opaque)
{
    uint32_t i, v;
    for(i = 0; i < len; i++) {
        if (asn1_get_bits(s, 8, &v))
            return -1;
    }
    return 0;
}

static int asn1_per_skip_open_type(ASN1DecodeState *s)
{
    return asn1_per_decode_ulength_items(s, asn1_per_skip_open_type1, 
                                        NULL, NULL);
}

static int asn1_per_decode_buf1(ASN1DecodeState *s, 
                                uint32_t base, uint32_t len, void *opaque)
{
    uint8_t **pbuf = opaque;
    uint8_t *buf;
    uint32_t i, v;

    buf = asn1_realloc(*pbuf, base + len);
    if (!buf)
        return mem_error(s);
    *pbuf = buf;
    buf += base;
    for(i = 0; i < len; i++) {
        if (asn1_get_bits(s, 8, &v))
            return -1;
        buf[i] = v;
    }
    return 0;
}

static int asn1_per_decode_open_type(ASN1DecodeState *s, const ASN1CType *type, 
                                     uint8_t *data)
{
    ASN1DecodeState s1_s, *s1 = &s1_s;
    int ret;
    uint8_t *buf;
    uint32_t len;

    buf = NULL;
    ret = asn1_per_decode_ulength_items(s, asn1_per_decode_buf1, &buf, &len);
    if (ret) {
        asn1_free(buf);
        return -1;
    }
    if (len == 0)
        return -1; /* this is invalid */

    asn1_get_bits_init(s1, buf, len, s->aligned_per);

    ret = asn1_per_decode_type(s1, type, data);
    asn1_free(buf);
    return ret;
}

static int asn1_per_decode_ext_group(ASN1DecodeState *s, uint8_t *data,
                                     const ASN1SequenceField *f1,
                                     int nb_group_fields)
{
    ASN1DecodeState s1_s, *s1 = &s1_s;
    int ret, i, flag;
    uint8_t *buf;
    uint32_t len, b;
    const ASN1SequenceField *f;
    uint8_t *table_present;

    buf = NULL;
    ret = asn1_per_decode_ulength_items(s, asn1_per_decode_buf1, &buf, &len);
    if (ret) {
        asn1_free(buf);
        return -1;
    }
    if (len == 0)
        return -1; /* this is invalid */

    asn1_get_bits_init(s1, buf, len, s->aligned_per);

    table_present = asn1_malloc(nb_group_fields);
    if (!table_present) {
        ret = mem_error(s);
        goto fail;
    }

    for(i = 0; i < nb_group_fields; i++) {
        f = f1 + i;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag != ASN1_SEQ_FLAG_NORMAL) {
            if ((ret = asn1_get_bits(s1, 1, &b)) != 0)
                goto fail;
        } else {
            b = 1;
        }
        if (flag != ASN1_SEQ_FLAG_DEFAULT)
            *(BOOL *)(data + f->u.option_offset) = b;
        table_present[i] = b;
    }

    for(i = 0; i < nb_group_fields; i++) {
        if (table_present[i]) {
            f = f1 + i;
            ret = asn1_per_decode_type(s1, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto fail;
        }
    }
    ret = 0;

 fail:
    asn1_free(table_present);
    asn1_free(buf);
    return ret;
}

static int asn1_per_decode_sequence(ASN1DecodeState *s, const ASN1CType *p, 
                                    uint8_t *data)
{
    int has_ext, nb_fields, extension_present, i;
    int ret;
    const ASN1SequenceField *f, *f1, *f2;
    int flag;
    uint32_t b;
    uint8_t *table_present;
    ASN1ValueStack value_entry;

    value_entry.prev = s->top_value;
    value_entry.type = p;
    value_entry.data = data;
    s->top_value = &value_entry;

    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;

    if (has_ext) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        extension_present = b;
    } else {
        extension_present = 0;
    }

    /* read present bits */
    table_present = asn1_malloc(nb_fields);
    if (!table_present)
        return mem_error(s);

    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f)) {
            flag = ASN1_GET_SEQ_FLAG(f);
            if (flag != ASN1_SEQ_FLAG_NORMAL) {
                if (asn1_get_bits(s, 1, &b))
                    goto fail;
                if (flag == ASN1_SEQ_FLAG_OPTIONAL) {
                    *(BOOL *)(data + f->u.option_offset) = b;
                } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
                    if (!b) {
                        if (asn1_sequence_set_default_value(data, f)) {
                            mem_error(s);
                            goto fail;
                        }
                    }
                } else {
                    goto fail;
                }
            } else {
                b = 1; 
            }
        } else {
            b = 0;
        }
        table_present[i] = b;
    }

    /* read sequence values */
    for(i = 0; i < nb_fields; i++) {
        if (table_present[i]) {
            f = f1 + i;
#ifdef DEBUG
            printf("seq field=%s\n", f->name);
#endif
            ret = asn1_per_decode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto fail;
        }
    }

    asn1_free(table_present);
    table_present = NULL;

    if (extension_present) {
        uint32_t nb_exts;
        int nb_group_fields, ext_idx;

        if (asn1_per_decode_normally_small_length(s, &nb_exts))
            goto fail;
        /* Note: nb_exts is at most 16383 */
        table_present = asn1_malloc(nb_exts);
        if (!table_present)
            return mem_error(s);
        
        /* extension present bit mask */
        for(i = 0; i < nb_exts; i++) {
            if (asn1_get_bits(s, 1, &b))
                goto fail;
            table_present[i] = b;
        }
        
        /* extensions */

        i = 0;
        for(ext_idx = 0; ext_idx < nb_exts; ext_idx++) {
            /* find extension */
            while (i < nb_fields) {
                f = f1 + i;
                if (ASN1_IS_SEQ_EXT(f)) 
                    break;
                i++;
            }
            if (i >= nb_fields) {
                /* unknown extension */
                if (table_present[ext_idx]) {
                    if (asn1_per_skip_open_type(s))
                        goto fail;
                }
            } else {
                f = f1 + i;
                if (ASN1_IS_SEQ_EXT_GROUP_START(f)) {
                    /* compute the number of fields in extension group */
                    nb_group_fields = 0;
                    for(;;) {
                        f2 = f + nb_group_fields;
                        nb_group_fields++;
                        if (ASN1_IS_SEQ_EXT_GROUP_END(f2))
                            break;
                    }
                    if (table_present[ext_idx]) {
                        if (asn1_per_decode_ext_group(s, data, 
                                                      f, nb_group_fields))
                            goto fail;
                    }
                    i += nb_group_fields;
                } else {
                    /* simple extension */
                    if (table_present[ext_idx]) {
                        /* mark as present */
                        flag = ASN1_GET_SEQ_FLAG(f);
                        if (flag != ASN1_SEQ_FLAG_DEFAULT)
                            *(BOOL *)(data + f->u.option_offset) = TRUE;
                        if (asn1_per_decode_open_type(s, f->type, 
                                                      data + ASN1_GET_SEQ_OFFSET(f)))
                            goto fail;
                    }
                    i++;
                }
            }
        }
        asn1_free(table_present);
    }
    s->top_value = s->top_value->prev;
    return 0;
 fail:
    asn1_free(table_present);
    return -1;
}

typedef struct {
    ASN1String *str;
    int elem_size;
    ASN1CType *type;
} PERSequenceOfState;

static int asn1_per_decode_sequence_of1(ASN1DecodeState *s, 
                                        uint32_t base, uint32_t len,
                                        void *opaque)
{
    PERSequenceOfState *ss = opaque;
    ASN1String *str = ss->str;
    uint8_t *buf;
    uint32_t i;
    int ret;

    buf = asn1_realloc2(str->buf, base + len, ss->elem_size);
    if (!buf)
        return mem_error(s);
    memset(buf + base * ss->elem_size, 0, len * ss->elem_size);
    str->buf = buf;
    str->len = base + len;
    for(i = 0; i < len; i++) {
        ret = asn1_per_decode_type(s, ss->type, 
                                   buf + (base + i) * ss->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_per_decode_sequence_of(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    uint32_t len, b;
    int ret, range_min, range_max, flags;
    ASN1String *str;
    PERSequenceOfState ss_s, *ss = &ss_s;

    str = (ASN1String *)data;
    str->buf = NULL;
    str->len = 0;
    ss->str = str;

    flags = *p++;
    range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = *p++;
    } else {
        range_max = UINT32_MAX;
    }
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b) {
            range_min = 0;
            goto unconstrained_length;
        }
    }
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        if (range_max >= 65536) 
            goto unconstrained_length;
        if (asn1_per_decode_constrained_whole_number(s, range_min, 
                                                     range_max, &len))
            return -1;
        f = (const ASN1SequenceOfCType *)p;
        ss->type = f->type;
        ss->elem_size = f->elem_size;
        ret = asn1_per_decode_sequence_of1(s, 0, len, ss);
        if (ret)
            return ret;
    } else {
    unconstrained_length:
        f = (const ASN1SequenceOfCType *)p;
        ss->type = f->type;
        ss->elem_size = f->elem_size;
        ret = asn1_per_decode_ulength_items(s, asn1_per_decode_sequence_of1,
                                            ss, NULL);
        if (ret)
            return ret;
        if (str->len < range_min)
            return -1;
    }
#ifdef DEBUG
    printf("seq of: len=%d\n", (int)str->len);
#endif
    return 0;
}

static int asn1_per_decode_choice(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    int has_ext, nb_fields, nb_ext_fields;
    uint32_t choice_val, data_offset, choice_offset, b;
    const ASN1ChoiceField *f;

    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    if (has_ext) {
        nb_ext_fields = *p++;
    } else {
        nb_ext_fields = 0;
    }
    p++;
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    
    if (has_ext) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b != 0) {
            /* extension present */
            if (asn1_per_decode_normally_small_non_neg_whole_number(s, 
                                                             &choice_val))
                return -1;
            choice_val += nb_fields;
            *(uint32_t *)(data + choice_offset) = choice_val;
            if (choice_val < nb_fields + nb_ext_fields) {
                /* known extension */
                f = (const ASN1ChoiceField *)p + choice_val;
                return asn1_per_decode_open_type(s, f->type, 
                                                 data + data_offset);
            } else {
                /* skip extension */
                if (asn1_per_skip_open_type(s))
                    return -1;
                return 0;
            }
        }
    }
    /* normal case */
    if (asn1_per_decode_constrained_whole_number(s, 0, 
                                                 nb_fields - 1, &choice_val))
        return -1;
    *(uint32_t *)(data + choice_offset) = choice_val;
    f = (const ASN1ChoiceField *)p + choice_val;
    return asn1_per_decode_type(s, f->type, data + data_offset);
}

static int asn1_per_decode_enumerated(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    int has_ext, nb_fields;
    uint32_t val, b;

    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    
    if (has_ext) {
        if (asn1_get_bits(s, 1, &b))
            return -1;
        if (b != 0) {
            if (asn1_per_decode_normally_small_non_neg_whole_number(s, &val))
                return -1;
            val += nb_fields;
            goto the_end;
        }
    }
    if (asn1_per_decode_constrained_whole_number(s, 0, 
                                                 nb_fields - 1, &val))
        return -1;
 the_end:
    *(uint32_t *)data = val;
    return 0;
}

static int asn1_per_decode_object_identifier(ASN1DecodeState *s, const ASN1CType *p, 
                                             uint8_t *data)
{
    ASN1String *str;
    str = (ASN1String *)data;
    return asn1_per_decode_ulength_items(s, 
                                         asn1_per_decode_octet_string1, str,
                                         NULL);
}

static int asn1_per_decode_any(ASN1DecodeState *s, const ASN1CType *p, 
                               uint8_t *data)
{
    ASN1OpenType *str;

    str = (ASN1OpenType *)data;
    
    str->type = (ASN1CType *)asn1_get_constrained_type(s->top_value, p);
    if (!str->type) {
        return asn1_per_decode_ulength_items(s, 
                                             asn1_per_decode_octet_string1, 
                                             &str->u.octet_string,
                                             NULL);
    } else {
        str->u.data = asn1_mallocz_value(str->type);
        if (!str->u.data)
            return mem_error(s);
        return asn1_per_decode_open_type(s, str->type, str->u.data);
    }
}

static int asn1_per_decode_real(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    ASN1String str_s, *str = &str_s;
    int ret;

    str->buf = NULL;
    str->len = 0;
    ret = asn1_per_decode_ulength_items(s, 
                                        asn1_per_decode_octet_string1, str,
                                        NULL);
    if (ret) {
        asn1_free(str->buf);
        return ret;
    }
    ret = asn1_decode_real_ber(str->buf, str->len, (double *)data);
    asn1_free(str->buf);
    if (ret)
        return asn1_decode_error(s, "real: error in encoding");
    return 0;
}

typedef struct {
    ASN1String *str;
    int shift;
    int bits_per_char;
    int nb_ranges;
    const ASN1CType *ranges;
} CharStringDecodeState;

static int asn1_per_decode_char_string1(ASN1DecodeState *s, 
                                        uint32_t base, uint32_t len, 
                                        void *opaque)
{
    CharStringDecodeState *ss = opaque;
    ASN1String *str = ss->str;
    uint8_t *buf;
    uint32_t val, c, i, l;
    int j;

    /* XXX: potentially unbounded malloc if ss->bits_per_char = 0, but
       it only happens with character constraints containing a single
       character which is not common at all and not clearly defined in
       the spec. */
    buf = asn1_realloc2(str->buf, base + len, 1 << ss->shift);
    if (!buf)
        return mem_error(s);
    str->buf = buf;
    str->len = base + len;
    
    buf += base << ss->shift;
    
    for(i = 0; i < len; i++) {
        if (unlikely(ss->bits_per_char == 0)) {
            val = 0;
        } else {
            if (asn1_get_bits(s, ss->bits_per_char, &val))
                return -1;
        }
        if (ss->nb_ranges == 0) {
            c = val;
        } else {
            j = 0;
            for(;;) {
                l = ss->ranges[2 * j + 1] - ss->ranges[2 * j];
                if (val <= l) {
                    c = ss->ranges[2 * j] + val;
                    break;
                }
                val -= l + 1;
                j++;
                if (j >= ss->nb_ranges)
                    return asn1_decode_error(s, "character outside range");
            }
        }
        if (ss->shift == 0)
            ((uint8_t *)buf)[i] = c;
        else if (ss->shift == 1)
            ((uint16_t *)buf)[i] = c;
        else
            ((uint32_t *)buf)[i] = c;
    }
    return 0;
}

static int asn1_per_decode_char_string(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    ASN1String *str;
    uint32_t flags;
    int char_string_type, ret;

    str = (ASN1String *)data;
    flags = *p++;
    char_string_type = *p++;
    if (char_string_type > ASN1_CSTR_UniversalString) {
        ret = asn1_per_decode_ulength_items(s, 
                                            asn1_per_decode_octet_string1, str,
                                            NULL);
        if (ret)
            return ret;
    } else {
        uint32_t range_min, range_max, char_count, char_max, len, b;
        int nb_ranges, bits_per_char, i;
        const ASN1CType *ranges;
        CharStringDecodeState ss_s, *ss = &ss_s;
        
        range_min = *p++;
        range_max = UINT32_MAX;
        if (flags & ASN1_CTYPE_HAS_HIGH)
            range_max = *p++;
        nb_ranges = *p++;
        ranges = p;

        if (char_string_type == ASN1_CSTR_BMPString)
            ss->shift = 1;
        else if (char_string_type == ASN1_CSTR_UniversalString)
            ss->shift = 2;
        else
            ss->shift = 0;
        char_count = 0;
        if (nb_ranges == 0)
            return -1;
        for(i = 0; i < nb_ranges; i++)
            char_count += ranges[2 * i + 1] - ranges[2 * i] + 1;
        char_count--;
        if (char_count == 0) {
            /* the spec does not handle this case specifically, but
               the formula implies 0 bit per char */
            bits_per_char = 0;
        } else {
            bits_per_char = 32 - clz32(char_count); 
            if (s->aligned_per) {
                /* align to a power of two */
                i = 32 - clz32(bits_per_char - 1);
                bits_per_char = 1 << i;
            }
        }
        ss->bits_per_char = bits_per_char;
        if (bits_per_char == 32)
            char_max = UINT32_MAX;
        else
            char_max = (1 << bits_per_char) - 1;
        /* see if character transcoding is needed */
        if (ranges[2 * (nb_ranges - 1) + 1] <= char_max) {
            ss->nb_ranges = 0;
        } else {
            ss->nb_ranges = nb_ranges;
            ss->ranges = ranges;
        }
#ifdef DEBUG
        printf("char string: type=%d shift=%d bps=%d trans=%d\n", 
               char_string_type, 
               ss->shift, ss->bits_per_char, ss->nb_ranges != 0);
#endif
        ss->str = str;
        
        if (flags & ASN1_CTYPE_HAS_EXT) {
            if (asn1_get_bits(s, 1, &b))
                return -1;
            if (b) {
                range_min = 0;
                goto unconstrained_length;
            }
        }

        if (flags & ASN1_CTYPE_HAS_HIGH) {
            if (range_max >= 65536) 
                goto unconstrained_length;
            if (asn1_per_decode_constrained_whole_number(s, range_min, range_max, 
                                                         &len))
                return -1;
            if (range_min == range_max) {
                if (range_max * bits_per_char > 16)
                    asn1_align(s);
            } else {
                if (range_max * bits_per_char >= 16)
                    asn1_align(s);
            }
            ret = asn1_per_decode_char_string1(s, 0, len, ss);
            if (ret)
                return ret;
        } else {
        unconstrained_length:
            ret = asn1_per_decode_ulength_items(s, 
                                                asn1_per_decode_char_string1, ss,
                                                NULL);
            if (ret)
                return ret;
            if ((uint32_t)str->len < range_min)
                return -1;
        }
    }
    return 0;
}

static int asn1_per_decode_tagged(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    uint8_t *data1;
    int flags;

    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data1 = asn1_mallocz_value((ASN1CType *)p[1]);
        if (!data1)
            return mem_error(s);
        *(void **)data = data1;
    } else {
        data1 = data;
    }
    return asn1_per_decode_type(s, (ASN1CType *)p[1], data1);
}

static int asn1_per_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    int type, ret;

    type = ASN1_GET_CTYPE(p[0]);
#ifdef DEBUG
    printf("type=%s\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?");
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_per_decode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_per_decode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_per_decode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_per_decode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_per_decode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_per_decode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_per_decode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_per_decode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_per_decode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_per_decode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_per_decode_any(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_per_decode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_per_decode_char_string(s, p, data);
        break;
    default:
        ret = asn1_decode_error(s, "unsupported type (%d)", type);
        break;
    }
    return ret;
}

/* PER decoding. Return the number of consumed bytes or -1 if
   error. */
static asn1_ssize_t asn1_per_decode(void **pdata, const ASN1CType *p,
                                const uint8_t *buf, size_t buf_len,
                                BOOL aligned_per, ASN1Error *err)
{
    ASN1DecodeState s_s, *s = &s_s;
    int ret;
    void *data;

    s->error.bit_pos = 0;
    s->error.msg[0] = '\0';
    s->top_value = NULL;

    data = asn1_mallocz_value(p);
    if (!data) {
        mem_error(s);
        goto fail;
    }
    
    asn1_get_bits_init(s, buf, buf_len, aligned_per);

    ret = asn1_per_decode_type(s, p, data);
    if (ret < 0) {
        asn1_free_value(p, data);
    fail:
        if (err)
            *err = s->error;
        *pdata = NULL;
        return -1;
    } else {
        *pdata = data;
        return (s->buf_index * 8 - s->bit_count + 7) >> 3;
    }
}

asn1_ssize_t asn1_uper_decode(void **pdata, const ASN1CType *p,
                          const uint8_t *buf, size_t buf_len, ASN1Error *err)
{
    return asn1_per_decode(pdata, p, buf, buf_len, FALSE, err);
}

asn1_ssize_t asn1_aper_decode(void **pdata, const ASN1CType *p,
                          const uint8_t *buf, size_t buf_len, ASN1Error *err)
{
    return asn1_per_decode(pdata, p, buf, buf_len, TRUE, err);
}
