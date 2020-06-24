/* 
 * ASN1 OER decoder
 * Copyright (C) 2015-2018 Fabrice Bellard
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>

#include "asn1defs_int.h"

//#define DEBUG

#define INDEFINITE_LEN (-2)

typedef struct ASN1DecodeState {
    const uint8_t *buf;
    size_t buf_len;
    size_t buf_index;
    ASN1ValueStack *top_value;
    ASN1Error error;
} ASN1DecodeState;

static int asn1_oer_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data);
static int asn1_oer_decode_open_type(ASN1DecodeState *s, const ASN1CType *p,
                                     uint8_t *data);

static int asn1_error_internal(ASN1DecodeState *s)
{
    s->error.bit_pos = s->buf_index * 8;
    return -1;
}

static __attribute__((format(printf, 2, 3)))
    int asn1_error(ASN1DecodeState *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(s->error.msg, sizeof(s->error.msg), fmt, ap);
    va_end(ap);
    return asn1_error_internal(s);
}

static int mem_error(ASN1DecodeState *s)
{
    return asn1_error(s, "not enough memory");
}

static int eos_error(ASN1DecodeState *s)
{
    return asn1_error(s, "unexpected end of stream");
}

/* return -1 if end of stream */
static int get_byte(ASN1DecodeState *s)
{
    if (unlikely(s->buf_index >= s->buf_len))
        return eos_error(s);
    else
        return s->buf[s->buf_index++];
}

static int get_be16(ASN1DecodeState *s, int *pval)
{
    if (unlikely(s->buf_index + 1 >= s->buf_len)) {
        return eos_error(s);
    } else {
        *pval = (s->buf[s->buf_index] << 8) |
            s->buf[s->buf_index + 1];
        s->buf_index += 2;
        return 0;
    }
}

static int get_be32(ASN1DecodeState *s, int *pval)
{
    if (unlikely(s->buf_index + 3 >= s->buf_len)) {
        return eos_error(s);
    } else {
        *pval = (s->buf[s->buf_index] << 24) |
            (s->buf[s->buf_index + 1] << 16) |
            (s->buf[s->buf_index + 2] << 8) |
            s->buf[s->buf_index + 3];
        s->buf_index += 4;
        return 0;
    }
}

static int get_bytes(ASN1DecodeState *s, uint8_t *buf, int len)
{
    if (unlikely((s->buf_index + len) > s->buf_len)) {
        return eos_error(s);
    }
    memcpy(buf, s->buf + s->buf_index, len);
    s->buf_index += len;
    return 0;
}

/* return -1 if error. */
static int decode_tag(ASN1DecodeState *s)
{
    int tag, c, n, b;

    b = get_byte(s);
    if (b < 0)
        return -1;
    tag = b & 0x1f;
    if (tag == 0x1f) {
        c = get_byte(s);
        if (c < 0)
            return -1;
        tag = c & 0x7f;
        if (tag == 0)
            return -1; /* invalid */
        n = 1;
        while (c & 0x80) {
            c = get_byte(s);
            if (c < 0)
                return -1;
            tag = (tag << 7) | (c & 0x7f);
            n++;
        }
        if (n > 4)
            tag = ASN1_TAG_MAX; /* tag too large (more than 28
                                   bits). it will be ignored */
    }
    //    *pis_constructed = (b >> 5) & 1;
    tag |= (b & 0xc0) << (ASN1_TAG_CLASS_SHIFT - 6);
    return tag;
}

/* return -1 if error, INDEFINITE_LEN if indefinite length (< 0 too),
   otherwise the decoded length */ 
static int decode_len(ASN1DecodeState *s)
{
    int b, n, i;
    uint32_t len;

    b = get_byte(s);
    if (b < 0)
        return -1;
    if (b < 0x80) {
        return b;
    } else if (b == 0x80) {
        return INDEFINITE_LEN;
    } else {
        if (b == 0xff)
            return -1; /* reserved */
        n = b & 0x7f;
        /* Note: it is legal to use more bytes than necessary */
        len = 0;
        for(i = 0; i < n; i++) {
            b = get_byte(s);
            if (b < 0)
                return -1;
            if ((len >> 16) >= 0x80)
                return -1; /* length too large */
            len = (len << 8) | b;
        }
        return len;
   }
}

static int asn1_oer_decode_boolean(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    int b;

    b = get_byte(s);
    if (b < 0)
        return -1;
    *(BOOL *)data = (b != 0);
    return 0;
}

static int asn1_decode_int32_internal(ASN1DecodeState *s, int *pval, int len)
{
    int i, val, b, shift;
    if (len <= 0 || len > 4)
        return asn1_error(s, "invalid integer length");
    val = 0;
    for(i = 0; i < len; i++) {
        b = get_byte(s);
        if (b < 0)
            return -1;
        val = (val << 8) | b;
    }
    /* sign extend */
    shift = 32 - len * 8;
    *pval = (val << shift) >> shift;
    return 0;
}

static int asn1_decode_int32(ASN1DecodeState *s, int *pval)
{
    int len;
    len = decode_len(s);
    return asn1_decode_int32_internal(s, pval, len);
}

static int asn1_decode_uint32(ASN1DecodeState *s, int *pval)
{
    int i, val, b, len;

    len = decode_len(s);
    if (len <= 0 || len > 4)
        return asn1_error(s, "invalid integer length");
    val = 0;
    for(i = 0; i < len; i++) {
        b = get_byte(s);
        if (b < 0)
            return -1;
        val = (val << 8) | b;
    }
    *pval = val;
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
/* decode the larger integer 'r'. If nb_bytes = 0, use indefinite length. */
static int asn1_oer_decode_large_integer1(ASN1DecodeState *s, 
                                          ASN1Integer *r, 
                                          int nb_bytes,
                                          BOOL is_unsigned)
{
    int nb_limbs, i, l, b, len;
    ASN1Limb v;
    uint8_t buf[ASN1_LIMB_BYTES];
    
    if (nb_bytes == 0) {
        len = decode_len(s);
        if (len <= 0)
            return -1;
    } else {
        len = nb_bytes;
    }
#ifdef DEBUG
    printf("integer len=%d\n", len);
#endif

    asn1_integer_init(r);
    nb_limbs = (len + ASN1_LIMB_BYTES - 1) >> (ASN1_LIMB_BITS_LOG2 - 3);
    if (asn1_integer_resize(r, nb_limbs))
        return mem_error(s);
    l = len & (ASN1_LIMB_BYTES - 1);
    if (l == 0) {
        l = ASN1_LIMB_BYTES;
    }
    v = 0;
    for(i = l - 1; i >= 0; i--) {
        b = get_byte(s);
        if (b < 0)
            return -1;
        v |= b << (i * 8);
    }
    /* sign extend highest limb */
    if (!is_unsigned)
        v = ((int)v << (32 - l * 8)) >> (32 - l * 8);
    r->data[nb_limbs - 1] = v;
    for(i = nb_limbs - 2; i >= 0; i--) {
        if (get_bytes(s, buf, ASN1_LIMB_BYTES) < 0)
            return -1;
        r->data[i] = to_be32(buf);
    }
    if (is_unsigned) {
        r->negative = 0;
        asn1_integer_renorm(r);
    } else {
        if (asn1_integer_from_2comp(r))
            return mem_error(s);
    }
#if defined(DEBUG)
    {
        printf("asn1_oer_decode_large_integer1: ");
        asn1_integer_out_str(stdout, r);
        printf("\n");
    }  
#endif
    return 0;
}

static int asn1_oer_decode_large_integer(ASN1DecodeState *s,
                                         const ASN1CType *p, 
                                         uint8_t *data)
{
    const ASN1Integer *range_min, *range_max;
    ASN1Integer *val;
    int flags, nb_bits, nb_bits1, ret, n;
    BOOL is_unsigned;

    flags = *p++;
    range_min = NULL;
    range_max = NULL;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = (const ASN1Integer *)*p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = (const ASN1Integer *)*p++;

    val = (ASN1Integer *)data;
    if (flags & ASN1_CTYPE_HAS_EXT)
        goto unconstrained;
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        if (asn1_integer_cmp_si(range_min, 0) >= 0) {
            nb_bits = asn1_integer_get_bit_size(range_max);
            is_unsigned = TRUE;
        } else {
            if (asn1_integer_get_bit_size_2comp(&nb_bits, range_min))
                return mem_error(s);
            if (asn1_integer_get_bit_size_2comp(&nb_bits1, range_max))
                return mem_error(s);
            nb_bits = max_int(nb_bits, nb_bits1);
            is_unsigned = FALSE;
        }
        if (nb_bits <= 8) {
            n = 1;
        } else if (nb_bits <= 16) {
            n = 2;
        } else if (nb_bits <= 32) {
            n = 4;
        } else if (nb_bits <= 64) {
            n = 8;
        } else {
            n = 0;
        }
        ret = asn1_oer_decode_large_integer1(s, val, n, is_unsigned);
    } else if ((flags & ASN1_CTYPE_HAS_LOW) &&
               asn1_integer_cmp_si(range_min, 0) >= 0) {
        ret = asn1_oer_decode_large_integer1(s, val, 0, TRUE);
    } else {
    unconstrained: ;
        ret = asn1_oer_decode_large_integer1(s, val, 0, FALSE);
    }
    return ret;
}
#endif

static int asn1_oer_decode_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    int val, flags, range_min, range_max;

    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_oer_decode_large_integer(s, p, data);
#else
        return asn1_error(s, "large integers are not supported");
#endif
    }
    p++;
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
    
    if (flags & ASN1_CTYPE_HAS_EXT) 
        goto unconstrained;

    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        if (range_min >= 0) {
            if ((unsigned)range_max <= 255) {
                val = get_byte(s);
                if (val < 0)
                    return -1;
            } else if ((unsigned)range_max <= 65535) {
                if (get_be16(s, &val))
                    return -1;
            } else {
                if (get_be32(s, &val))
                    return -1;
            }
        } else {
            if (range_min >= -128 && range_max <= 127) {
                val = get_byte(s);
                if (val < 0)
                    return -1;
                val = (int8_t)val;
            } else if (range_min >= -32768 && range_max <= 32767) {
                if (get_be16(s, &val))
                    return -1;
                val = (int16_t)val;
            } else {
                if (get_be32(s, &val))
                    return -1;
            }
        }
    } else {
        if ((flags & ASN1_CTYPE_HAS_LOW) && range_min >= 0) {
            if (asn1_decode_uint32(s, &val) < 0)
                return -1;
        } else {
        unconstrained:
            if (asn1_decode_int32(s, &val) < 0)
                return -1;
        }
    }
#ifdef DEBUG
    printf("integer: %d\n", val);
#endif
    *(int *)data = val;
    return 0;
}

static int asn1_oer_decode_bit_string1(ASN1DecodeState *s, ASN1String *str,
                                       int bit_len)
{
    int len, b;
    if (bit_len == -1) {
        len = decode_len(s);
        if (len < 0)
            return -1;
        if (len < 1)
            return asn1_error(s, "bit string: length must be >= 1");
        b = get_byte(s);
        if (b < 0)
            return -1;
        if (len == 1 && b != 0)
            return asn1_error(s, "bit string: first byte must be zero");
        if (b > 7)
            return asn1_error(s, "bit string: first byte must be <= 7");
        bit_len = (len - 1) * 8 - b;
    }
    len = (unsigned)(bit_len + 7) / 8;
    str->buf = asn1_malloc(len);
    if (!str->buf)
        return mem_error(s);
    str->len = bit_len;
    if (get_bytes(s, str->buf, len) < 0)
        return -1;
    /* mask unused bits in last byte */
    if ((bit_len & 7) != 0)
        str->buf[len - 1] &= 0xff00 >> (bit_len & 7);
#ifdef DEBUG
    printf("bit string: len=%d\n", str->len);
#endif
    return 0;
}

static int asn1_oer_decode_bit_string(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    uint32_t range_min, range_max;
    ASN1String *str;
    int flags, bit_len;

    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    str = (ASN1String *)data;
    if (flags & ASN1_CTYPE_HAS_EXT) 
        goto unconstrained_length;
    if (range_min == range_max) {
        bit_len = range_min;
    } else {
    unconstrained_length:
        bit_len = -1;
    }
    return asn1_oer_decode_bit_string1(s, str, bit_len);
}

static int asn1_oer_decode_octet_string(ASN1DecodeState *s, const ASN1CType *p, 
                                        uint8_t *data)
{
    uint32_t range_min, range_max, len;
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
    if (flags & ASN1_CTYPE_HAS_EXT) 
        goto unconstrained_length;
    if (range_min == range_max) {
        len = range_min;
    } else {
 unconstrained_length:
        ret = decode_len(s);
        if (ret < 0)
            return ret;
        len = ret;
    }
    str->buf = asn1_malloc(len);
    if (!str->buf)
        return mem_error(s);
    str->len = len;
    if (get_bytes(s, str->buf, len) < 0)
        return -1;
#ifdef DEBUG
    printf("octet string: len=%d\n", str->len);
#endif
    return 0;
}

static int asn1_oer_decode_open_type(ASN1DecodeState *s, const ASN1CType *p,
                                     uint8_t *data)
{
    int len, saved_buf_len, ret;

    len = decode_len(s);
#ifdef DEBUG
    printf("open type: len=%d\n", len);
#endif
    if (len < 0)
        return -1;
    if (s->buf_index + len > s->buf_len)
        return eos_error(s);
    saved_buf_len = s->buf_len;
    s->buf_len = s->buf_index + len;
    
    ret = asn1_oer_decode_type(s, p, data);

    s->buf_len = saved_buf_len;
    return ret;
}

static int asn1_oer_skip_open_type(ASN1DecodeState *s)
{
    int len;
    len = decode_len(s);
    if (len < 0)
        return -1;
    if (s->buf_index + len > s->buf_len)
        return eos_error(s);
    s->buf_index += len;
    return 0;
}

static inline int preamble_get_bit(ASN1DecodeState *s, 
                                   int *plen, uint8_t *pbyte)
{
    int len, bit, b; 

    len = *plen;
    if (len == 0) {
        b = get_byte(s);
        if (b < 0)
            return -1;
        *pbyte = b;
        len = 8;
    }
    len--;
    bit = (*pbyte >> len) & 1;
    *plen = len;
    return bit;
}

static int asn1_oer_decode_ext_group(ASN1DecodeState *s, uint8_t *data,
                                     const ASN1SequenceField *f1,
                                     int nb_group_fields)
{
    int ret, i, flag, preamble_len, b, len, saved_buf_len;
    const ASN1SequenceField *f;
    uint8_t *table_present, preamble_byte;

    len = decode_len(s);
#ifdef DEBUG
    printf("ext group len=%d %d/%d\n", len, s->buf_index, s->buf_len);
#endif
    if (len < 0)
        return -1;
    if (s->buf_index + len > s->buf_len)
        return eos_error(s);
    saved_buf_len = s->buf_len;
    s->buf_len = s->buf_index + len;
    
    table_present = asn1_malloc(nb_group_fields);
    if (!table_present) {
        ret = mem_error(s);
        goto fail;
    }

    preamble_byte = 0;
    preamble_len = 0;

    for(i = 0; i < nb_group_fields; i++) {
        f = f1 + i;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag != ASN1_SEQ_FLAG_NORMAL) {
            b = preamble_get_bit(s, &preamble_len, &preamble_byte);
            if (b < 0) {
                ret = -1;
                goto fail;
            }
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
#ifdef DEBUG
            printf("ext group field=%s\n", f->name);
#endif
            ret = asn1_oer_decode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto fail;
        }
    }
    ret = 0;

 fail:
    asn1_free(table_present);
    s->buf_len = saved_buf_len;
    return ret;
}

static int asn1_oer_decode_sequence(ASN1DecodeState *s, const ASN1CType *p, 
                                    uint8_t *data)
{
    int has_ext, nb_fields, extension_present, i;
    int ret;
    const ASN1SequenceField *f, *f1, *f2;
    int flag, preamble_len, b;
    uint8_t *table_present, preamble_byte;
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

    preamble_len = 0;
    preamble_byte = 0; /* avoids warning */
    if (has_ext) {
        b = preamble_get_bit(s, &preamble_len, &preamble_byte);
        if (b < 0)
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
                b = preamble_get_bit(s, &preamble_len, &preamble_byte);
                if (b < 0)
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
            ret = asn1_oer_decode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                goto fail;
        }
    }

    asn1_free(table_present);
    table_present = NULL;

    if (extension_present) {
        ASN1String str;
        uint32_t nb_exts;
        int nb_group_fields, ext_idx;

        /* extension present bit mask */
        if (asn1_oer_decode_bit_string1(s, &str, -1) < 0)
            goto fail;
        nb_exts = str.len;
        table_present = str.buf;

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
                if (get_bit(table_present, ext_idx)) {
                    if (asn1_oer_skip_open_type(s))
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
                    if (get_bit(table_present, ext_idx)) {
                        if (asn1_oer_decode_ext_group(s, data, 
                                                      f, nb_group_fields))
                            goto fail;
                    }
                    i += nb_group_fields;
                } else {
                    /* simple extension */
                    if (get_bit(table_present, ext_idx)) {
                        /* mark as present */
                        flag = ASN1_GET_SEQ_FLAG(f);
                        if (flag != ASN1_SEQ_FLAG_DEFAULT)
                            *(BOOL *)(data + f->u.option_offset) = TRUE;
#ifdef DEBUG
                        printf("seq ext field=%s\n", f->name);
#endif
                        if (asn1_oer_decode_open_type(s, f->type, 
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

static int asn1_oer_decode_sequence_of(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, len, i;
    ASN1String *str;

    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) 
        p++;
    f = (const ASN1SequenceOfCType *)p;
    
    if (asn1_decode_uint32(s, &len) < 0)
        return -1;

    str = (ASN1String *)data;
    str->buf = asn1_mallocz2(len, f->elem_size);
    if (!str->buf)
        return mem_error(s);
    str->len = len;
    
    for(i = 0; i < len; i++) {
        ret = asn1_oer_decode_type(s, f->type, 
                                   str->buf + i * f->elem_size);
        if (ret)
            return ret;
    }
#ifdef DEBUG
    printf("seq of: len=%d\n", str->len);
#endif
    return 0;
}

static BOOL find_tag1(const ASN1CType *p, int tag1)
{
    uint32_t flags, nb_fields, i;
    int type, tag;
    const ASN1ChoiceField *f;

 redo:
    flags = *p;
    type = ASN1_GET_CTYPE(flags);
    tag = ASN1_GET_TAG(flags);
    if (type == ASN1_CTYPE_CHOICE && tag == ASN1_NO_TAG) {
        /* must look at the choice to know the exact tag */
        p++;
        nb_fields = *p++;
        if (flags & ASN1_CTYPE_HAS_EXT) 
            nb_fields += *p++;
        p++;
        p += 2;
        f = (const ASN1ChoiceField *)p;
        for(i = 0; i < nb_fields; i++) {
            if (find_tag1(f->type, tag1))
                return TRUE;
            f++;
        }
        return FALSE;
    } else if (type == ASN1_CTYPE_TAGGED && tag == ASN1_NO_TAG) {
        p = (ASN1CType *)p[1];
        goto redo;
    } else if (type == ASN1_CTYPE_ANY) {
        /* any tag is accepted */
        return TRUE;
    } else {
#ifdef DEBUG
        printf("  tag=0x%x\n", tag);
#endif
        return (tag1 == tag);
    }
}

static int asn1_oer_decode_choice(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    int nb_fields, i, flags, ret, nb_fields_ext, tag;
    uint32_t data_offset, choice_offset;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields_ext = *p++;
    else
        nb_fields_ext = 0;
    nb_fields += nb_fields_ext;
    p++;
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;

    tag = decode_tag(s);
    if (tag < 0) {
        return asn1_error(s, "invalid choice tag");
    }
    
    f = (const ASN1ChoiceField *)p;
#ifdef DEBUG
    printf("searching choice tag: 0x%x\n", tag);
#endif
    for(i = 0; i < nb_fields; i++) {
        if (find_tag1(f->type, tag)) 
            break;
        f++;
    }
    if (i == nb_fields)
        return asn1_error(s, "tag 0x%x not present in choice", tag);
    *(uint32_t *)(data + choice_offset) = i;
    if (i < nb_fields - nb_fields_ext) {
        ret = asn1_oer_decode_type(s, f->type, data + data_offset);
    } else {
        ret = asn1_oer_decode_open_type(s, f->type, data + data_offset);
    }
    return ret;
}

static int asn1_oer_decode_enumerated(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    int nb_fields, val, flags, b;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    
    b = get_byte(s);
    if (b < 0)
        return -1;
    if (b >= 0 && b <= 127) {
        val = b;
    } else {
        if (asn1_decode_int32_internal(s, &val, b & 0x7f) < 0)
            return -1;
    }
    val = asn1_find_enum_index(p, nb_fields, val, flags);
    if (val < 0)
        goto out_of_range;
    *(uint32_t *)data = val;
    return 0;
 out_of_range:
    return asn1_error(s, "enumerated value out of range");
}

static int asn1_oer_decode_tagged(ASN1DecodeState *s, const ASN1CType *p, 
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
    return asn1_oer_decode_type(s, (ASN1CType *)p[1], data1);
}

static int asn1_oer_decode_object_identifier(ASN1DecodeState *s, 
                                             const ASN1CType *p, 
                                             uint8_t *data)
{
    ASN1String *str;
    int len;
    len = decode_len(s);
    if (len < 0)
        return -1;
    str = (ASN1String *)data;
    str->buf = asn1_malloc(len);
    if (!str->buf)
        return mem_error(s);
    str->len = len;
    if (get_bytes(s, str->buf, len) < 0)
        return -1;
    return 0;
}

static int asn1_oer_decode_real(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    int len;
    double d;
    len = decode_len(s);
    if (len < 0)
        return -1;
    if (s->buf_index + len > s->buf_len)
        return -1;
    /* XXX: need to add support for float32/double32 */
    if (asn1_decode_real_ber(s->buf + s->buf_index, len, &d))
        return asn1_error(s, "real: error in encoding");
    s->buf_index += len;
    *(double *)data = d;
    return 0;
}

static int asn1_oer_decode_char_string(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    int char_string_type, shift, i, flags, len;
    uint32_t range_min, range_max;
    ASN1String *str;

    flags = *p++;
    char_string_type = *p++;
    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;
    str = (ASN1String *)data;
    if (char_string_type > ASN1_CSTR_UniversalString) {
        len = decode_len(s);
        if (len < 0)
            return -1;
    } else {
        range_min = *p++;
        range_max = UINT32_MAX;
        if (flags & ASN1_CTYPE_HAS_HIGH)
            range_max = *p++;
        if (range_min == range_max) {
            len = range_min;
        } else {
            len = decode_len(s);
            if (len < 0)
                return -1;
            if ((len & ((1 << shift) - 1)) != 0) {
                return asn1_error(s, 
                                  "character string length must be multiple of %d (%d)",
                                  1 << shift, len);
            }
            len >>= shift;
        }
    }
    str->buf = asn1_malloc2(len, 1 << shift);
    if (!str->buf)
        return mem_error(s);
    str->len = len;
    switch(shift) {
    default:
    case 0:
        if (get_bytes(s, str->buf, str->len) < 0)
            return -1;
        break;
    case 1:
        for(i = 0; i < str->len; i++) {
            int val;
            if (get_be16(s, &val))
                return -1;
            ((uint16_t *)str->buf)[i] = val;
        }
        break;
    case 2:
        for(i = 0; i < str->len; i++) {
            int val;
            if (get_be32(s, &val))
                return -1;
            ((uint32_t *)str->buf)[i] = val;
        }
        break;
    }
    return 0;
}

static int asn1_oer_decode_any(ASN1DecodeState *s, const ASN1CType *p, 
                               uint8_t *data)
{
    ASN1OpenType *str;
    int len;

    str = (ASN1OpenType *)data;
    str->type = (ASN1CType *)asn1_get_constrained_type(s->top_value, p);
    if (!str->type) {
        len = decode_len(s);
        if (len < 0)
            return -1;
        str->u.octet_string.len = len;
        str->u.octet_string.buf = asn1_malloc(len);
        if (!str->u.octet_string.buf)
            return mem_error(s);
        if (get_bytes(s, str->u.octet_string.buf, len) < 0)
            return -1;
        return 0;
    } else {
        str->u.data = asn1_mallocz_value(str->type);
        if (!str->u.data)
            return mem_error(s);
        return asn1_oer_decode_open_type(s, str->type, str->u.data);
    }
}

static int asn1_oer_decode_type(ASN1DecodeState *s, const ASN1CType *p,
                                uint8_t *data)
{
    int type, ret, flags;
    
    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
#ifdef DEBUG
    printf("type=%s\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?");
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_oer_decode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_oer_decode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_oer_decode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_oer_decode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_oer_decode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_oer_decode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_oer_decode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_oer_decode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_oer_decode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_oer_decode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_oer_decode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_oer_decode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_oer_decode_any(s, p, data);
        break;
    default:
        ret = asn1_error(s, "unsupported type (%d)", type);
        break;
    }
    return ret;
}

/* OER decoding. Return the number of consumed byted or < 0
   if error. */
asn1_ssize_t asn1_oer_decode(void **pdata, const ASN1CType *p,
                         const uint8_t *buf, size_t buf_len, ASN1Error *err)
{
    ASN1DecodeState s_s, *s = &s_s;
    int ret;
    void *data;

    s->buf = buf;
    s->buf_len = buf_len;
    s->buf_index = 0;
    s->top_value = NULL;

    s->error.bit_pos = 0;
    s->error.msg[0] = '\0';
    
    data = asn1_mallocz_value(p);
    if (!data) {
        mem_error(s);
        goto fail;
    }
    
    ret = asn1_oer_decode_type(s, p, data);
    if (ret < 0) {
    fail:
        if (data)
            asn1_free_value(p, data);
        if (err)
            *err = s->error;
        *pdata = NULL;
        return -1;
    } else {
        *pdata = data;
        return s->buf_index;
    }
}
