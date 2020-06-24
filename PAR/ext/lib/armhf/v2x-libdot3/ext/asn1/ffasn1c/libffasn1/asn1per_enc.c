/* 
 * ASN1 PER encoder
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
//#define DEBUG_PUT_BITS

typedef struct ASN1PutBitState {
    ASN1ByteBuffer bb;
    int bit_count; /* current number of bits in bit_buf */
    unsigned int bit_buf; /* bit buffer, starting from MSB */
    BOOL aligned_per;
    ASN1Error error;
} ASN1PutBitState;

static inline size_t get_bit_count(ASN1PutBitState *s)
{
    return s->bb.len * 8 + s->bit_count;
}

static int asn1_encode_error(ASN1PutBitState *s, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    s->error.bit_pos = get_bit_count(s);
    vsnprintf(s->error.msg, sizeof(s->error.msg), fmt, ap);
    va_end(ap);
    return -1;
}

static int mem_error(ASN1PutBitState *s)
{
    return asn1_encode_error(s, "not enough memory");
}
                              
static void asn1_put_bits_init(ASN1PutBitState *s, BOOL aligned_per)
{
    asn1_byte_buffer_init(&s->bb);
    s->bit_count = 0;
    s->bit_buf = 0;
    s->aligned_per = aligned_per;
}

/* 1 <= n <= 32 */
/* XXX: not optimized */
static void asn1_put_bits(ASN1PutBitState *s, int n, unsigned int val)
{
    int bit_count, k;

#ifdef DEBUG_PUT_BITS
    printf("put_bits: n=%d val=0x%x\n", 
           n, val);
#endif
    bit_count = s->bit_count;
    k = 32 - bit_count - n;
    if (likely(k > 0)) {
        s->bit_buf |= val << k;
        bit_count += n;
    } else {
        k = -k;
        s->bit_buf |= val >> k;
        asn1_put_be32(&s->bb, s->bit_buf);
        bit_count = k;
        if (k == 0)
            s->bit_buf = 0;
        else
            s->bit_buf = val << (32 - k);
    }
    s->bit_count = bit_count;
}

static void asn1_put_bits_align8(ASN1PutBitState *s)
{
    int n;
    n = (-s->bit_count) & 7;
    if (n != 0) {
        asn1_put_bits(s, n, 0);
    }
}

static asn1_exception int asn1_put_bits_flush(ASN1PutBitState *s)
{
    while (s->bit_count >= 8) {
        asn1_put_byte(&s->bb, s->bit_buf >> 24);
        s->bit_buf <<= 8;
        s->bit_count -= 8;
    }
    if (s->bit_count > 0) {
        asn1_put_byte(&s->bb, s->bit_buf >> 24);
        s->bit_buf = 0;
        s->bit_count = 0;
    }
    if (s->bb.has_error)
        return -1;
    else
        return 0;
}

static void asn1_align(ASN1PutBitState *s)
{
    if (s->aligned_per)
        asn1_put_bits_align8(s);
}

/* contrainted signed or unsigned 32 bit number */
static int asn1_per_encode_constrained_whole_number(ASN1PutBitState *s, 
                                                 int range_min, int range_max,
                                                 uint32_t val)
{
    int n;
    uint32_t diff, v;
    
    diff = range_max - range_min;
    val -= range_min;
    if (val > diff) {
        val += range_min;
        if (range_min < 0)
            return asn1_encode_error(s, "integer outside range (%d %d-%d)",
                                     val, range_min, range_max);
        else
            return asn1_encode_error(s, "integer outside range (%u %u-%u)",
                                     val, range_min, range_max);
    }
    if (diff == 0) {
        /* nothing to encode */
    } else {
        if (diff <= 254 || !s->aligned_per) {
            n = 32 - clz32(diff);
            asn1_put_bits(s, n, val);
        } else if (diff == 255) {
            asn1_align(s);
            asn1_put_bits(s, 8, val);
        } else if (diff <= 0xffff) {
            asn1_align(s);
            asn1_put_bits(s, 16, val);
        } else {
            /* compute number of bytes (1 <= n <= 4) */
            v = val;
            n = 0;
            do {
                v >>= 8;
                n++;
            } while (v != 0);
            asn1_put_bits(s, 2, n - 1);
            asn1_align(s);
            asn1_put_bits(s, n * 8, val);
        }
    }
    return 0;
}

/* encoding of len items with unconstrainted length with fragmentation
   handling. Note: base is always multiple of 16K */
static int asn1_per_encode_ulength_items(ASN1PutBitState *s, 
                                         int (*encode_buf)(ASN1PutBitState *, uint32_t base, uint32_t len, void *opaque),
                                         void *opaque,
                                         uint32_t len)
{
    uint32_t l, v, base;
    int ret;

    base = 0;
    for(;;) {
        l = len;
        if (l >= 16384) {
            v = l >> 14;
            if (v >= 4)
                v = 4;
            l = v << 14;
            /* fragmented case */
            asn1_align(s);
            asn1_put_bits(s, 8, 0xc0 | v);
            ret = encode_buf(s, base, l, opaque);
            if (ret)
                return ret;
            base += l;
            len -= l;
        } else {
            asn1_align(s);
            if (l <= 127) {
                asn1_put_bits(s, 8, l);
            } else {
                asn1_put_bits(s, 16, l | 0x8000);
            }
            if (l > 0) {
                ret = encode_buf(s, base, l, opaque);
                if (ret)
                    return ret;
            }
            break;
        }
    }
    return 0;
}

/* used only for small lengths */
static int asn1_per_encode_unconstrained_length(ASN1PutBitState *s,
                                                uint32_t val)
{
    asn1_align(s);
    if (val <= 127) {
        asn1_put_bits(s, 8, val);
    } else if (val <= 16383) {
        asn1_put_bits(s, 16, val | 0x8000);
    } else {
        return -1;
    }
    return 0;
}

static int asn1_per_encode_semi_constrained_whole_number(ASN1PutBitState *s, 
                                                         int range_min,
                                                         uint32_t val)
{
    uint32_t v, n;

    if (range_min < 0) {
        /* assume signed 32 bit */
        if ((int)val < (int)range_min)
            return asn1_encode_error(s, "integer < lower end (%d < %d)",
                                     val, range_min);
    } else {
        if ((uint32_t)val < (uint32_t)range_min)
            return asn1_encode_error(s, "integer < lower end (%u < %u)",
                                     val, range_min);
    }
    val -= range_min;
    v = val;
    n = 0;
    do {
        v >>= 8;
        n++;
    } while (v != 0);
    asn1_per_encode_unconstrained_length(s, n);
    asn1_put_bits(s, 8 * n, val);
    return 0;
}

/* no constraint or only upper constraint: we still limit to 32 bits */
static int asn1_per_encode_unconstrained_whole_number(ASN1PutBitState *s, 
                                                      int val)
{
    int n, shift, mask;
    
    n = 1;
    shift = 24;
    while (((val << shift) >> shift) != val) {
        n++;
        shift -= 8;
    }
    
    if (n == 4)
        mask = -1;
    else
        mask = ((1 << (8 * n)) - 1);
    asn1_per_encode_unconstrained_length(s, n);
    asn1_put_bits(s, 8 * n, val & mask);
    return 0;
}

static int asn1_per_encode_normally_small_non_neg_whole_number(ASN1PutBitState *s, 
                                                               uint32_t val)
{
    uint32_t v;
    int n;

    if (val <= 63) {
        asn1_put_bits(s, 7, val);
    } else {
        /* compute number of bytes (1 <= n <= 4) */
        v = val;
        n = 0;
        do {
            v >>= 8;
            n++;
        } while (v != 0);
        asn1_put_bits(s, 1, 1);
        asn1_per_encode_unconstrained_length(s, n);
        asn1_put_bits(s, 8 * n, val);
    }
    return 0;
}

/* val >= 1 */
static int asn1_per_encode_normally_small_length(ASN1PutBitState *s,
                                                 uint32_t val)
{
    int ret;
    if (val == 0)
        return -1;

    if (val <= 64) {
        asn1_put_bits(s, 7, val - 1);
        ret = 0;
    } else {
        asn1_put_bits(s, 1, 1);
        ret = asn1_per_encode_unconstrained_length(s, val);
    }
    return ret;
}

static int asn1_per_encode_boolean(ASN1PutBitState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int val;
    val = *(BOOL *)data != 0;
    asn1_put_bits(s, 1, val);
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
static void encode_large_positive_integer(ASN1PutBitState *s, 
                                          const ASN1Integer *r)
{
    uint32_t v;
    int n, i;

    if (r->len == 0) {
        asn1_put_bits(s, 8, 0);
    } else {
        v = r->data[r->len - 1];
        n = 0;
        do {
            v >>= 8;
            n++;
        } while (v != 0);
        asn1_put_bits(s, 8 * n, r->data[r->len - 1]);
        for(i = r->len - 2; i >= 0; i--) {
            asn1_put_bits(s, 32, r->data[i]);
        }
    }
}

static asn1_exception int encode_large_integer(ASN1PutBitState *s, 
                                               const ASN1Integer *r)
{
    int len, nb_bits, i, l;
    uint32_t mask;
    ASN1Integer *q, tmp;

    if (r->len == 0) {
        asn1_per_encode_unconstrained_length(s, 1);
        asn1_put_bits(s, 8, 0);
    } else {
        q = asn1_integer_to_2comp(&tmp, r, &nb_bits);
        if (!q)
            return mem_error(s);
        len = (nb_bits + 8) >> 3;
        asn1_per_encode_unconstrained_length(s, len);
        
        if ((nb_bits & (ASN1_LIMB_BITS - 1)) == 0) {
            asn1_put_bits(s, 8, (-r->negative) & 0xff); /* extra byte */
            len--;
        }
        if (nb_bits != 0) {
            /* first limb */
            l = len & 3;
            if (l == 0) {
                l = 4;
                mask = -1;
            } else {
                mask = (1 << (l * 8)) - 1;
            }
            asn1_put_bits(s, l * 8, q->data[q->len - 1] & mask);
            /* next limbs */
            for(i = q->len - 2; i >= 0; i--) {
                asn1_put_bits(s, 32, q->data[i]);
            }
        }
        if (q == &tmp)
            asn1_integer_clear(q);
    }
    return 0;
}

static int asn1_per_encode_large_integer(ASN1PutBitState *s, const ASN1CType *p, 
                                         const uint8_t *data)
{
    const ASN1Integer *range_min, *range_max, *val;
    ASN1Integer d_s, *d = &d_s, diff_s, *diff = &diff_s;
    int flags, out_of_range;

    val = (const ASN1Integer *)data;
#if defined(DEBUG)
    {
        printf("encode_large_integer: ");
        asn1_integer_out_str(stdout, val);
        printf("\n");
    }  
#endif
    flags = *p++;
    range_min = NULL;
    range_max = NULL;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = (const ASN1Integer *)*p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = (const ASN1Integer *)*p++;

    out_of_range = (range_min && asn1_integer_cmp(val, range_min) < 0) ||
        (range_max && asn1_integer_cmp(val, range_max) > 0);
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (out_of_range) {
            asn1_put_bits(s, 1, 1);
            goto unconstrained;
        }
        asn1_put_bits(s, 1, 0);
    } else {
        if (out_of_range)
            return -1;
    }
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        asn1_integer_init(diff);
        if (asn1_integer_sub(diff, range_max, range_min)) {
            asn1_integer_clear(diff);
            return mem_error(s);
        }
        asn1_integer_init(d);
        if (asn1_integer_sub(d, val, range_min)) {
            asn1_integer_clear(diff);
            asn1_integer_clear(d);
            return mem_error(s);
        }
        if (diff->len == 0) {
            /* nothing to encode */
        } else if (!s->aligned_per || asn1_integer_cmp_si(diff, 254) <= 0) {
            int nb_bits, l, i;
            ASN1Limb v;

            nb_bits = asn1_integer_get_bit_size(diff);
            l = nb_bits & (ASN1_LIMB_BITS - 1);
            if (l == 0) 
                l = ASN1_LIMB_BITS;
            
            i = diff->len - 1;
            if (i < d->len)
                v = d->data[i];
            else
                v = 0;
            asn1_put_bits(s, l, v);
            for(i = diff->len - 2; i >= 0; i--) {
                if (i < d->len)
                    v = d->data[i];
                else
                    v = 0;
                asn1_put_bits(s, 32, v);
            }
        } else if (asn1_integer_cmp_si(diff, 255) == 0) {
            asn1_align(s);
            asn1_put_bits(s, 8, d->data[0]);
        } else if (asn1_integer_cmp_si(diff, 0xffff) <= 0) {
            asn1_align(s);
            asn1_put_bits(s, 16, d->data[0]);
        } else {
            int max_bytes, length_bits, nb_bytes;
            max_bytes = (asn1_integer_get_bit_size(diff) + 7) / 8;
            length_bits = 32 - clz32(max_bytes - 1);
            /* XXX: spec not clear */
            nb_bytes = (asn1_integer_get_bit_size(d) + 7) / 8;
            if (nb_bytes == 0)
                nb_bytes = 1;
            asn1_put_bits(s, length_bits, nb_bytes - 1);
            asn1_align(s);
            encode_large_positive_integer(s, d);
        }
        asn1_integer_clear(d);
        asn1_integer_clear(diff);
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        int nb_bytes;
        asn1_integer_init(d);
        if (asn1_integer_sub(d, val, range_min)) {
            asn1_integer_clear(d);
            return mem_error(s);
        }
        nb_bytes = (asn1_integer_get_bit_size(d) + 7) / 8;
        if (nb_bytes == 0)
            nb_bytes = 1;
        asn1_per_encode_unconstrained_length(s, nb_bytes);
        encode_large_positive_integer(s, d);
        asn1_integer_clear(d);
    } else {
    unconstrained: ;
        if (encode_large_integer(s, val))
            return -1;
    }
    return 0;
}
#endif

/* contrainted signed or unsigned 32 bit number */
static int asn1_per_encode_integer(ASN1PutBitState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int range_min, range_max, flags, val, in_range;
    
    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_per_encode_large_integer(s, p, data);
#else
        return -1;
#endif
    }
    p++;
    val = *(int *)data;
    range_min = INT32_MIN;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = *p++;
    } else {
        if (range_min < 0)
            range_max = INT32_MAX;
        else
            range_max = UINT32_MAX;
    }
    
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (range_min < 0)
            in_range = (val >= range_min && val <= range_max);
        else
            in_range = ((uint32_t)val >= (uint32_t)range_min && 
                        (uint32_t)val <= (uint32_t)range_max);
        asn1_put_bits(s, 1, !in_range);
        if (!in_range)
            goto unconstrained;
    }

    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        return asn1_per_encode_constrained_whole_number(s, 
                                                        range_min, range_max, 
                                                        val);
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        return asn1_per_encode_semi_constrained_whole_number(s, range_min, val);
    } else {
        if (val > range_max)
            return asn1_encode_error(s, "integer < higher end (%d > %d)",
                                     val, range_max);
    unconstrained:
        return asn1_per_encode_unconstrained_whole_number(s, val);
    }
}

static int asn1_per_encode_bits(ASN1PutBitState *s, 
                                uint32_t base, uint32_t len,
                                void *opaque)
{
    const uint8_t *buf = opaque;
    uint32_t i, n, k;

    buf += base >> 3;
    n = len >> 3;
    k = len & 7;
    /* byte output */
    for(i = 0; i < n; i++) {
        asn1_put_bits(s, 8, buf[i]);
    }
    if (k != 0) {
        asn1_put_bits(s, k, buf[n] >> (8 - k));
    }
    return 0;
}

static int asn1_per_encode_bit_string(ASN1PutBitState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    uint32_t range_min, range_max, in_range;
    ASN1BitString *str;
    int ret, flags;

    str = (ASN1BitString *)data;
#ifdef DEBUG
    printf("bit string: len=%d\n", str->len);
#endif
    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    if (flags & ASN1_CTYPE_HAS_EXT) {
        in_range = (str->len >= range_min && str->len <= range_max);
        asn1_put_bits(s, 1, !in_range);
        if (!in_range) {
            range_min = 0;
            goto unconstraint_length;
        }
    }

    if (flags & ASN1_CTYPE_HAS_HIGH) {
        if (range_max >= 65536)
            goto unconstraint_length;
        ret = asn1_per_encode_constrained_whole_number(s, range_min, range_max, 
                                                       str->len);
        if (ret)
            return ret;
        if (range_min == range_max && range_max <= 16) {
            /* no alignment */
        } else {
            asn1_align(s);
        }
        asn1_per_encode_bits(s, 0, str->len, str->buf);
    } else {
    unconstraint_length:
        if  ((uint32_t)str->len < range_min)
            return -1;
        ret = asn1_per_encode_ulength_items(s, 
                                            asn1_per_encode_bits,
                                            str->buf, str->len);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_per_encode_buf1(ASN1PutBitState *s, 
                                uint32_t base, uint32_t len,
                                void *opaque)
{
    const uint8_t *buf = opaque;
    uint32_t i;

    buf += base;
    for(i = 0; i < len; i++)
        asn1_put_bits(s, 8, buf[i]);
    return 0;
}

static int asn1_per_encode_octet_string(ASN1PutBitState *s, const ASN1CType *p, 
                                        const uint8_t *data)
{
    uint32_t range_min, range_max, in_range;
    ASN1String *str;
    int ret, flags;

    str = (ASN1String *)data;
#ifdef DEBUG
    printf("octet string: len=%d\n", str->len);
#endif
    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    if (flags & ASN1_CTYPE_HAS_EXT) {
        in_range = (str->len >= range_min && str->len <= range_max);
        asn1_put_bits(s, 1, !in_range);
        if (!in_range) {
            range_min = 0;
            goto unconstraint_length;
        }
    }
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        if (range_max >= 65536)
            goto unconstraint_length;
        ret = asn1_per_encode_constrained_whole_number(s, range_min, range_max, 
                                                       str->len);
        if (ret)
            return ret;
        if (range_min == range_max && range_max <= 2) {
            /* no alignment */
        } else {
            asn1_align(s);
        }
        asn1_per_encode_buf1(s, 0, str->len, str->buf);
    } else {
    unconstraint_length:
        if  ((uint32_t)str->len < range_min)
            return -1;
        ret = asn1_per_encode_ulength_items(s, 
                                            asn1_per_encode_buf1,
                                            str->buf, str->len);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_per_encode_type(ASN1PutBitState *s, const ASN1CType *p, 
                                const uint8_t *data);

static int asn1_per_encode_open_type(ASN1PutBitState *s, const ASN1CType *type, 
                                     const uint8_t *data)
{
    ASN1PutBitState s1_s, *s1 = &s1_s;
    int ret;
    
    asn1_put_bits_init(s1, s->aligned_per);
    ret = asn1_per_encode_type(s1, type,  data);
    if (ret)
        goto fail;
    if (get_bit_count(s1) == 0) {
        asn1_put_bits(s1, 8, 0); /* must contain at least
                                    one byte */
    }
    if (asn1_put_bits_flush(s1)) {
        ret = mem_error(s);
        goto fail;
    }
        
    ret = asn1_per_encode_ulength_items(s, 
                                        asn1_per_encode_buf1,
                                        s1->bb.buf, s1->bb.len);
 fail:
    asn1_free(s1->bb.buf);
    return ret;
}

static int asn1_per_encode_ext_group(ASN1PutBitState *s, 
                                     const uint8_t *data,
                                     const ASN1SequenceField *f1,
                                     const uint8_t *table_present, 
                                     int nb_group_fields)
{
    int j, ret, flag;
    const ASN1SequenceField *f;
    ASN1PutBitState s1_s, *s1 = &s1_s;

    asn1_put_bits_init(s1, s->aligned_per);
    
    /* bit mask for DEFAULT and OPTIONAL fields */
    for(j = 0; j < nb_group_fields; j++) {
        f = f1 + j;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag != ASN1_SEQ_FLAG_NORMAL) {
            asn1_put_bits(s1, 1, table_present[j] & 1);
        }
    }
    
    ret = 0;
    for(j = 0; j < nb_group_fields; j++) {
        if (table_present[j] & 1) {
            f = f1 + j;
            ret = asn1_per_encode_type(s1, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto done;
        }
    }
    
    if (get_bit_count(s1) == 0) {
        asn1_put_bits(s1, 8, 0); /* must contain at least
                                    one byte */
    }
    if (asn1_put_bits_flush(s1)) {
        ret = mem_error(s);
        goto done;
    }
    
    ret = asn1_per_encode_ulength_items(s, 
                                        asn1_per_encode_buf1,
                                        s1->bb.buf, s1->bb.len);
 done:
    asn1_free(s1->bb.buf);
    return ret;
}

static int asn1_per_encode_sequence(ASN1PutBitState *s, const ASN1CType *p, 
                                    const uint8_t *data)
{
    int has_ext, nb_fields, nb_ext_fields, extension_present, i;
    int option_bit, ret, j, nb_group_fields;
    const ASN1SequenceField *f, *f1;
    int flag;
    uint32_t val;
    uint8_t *table_present;
    
    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;

    /* compute if each field is present or not (bit 0) */
    table_present = asn1_malloc(nb_fields);
    if (!table_present)
        return -1;
    extension_present = 0;
    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            val = *(BOOL *)(data + f->u.option_offset);
            option_bit = (val != 0);
        } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
            val = *(uint32_t *)(data + ASN1_GET_SEQ_OFFSET(f));
            option_bit = (val != f->u.default_value);
        } else {
            option_bit = 1;
        }
        table_present[i] = option_bit;
        if (ASN1_IS_SEQ_EXT(f))
            extension_present |= option_bit;
    }

    if (has_ext) {
        /* write extension present bit */
        asn1_put_bits(s, 1, extension_present);
    }
    
    /* write option bits */
    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f)) {
            flag = ASN1_GET_SEQ_FLAG(f);
            if (flag != ASN1_SEQ_FLAG_NORMAL) {
                asn1_put_bits(s, 1, table_present[i]);
            }
        }
    }
    
    /* write sequence values */
    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f) && table_present[i]) {
#ifdef DEBUG
            printf("seq field=%s\n", f->name);
#endif
            ret = asn1_per_encode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto fail;
        }
    }

    if (extension_present) {

        /* compute the number of extension fields and compute if the
           extension groups are present */
        nb_ext_fields = 0;
        i = 0;
        while (i < nb_fields) {
            f = f1 + i;
            if (!ASN1_IS_SEQ_EXT(f)) {
                i++;
            } else {
                if (ASN1_IS_SEQ_EXT_GROUP_START(f)) {
                    option_bit = 0;
                    j = i;
                    for(;;) {
                        f = f1 + j;
                        option_bit |= table_present[j];
                        if (ASN1_IS_SEQ_EXT_GROUP_END(f))
                            break;
                        j++;
                    }
                    table_present[i] |= (option_bit << 1) | 4;
                    i = j + 1;
                } else {
                    option_bit = table_present[i];
                    table_present[i] |= (option_bit << 1) | 4;
                    i++;
                }
                nb_ext_fields++;
            }
        }
        
        ret = asn1_per_encode_normally_small_length(s, nb_ext_fields);
        if (ret)
            goto fail;
        /* extension present bit mask */
        for(i = 0; i < nb_fields; i++) {
            if (table_present[i] & 4) {
                asn1_put_bits(s, 1, (table_present[i] >> 1) & 1);
            }
        }

        /* extensions */
        for(i = 0; i < nb_fields; i++) {
            if (table_present[i] & 2) {
                f = f1 + i;
#ifdef DEBUG
                printf("seq ext field=%s\n", f->name);
#endif
                if (ASN1_IS_SEQ_EXT_GROUP_START(f)) {
                    /* compute the number of fields in extension group */
                    nb_group_fields = 0;
                    for(;;) {
                        f = f1 + i + nb_group_fields;
                        nb_group_fields++;
                        if (ASN1_IS_SEQ_EXT_GROUP_END(f))
                            break;
                    }
                    ret = asn1_per_encode_ext_group(s, data,
                                                    f1 + i,
                                                    table_present + i,
                                                    nb_group_fields);
                    if (ret)
                        goto fail;
                } else {
                    ret = asn1_per_encode_open_type(s, f->type,
                                                    data + ASN1_GET_SEQ_OFFSET(f));
                    if (ret)
                        goto fail;
                }
            }
        }
    }
    ret = 0;
 fail:
    asn1_free(table_present);
    return ret;
}

typedef struct {
    const uint8_t *buf;
    int elem_size;
    ASN1CType *type;
} PERSequenceOfState;

static int asn1_per_encode_sequence_of1(ASN1PutBitState *s, 
                                        uint32_t base, uint32_t len,
                                        void *opaque)
{
    PERSequenceOfState *ss = opaque;
    uint32_t i;
    int ret;

    for(i = 0; i < len; i++) {
        ret = asn1_per_encode_type(s, ss->type, 
                                   ss->buf + (base + i) * ss->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_per_encode_sequence_of(ASN1PutBitState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    PERSequenceOfState ss_s, *ss = &ss_s;
    const ASN1SequenceOfCType *f;
    uint32_t range_min, range_max;
    int ret, flags, in_range;
    ASN1String *str;

    str = (ASN1String *)data;
#ifdef DEBUG
    printf("seq of: len=%d\n", str->len);
#endif
    flags = *p++;
    range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = *p++;
    } else {
        range_max = UINT32_MAX;
    }
    if (flags & ASN1_CTYPE_HAS_EXT) {
        in_range = (str->len >= range_min && str->len <= range_max);
        asn1_put_bits(s, 1, !in_range);
        if (!in_range) {
            range_min = 0;
            goto unconstraint_length;
        }
    }
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        if (range_max >= 65536)
            goto unconstraint_length;
        ret = asn1_per_encode_constrained_whole_number(s, range_min, 
                                                       range_max, str->len);
        if (ret)
            return ret;
        f = (const ASN1SequenceOfCType *)p;
        ss->buf = str->buf;
        ss->elem_size = f->elem_size;
        ss->type = f->type;
        ret = asn1_per_encode_sequence_of1(s, 0, str->len, ss);
        if (ret)
            return ret;
    } else {
    unconstraint_length:
        if ((uint32_t)str->len < range_min)
            return -1;
        f = (const ASN1SequenceOfCType *)p;
        ss->buf = str->buf;
        ss->elem_size = f->elem_size;
        ss->type = f->type;
        ret = asn1_per_encode_ulength_items(s, asn1_per_encode_sequence_of1,
                                            ss, str->len);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_per_encode_choice(ASN1PutBitState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int has_ext, nb_fields, nb_ext_fields, ret;
    uint32_t choice_val, data_offset;
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
    choice_val = *(uint32_t *)(data + p[0]);
    data_offset = p[1];
    p += 2;
    
    /* check choice consistency */
    if (choice_val >= (nb_fields + nb_ext_fields))
        return -1;
    f = (const ASN1ChoiceField *)p + choice_val;
    if (choice_val < nb_fields) {
        if (has_ext) {
            asn1_put_bits(s, 1, 0);
        }
        asn1_per_encode_constrained_whole_number(s, 0, 
                                                 nb_fields - 1, choice_val);
        ret = asn1_per_encode_type(s, f->type, data + data_offset);
    } else {
        /* extension case */
        asn1_put_bits(s, 1, 1);
        ret = asn1_per_encode_normally_small_non_neg_whole_number(s, 
                                                                  choice_val - nb_fields);
        if (ret)
            return ret;
        ret = asn1_per_encode_open_type(s, f->type,  data + data_offset);
    }
    return ret;
}

static int asn1_per_encode_enumerated(ASN1PutBitState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    int has_ext, nb_fields, nb_ext_fields, ret;
    uint32_t val;

    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    if (has_ext) {
        nb_ext_fields = *p++;
    } else {
        nb_ext_fields = 0;
    }
    
    val = *(uint32_t *)data;
    if (val >= (nb_fields + nb_ext_fields))
        return -1;

    if (val < nb_fields) {
        if (has_ext) 
            asn1_put_bits(s, 1, 0);
        ret = asn1_per_encode_constrained_whole_number(s, 0, 
                                                       nb_fields - 1, val);
    } else {
        asn1_put_bits(s, 1, 1);
        ret = asn1_per_encode_normally_small_non_neg_whole_number(s, 
                                                            val - nb_fields);
    }
    return ret;
}            

static int asn1_per_encode_object_identifier(ASN1PutBitState *s, const ASN1CType *p, 
                                             const uint8_t *data)
{
    ASN1String *str;

    str = (ASN1String *)data;
    return asn1_per_encode_ulength_items(s, 
                                         asn1_per_encode_buf1,
                                         str->buf, str->len);
}

static int asn1_per_encode_any(ASN1PutBitState *s, const ASN1CType *p, 
                               const uint8_t *data)
{
    const ASN1OpenType *str;

    str = (const ASN1OpenType *)data;
    if (!str->type) {
        return asn1_per_encode_ulength_items(s, 
                                             asn1_per_encode_buf1,
                                             str->u.octet_string.buf, 
                                             str->u.octet_string.len);
    } else {
        return asn1_per_encode_open_type(s, str->type, str->u.data);
    }
}

static int asn1_per_encode_real(ASN1PutBitState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    uint8_t buf[ASN1_DOUBLE_DER_MAX_LEN];
    int len;
    
    len = asn1_encode_real_der(buf, *(double *)data);
    return asn1_per_encode_ulength_items(s, 
                                         asn1_per_encode_buf1,
                                         buf, len);
}

typedef struct {
    uint8_t *buf;
    int shift;
    int bits_per_char;
    int nb_ranges;
    const ASN1CType *ranges;
} CharStringEncodeState;

static int asn1_per_encode_char_string1(ASN1PutBitState *s, 
                                        uint32_t base, uint32_t len,
                                        void *opaque)
{
    CharStringEncodeState *ss = opaque;
    const uint8_t *buf;
    uint32_t i, c, a, r_min, r_max;
    int j;

    if (ss->bits_per_char != 0) {
        buf = ss->buf + (base << ss->shift);
        for(i = 0; i < len; i++) {
            if (ss->shift == 0)
                c = ((uint8_t *)buf)[i];
            else if (ss->shift == 1)
            c = ((uint16_t *)buf)[i];
            else
                c = ((uint32_t *)buf)[i];
            if (ss->nb_ranges == 0) {
                a = c;
            } else {
                j = 0;
                a = 0;
                for(;;) {
                    r_min = ss->ranges[2 * j];
                    r_max = ss->ranges[2 * j + 1];
                if (c >= r_min && c <= r_max) {
                    a += c - r_min;
                    break;
                }
                a += r_max - r_min + 1;
                j++;
                if (j >= ss->nb_ranges)
                    return -1;
                }
            }
            asn1_put_bits(s, ss->bits_per_char, a);
        }
    }
    return 0;
}

static int asn1_per_encode_char_string(ASN1PutBitState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    ASN1String *str;
    uint32_t flags;
    int char_string_type, ret;

    str = (ASN1String *)data;
    flags = *p++;
    char_string_type = *p++;
    if (char_string_type > ASN1_CSTR_UniversalString) {
#ifdef DEBUG
        printf("char string: len=%d\n", str->len);
#endif
        ret = asn1_per_encode_ulength_items(s, 
                                            asn1_per_encode_buf1,
                                            str->buf, str->len);
        if (ret)
            return ret;
    } else {
        uint32_t range_min, range_max, char_count, char_max;
        int nb_ranges, bits_per_char, i;
        const ASN1CType *ranges;
        CharStringEncodeState ss_s, *ss = &ss_s;
        
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
        printf("char string: type=%d len=%d shift=%d bps=%d trans=%d\n", 
               char_string_type, str->len, 
               ss->shift, ss->bits_per_char, ss->nb_ranges != 0);
#endif
        ss->buf = str->buf;
        if (flags & ASN1_CTYPE_HAS_EXT) {
            int in_range;
            in_range = (str->len >= range_min && str->len <= range_max);
            asn1_put_bits(s, 1, !in_range);
            if (!in_range) {
                range_min = 0;
                goto unconstraint_length;
            }
        }
        if (flags & ASN1_CTYPE_HAS_HIGH) {
            if (range_max >= 65536)
                goto unconstraint_length;
            ret = asn1_per_encode_constrained_whole_number(s, range_min, range_max, 
                                                           str->len);
            if (ret)
                return ret;
            if (range_min == range_max) {
                if (range_max * bits_per_char > 16)
                    asn1_align(s);
            } else {
                if (range_max * bits_per_char >= 16)
                    asn1_align(s);
            }
            ret = asn1_per_encode_char_string1(s, 0, str->len, ss);
            if (ret)
                return ret;
        } else {
        unconstraint_length:
            if  ((uint32_t)str->len < range_min)
                return -1;
            ret = asn1_per_encode_ulength_items(s, 
                                                asn1_per_encode_char_string1,
                                                ss, str->len);
            if (ret)
                return ret;
        }
    }
    return 0;
}

static int asn1_per_encode_tagged(ASN1PutBitState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int flags;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data = *(void **)data;
    }
    return asn1_per_encode_type(s, (ASN1CType *)p[1], data);
}

static int asn1_per_encode_type(ASN1PutBitState *s, const ASN1CType *p, 
                                const uint8_t *data)
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
        ret = asn1_per_encode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_per_encode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_per_encode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_per_encode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_per_encode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_per_encode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_per_encode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_per_encode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_per_encode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_per_encode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_per_encode_any(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_per_encode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_per_encode_char_string(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

static asn1_ssize_t asn1_per_encode(uint8_t **pbuf, const ASN1CType *p, 
                                    const void *data, ASN1Error *err, 
                                    BOOL aligned_per)
{
    ASN1PutBitState s_s, *s = &s_s;
    int ret;

    s->error.bit_pos = 0;
    s->error.msg[0] = '\0';

    asn1_put_bits_init(s, aligned_per);

    ret = asn1_per_encode_type(s, p, data);
    if (ret)
        goto fail;
    if (get_bit_count(s) == 0) {
        asn1_put_bits(s, 8, 0); /* must contain at least
                                    one byte */
    }

    if (asn1_put_bits_flush(s)) {
        ret = mem_error(s);
    fail:
        asn1_free(s->bb.buf);
        *pbuf = NULL;
        if (err)
            *err = s->error;
        return ret;
    } else {
        *pbuf = s->bb.buf;
        return s->bb.len;
    }
}

/* unaligned PER encoding. Return the encoded length (in bytes) and
   the allocated buffer. Return < 0 and *pbuf = NULL if error. */
asn1_ssize_t asn1_uper_encode(uint8_t **pbuf, const ASN1CType *p, const void *data)
{
    return asn1_per_encode(pbuf, p, data, NULL, FALSE);
}

asn1_ssize_t asn1_aper_encode(uint8_t **pbuf, const ASN1CType *p, const void *data)
{
    return asn1_per_encode(pbuf, p, data, NULL, TRUE);
}

asn1_ssize_t asn1_uper_encode2(uint8_t **pbuf, const ASN1CType *p, const void *data,
                           ASN1Error *err)
{
    return asn1_per_encode(pbuf, p, data, err, FALSE);
}

asn1_ssize_t asn1_aper_encode2(uint8_t **pbuf, const ASN1CType *p, const void *data,
                           ASN1Error *err)
{
    return asn1_per_encode(pbuf, p, data, err, TRUE);
}
