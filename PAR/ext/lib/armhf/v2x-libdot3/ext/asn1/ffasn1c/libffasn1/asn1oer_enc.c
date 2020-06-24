/* 
 * ASN1 OER encoder (ITU-T X.696)
 * Copyright (C) 2015-2018 Fabrice Bellard
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "asn1defs_int.h"
#include "asn1ber_common.h"

//#define DEBUG

typedef struct ASN1EncodeState {
    ASN1ByteBuffer bb;
    ASN1Error error;
} ASN1EncodeState;

static int asn1_oer_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data);
static int asn1_oer_encode_open_type(ASN1EncodeState *s, const ASN1CType *p, 
                                     const uint8_t *data);

static int asn1_encode_error(ASN1EncodeState *s, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(s->error.msg, sizeof(s->error.msg), fmt, ap);
    va_end(ap);
    return -1;
}

static int mem_error(ASN1EncodeState *s)
{
    return asn1_encode_error(s, "not enough memory");
}

static int asn1_encode_uint32(ASN1ByteBuffer *s, unsigned int val)
{
    int n, i;

    for(n = 1; n <= 3; n++) {
        if ((val >> (8 * n)) == 0)
            break;
    }
    asn1_ber_put_len(s, n);
    for(i = n - 1; i >= 0; i--) {
        asn1_put_byte(s, ((unsigned int)val >> (8 * i)) & 0xff);
    }
    return 0;
}

static int asn1_oer_encode_boolean(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int val;
    val = *(BOOL *)data;
    if (val)
        val = 0xff;
    asn1_put_byte(&s->bb, val);
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
/* encode the larger integer 'r'. If nb_bytes = 0, use indefinite length. */
static asn1_exception int asn1_oer_encode_large_integer1(ASN1EncodeState *s, 
                                                         const ASN1Integer *r, 
                                                         int nb_bytes,
                                                         BOOL is_unsigned)
{
    ASN1Integer tmp, *q;
    int nb_bits, len, l, i;
    ASN1Limb v;

    if (r->len == 0) {
        if (nb_bytes == 0) {
            asn1_ber_put_len(&s->bb, 1);
            asn1_put_byte(&s->bb, 0);
        } else {
            for(i = 0; i < nb_bytes; i++) {
                asn1_put_byte(&s->bb, 0);
            }
        }
    } else {
        q = asn1_integer_to_2comp(&tmp, r, &nb_bits);
        if (!q)
            return mem_error(s);
        len = (nb_bits + 8 - is_unsigned) >> 3;
        if (nb_bytes == 0) {
            asn1_ber_put_len(&s->bb, len);
            if (!is_unsigned && (nb_bits & (ASN1_LIMB_BITS - 1)) == 0) {
                asn1_put_byte(&s->bb, (-r->negative) & 0xff); /* extra byte */
                len--;
            }
        } else {
            if (!is_unsigned && (nb_bits & (ASN1_LIMB_BITS - 1)) == 0)
                len--;
            v = (-r->negative) & 0xff;
            for(i = len; i < nb_bytes; i++) {
                asn1_put_byte(&s->bb, v);
            }
        }
        if (nb_bits != 0) {
            /* first limb */
            v = q->data[q->len - 1];
            l = len & 3;
            if (l == 0)
                l = 4;
            for(i = l - 1; i >= 0; i--) {
                asn1_put_byte(&s->bb, (v >> (8 * i)) & 0xff);
            }
            /* next limbs */
            for(i = q->len - 2; i >= 0; i--)
                asn1_put_be32(&s->bb, q->data[i]);
        }
        if (q == &tmp)
            asn1_integer_clear(q);
    }
    return 0;
}

static int asn1_oer_encode_large_integer(ASN1EncodeState *s,
                                         const ASN1CType *p, 
                                         const uint8_t *data)
{
    const ASN1Integer *range_min, *range_max, *val;
    int flags, nb_bits, nb_bits1, n;
    BOOL is_unsigned;

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
        if (asn1_oer_encode_large_integer1(s, val, n, is_unsigned))
            return -1;
    } else if ((flags & ASN1_CTYPE_HAS_LOW) &&
               asn1_integer_cmp_si(range_min, 0) >= 0) {
        if (asn1_oer_encode_large_integer1(s, val, 0, TRUE))
            return -1;
    } else {
    unconstrained: ;
        if (asn1_oer_encode_large_integer1(s, val, 0, FALSE))
            return -1;
    }
    return 0;
}
#endif

/* contrainted signed or unsigned 32 bit number */
static int asn1_oer_encode_integer(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int range_min, range_max, flags, val;
    
    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_oer_encode_large_integer(s, p, data);
#else
        return asn1_error(s, "large integers are not supported");
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
    
    if (flags & ASN1_CTYPE_HAS_EXT) 
        goto unconstrained;

    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        if (range_min >= 0) {
            if ((unsigned)range_max <= 255) {
                asn1_put_byte(&s->bb, val);
            } else if ((unsigned)range_max <= 65535) {
                asn1_put_be16(&s->bb, val);
            } else {
                asn1_put_be32(&s->bb, val);
            }
        } else {
            if (range_min >= -128 && range_max <= 127) {
                asn1_put_byte(&s->bb, val);
            } else if (range_min >= -32768 && range_max <= 32767) {
                asn1_put_be16(&s->bb, val);
            } else {
                asn1_put_be32(&s->bb, val);
            }
        }
    } else {
        if ((flags & ASN1_CTYPE_HAS_LOW) && range_min >= 0) {
            asn1_encode_uint32(&s->bb, val);
        } else {
        unconstrained:
            asn1_ber_encode_int32(&s->bb, val);
        }
    }
    return 0;
}

static inline void asn1_oer_encode_bit_string1(ASN1EncodeState *s,
                                               const uint8_t *buf, int bit_len)
{
    int byte_len, n, k;

    byte_len = (unsigned)(bit_len + 7) / 8;
    asn1_ber_put_len(&s->bb, byte_len + 1);
    asn1_put_byte(&s->bb, (-bit_len) & 7);
    n = bit_len >> 3;
    asn1_put_bytes(&s->bb, buf, n);
    /* for the last octet, we force trailing bits to zero */
    k = bit_len & 7;
    if (k != 0) {
        asn1_put_byte(&s->bb, buf[n] & (0xff00 >> k));
    }
}

static int asn1_oer_encode_bit_string(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    ASN1BitString *str;
    uint32_t range_min, range_max;
    int flags;

    str = (ASN1BitString *)data;

    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    if (range_min == range_max && !(flags & ASN1_CTYPE_HAS_EXT)) {
        int n, k;
        /* fixed length */
        n = str->len >> 3;
        asn1_put_bytes(&s->bb, str->buf, n);
        k = str->len & 7;
        if (k != 0) {
            asn1_put_byte(&s->bb, str->buf[n] & (0xff00 >> k));
        }
    } else {
        asn1_oer_encode_bit_string1(s, str->buf, str->len);
    }
    return 0;
}

static int asn1_oer_encode_octet_string(ASN1EncodeState *s, const ASN1CType *p, 
                                        const uint8_t *data)
{
    ASN1String *str;
    uint32_t range_min, range_max;
    int flags;

    flags = p[0];
    range_min = p[1];
    if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = p[2];
    } else {
        range_max = UINT32_MAX;
    }
    str = (ASN1String *)data;
    if (range_min == range_max && !(flags & ASN1_CTYPE_HAS_EXT)) {
    } else {
        asn1_ber_put_len(&s->bb, str->len);
    }
    asn1_put_bytes(&s->bb, str->buf, str->len);
    return 0;
}

static int asn1_oer_encode_ext_group(ASN1EncodeState *s, 
                                     const uint8_t *data,
                                     const ASN1SequenceField *f1,
                                     const uint8_t *table_present, 
                                     int nb_group_fields)
{
    int j, ret, flag, preamble_len, start_pos;
    uint8_t preamble_byte;
    const ASN1SequenceField *f;
    
    start_pos = asn1_ber_put_len_start(&s->bb);

    /* bit mask for DEFAULT and OPTIONAL fields */
    preamble_byte = 0;
    preamble_len = 0;
    for(j = 0; j < nb_group_fields; j++) {
        f = f1 + j;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag != ASN1_SEQ_FLAG_NORMAL) {
            preamble_byte |= (table_present[j] & 1) << (7 - preamble_len);
            if (++preamble_len == 8) {
                asn1_put_byte(&s->bb, preamble_byte);
                preamble_byte = 0;
                preamble_len = 0;
            }
        }
    }

    if (preamble_len != 0) {
        asn1_put_byte(&s->bb, preamble_byte);
    }
    
    for(j = 0; j < nb_group_fields; j++) {
        if (table_present[j] & 1) {
            f = f1 + j;
            ret = asn1_oer_encode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
        }
    }
    
    asn1_ber_put_len_end(&s->bb, start_pos);
    return 0;
}

static int asn1_oer_encode_sequence(ASN1EncodeState *s, const ASN1CType *p, 
                                    const uint8_t *data)
{
    int nb_fields, i, flag, ret, has_ext, extension_present, j;
    int option_bit, nb_ext_fields, nb_group_fields, val;
    const ASN1SequenceField *f, *f1;
    uint32_t preamble_len;
    uint8_t *table_present, *preamble;
    
    has_ext = (p[0] & ASN1_CTYPE_HAS_EXT) != 0;
    p++;
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;
#ifdef DEBUG 
    printf("seq nb_fields=%d\n", nb_fields);
#endif

    /* compute if each field is present or not (bit 0) */
    table_present = asn1_malloc(nb_fields + ((nb_fields + 1 + 7) >> 3));
    if (!table_present)
        return -1;
    preamble = table_present + nb_fields;
    preamble_len = 0;
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

    /* write the preamble */
    memset(preamble, 0, (nb_fields + 1 + 7) >> 3);
    if (has_ext) {
        put_bit(preamble, preamble_len++, extension_present);
    }

    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f)) {
            flag = ASN1_GET_SEQ_FLAG(f);
            if (flag != ASN1_SEQ_FLAG_NORMAL) {
                put_bit(preamble, preamble_len++, table_present[i]);
            }
        }
    }

    if (preamble_len != 0)
        asn1_put_bytes(&s->bb, preamble, (preamble_len + 7) >> 3);
    
    /* write the root component */
    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f) && table_present[i]) {
            ret = asn1_oer_encode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret) 
                return ret;
        }
    }
    
    if (extension_present) {

        memset(preamble, 0, (nb_fields + 7) >> 3);
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
                put_bit(preamble, nb_ext_fields++, option_bit);
            }
        }

        asn1_oer_encode_bit_string1(s, preamble, nb_ext_fields);

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
                    ret = asn1_oer_encode_ext_group(s, data,
                                                    f1 + i,
                                                    table_present + i,
                                                    nb_group_fields);
                    if (ret)
                        goto fail;
                } else {
                    ret = asn1_oer_encode_open_type(s, f->type,
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

static int asn1_oer_encode_sequence_of(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, i;
    ASN1String *str;

    str = (ASN1String *)data;
#ifdef DEBUG
    printf("seq of: len=%d\n", str->len);
#endif
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    
    asn1_encode_uint32(&s->bb, str->len);

    ret = 0;
    for(i = 0; i < str->len; i++) {
        ret = asn1_oer_encode_type(s, f->type, str->buf + i * f->elem_size);
        if (ret)
            break;
    }
    return ret;
}

static int find_tag(const ASN1CType *p, uint32_t *ptag)
{
    uint32_t flags;
    int type, tag;

 redo:
    flags = *p;
    type = ASN1_GET_CTYPE(flags);
    tag = ASN1_GET_TAG(flags);
    if (tag == ASN1_NO_TAG) {
        if (type == ASN1_CTYPE_CHOICE) {
            int nb_fields;
            const ASN1ChoiceField *f;

            /* must look at the choice to know the exact tag */
            p++;
            nb_fields = *p++;
            if (flags & ASN1_CTYPE_HAS_EXT) 
                nb_fields += *p++;
            p++;
            p += 2;
            f = (const ASN1ChoiceField *)p;
            /* use the first element of the choice to find the tag
               (XXX: the spec is not clear at all) */
            p = f->type;
            goto redo;
        } else if (type == ASN1_CTYPE_TAGGED) {
            p = (ASN1CType *)p[1];
            goto redo;
        } else {
            /* should not happen */
            return -1;
        }
    }
    *ptag = tag;
    return 0;
}


static int asn1_oer_encode_choice(ASN1EncodeState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int nb_fields, ret, nb_fields_ext;
    uint32_t choice_val, flags, tag;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) 
        nb_fields_ext = *p++;
    else
        nb_fields_ext = 0;
    p++;
    choice_val = *(uint32_t *)(data + p[0]);
    data += p[1];
    p += 2;
    /* check choice consistency */
    if (choice_val >= (nb_fields + nb_fields_ext)) {
        return asn1_encode_error(s, "invalid choice value (%u >= %u)",
                                 choice_val, nb_fields + nb_fields_ext);
    }

    f = (const ASN1ChoiceField *)p + choice_val;

    if (find_tag(f->type, &tag) < 0) {
        return asn1_encode_error(s, "could not find choice tag");
    }
    asn1_ber_put_tag(&s->bb, tag, 0);
    if (choice_val < nb_fields) {
        ret = asn1_oer_encode_type(s, f->type, data);
    } else {
        ret = asn1_oer_encode_open_type(s, f->type, data);
    }
    return ret;
}

static int asn1_oer_encode_enumerated(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    uint32_t nb_fields, flags;
    int val;

    val = *(uint32_t *)data;

    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_CONV_TABLE) {
        /* slower case: need conversion table */
        nb_fields = *p++;
        if (flags & ASN1_CTYPE_HAS_EXT)
            nb_fields += *p++;
        p += nb_fields;
        if (flags & ASN1_CTYPE_HAS_NAME)
            p++;
        if (flags & ASN1_CTYPE_HAS_CONSTRAINT)
            p++;
        /* check consistency */
        if (val >= nb_fields) {
            return asn1_encode_error(s, "invalid enumerated value (%u >= %u)",
                                     val, nb_fields);
        }
        /* convert value */
        val = p[val];
    }
    if (val >= 0 && val <= 127) {
        asn1_put_byte(&s->bb, val);
    } else {
        int shift, n, i;
        n = 1;
        shift = 24;
        while (((val << shift) >> shift) != val) {
            n++;
            shift -= 8;
        }
        asn1_put_byte(&s->bb, n | 0x80);
        
        for(i = n - 1; i >= 0; i--) {
            asn1_put_byte(&s->bb, ((unsigned int)val >> (8 * i)) & 0xff);
        }
    }
    return 0;
}

static int asn1_oer_encode_real(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    uint8_t buf[ASN1_DOUBLE_DER_MAX_LEN];
    int len;
    /* XXX: need to add support for float32/double32 */
    len = asn1_encode_real_der(buf, *(double *)data);
    asn1_ber_put_len(&s->bb, len);
    asn1_put_bytes(&s->bb, buf, len);
    return 0;
}

static int asn1_oer_encode_char_string(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    int char_string_type, shift, i, flags;
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
        asn1_ber_put_len(&s->bb, str->len << shift);
    } else {
        range_min = *p++;
        range_max = UINT32_MAX;
        if (flags & ASN1_CTYPE_HAS_HIGH)
            range_max = *p++;
        if (range_min != range_max)
            asn1_ber_put_len(&s->bb, str->len << shift);
    }
    switch(shift) {
    default:
    case 0:
        asn1_put_bytes(&s->bb, str->buf, str->len);
        break;
    case 1:
        for(i = 0; i < str->len; i++) {
            asn1_put_be16(&s->bb, ((uint16_t *)str->buf)[i]);
        }
        break;
    case 2:
        for(i = 0; i < str->len; i++) {
            asn1_put_be32(&s->bb, ((uint32_t *)str->buf)[i]);
        }
        break;
    }
    return 0;
}

static int asn1_oer_encode_open_type(ASN1EncodeState *s, const ASN1CType *p, 
                                     const uint8_t *data)
{
    int pos, ret;

    pos = asn1_ber_put_len_start(&s->bb);

    ret = asn1_oer_encode_type(s, p, data);
    if (ret)
        return ret;
    asn1_ber_put_len_end(&s->bb, pos);
    return 0;
}

static int asn1_oer_encode_any(ASN1EncodeState *s, const ASN1CType *p, 
                               const uint8_t *data)
{
    const ASN1OpenType *str;

    str = (const ASN1OpenType *)data;
    if (!str->type) {
        asn1_ber_put_len(&s->bb, str->u.octet_string.len);
        asn1_put_bytes(&s->bb, str->u.octet_string.buf, 
                       str->u.octet_string.len);
        return 0;
    } else {
        return asn1_oer_encode_open_type(s, str->type, str->u.data);
    }
}

static int asn1_oer_encode_tagged(ASN1EncodeState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int flags;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data = *(void **)data;
    }
    return asn1_oer_encode_type(s, (ASN1CType *)p[1], data);
}

static int asn1_oer_encode_object_identifier(ASN1EncodeState *s, 
                                             const ASN1CType *p,
                                             const uint8_t *data)
{
    ASN1String *str;
    str = (ASN1String *)data;
    
    asn1_ber_put_len(&s->bb, str->len);
    asn1_put_bytes(&s->bb, str->buf, str->len);
    return 0;
}

static int asn1_oer_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    int type, ret;
    unsigned int flags;

    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
#ifdef DEBUG
    printf("type=%s\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?");
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_oer_encode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_oer_encode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_oer_encode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_oer_encode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_oer_encode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_oer_encode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_oer_encode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_oer_encode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_oer_encode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_oer_encode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_oer_encode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_oer_encode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_oer_encode_any(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

asn1_ssize_t asn1_oer_encode2(uint8_t **pbuf, const ASN1CType *p, 
                              const void *data, ASN1Error *err)
{
    ASN1EncodeState s_s, *s = &s_s;
    int ret;

    asn1_byte_buffer_init(&s->bb);

    s->error.bit_pos = 0;
    s->error.msg[0] = '\0';

    ret = asn1_oer_encode_type(s, p, data);
    if (ret) {
        goto fail;
    } else if (s->bb.has_error) {
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

asn1_ssize_t asn1_oer_encode(uint8_t **pbuf, const ASN1CType *p, 
                             const void *data)
{
    return asn1_oer_encode2(pbuf, p, data, NULL);
}
