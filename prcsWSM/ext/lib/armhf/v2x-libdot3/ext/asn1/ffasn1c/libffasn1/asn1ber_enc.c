/* 
 * ASN1 BER encoder
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
#include "asn1ber_common.h"

//#define DEBUG

typedef struct ASN1EncodeState {
    ASN1ByteBuffer bb;
    ASN1BERParams params;
} ASN1EncodeState;

static uint32_t put_len_start(ASN1EncodeState *s)
{
    uint32_t pos;

    if (s->params.indefinite_len_for_constructed_encoding) {
        asn1_put_byte(&s->bb, 0x80);
        pos = -1;
    } else {
        pos = asn1_ber_put_len_start(&s->bb);
    }
    return pos;
}

static void put_len_end(ASN1EncodeState *s1, uint32_t pos)
{
    ASN1ByteBuffer *s = &s1->bb;

    if (s1->params.indefinite_len_for_constructed_encoding) {
        /* End of content */
        asn1_put_byte(s, 0x00); 
        asn1_put_byte(s, 0x00);
    } else {
        asn1_ber_put_len_end(s, pos);
    }
}


static void put_tl(ASN1EncodeState *s, int tag, int constructed, int len)
{
    asn1_ber_put_tag(&s->bb, tag, constructed);
    asn1_ber_put_len(&s->bb, len);
}

static int asn1_ber_encode_boolean(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data, unsigned int tag)
{
    int val;
    put_tl(s, tag, 0, 1);
    val = *(BOOL *)data;
    if (val)
        val = 0xff;
    asn1_put_byte(&s->bb, val);
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
static int asn1_ber_encode_large_integer(ASN1EncodeState *s, const ASN1CType *p, 
                                         const uint8_t *data, unsigned int tag)
{
    const ASN1Integer *r;
    ASN1Integer tmp, *q;
    int nb_bits, len, l, i;
    ASN1Limb v;

    asn1_ber_put_tag(&s->bb, tag, 0);
    r = (const ASN1Integer *)data;
    if (r->len == 0) {
        asn1_ber_put_len(&s->bb, 1);
        asn1_put_byte(&s->bb, 0);
    } else {
        q = asn1_integer_to_2comp(&tmp, r, &nb_bits);
        if (!q)
            return -1;
        len = (nb_bits + 8) >> 3;
        asn1_ber_put_len(&s->bb, len);
        if ((nb_bits & (ASN1_LIMB_BITS - 1)) == 0) {
            asn1_put_byte(&s->bb, (-r->negative) & 0xff); /* extra byte */
            len--;
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
            for(i = q->len - 2; i >= 0; i--) {
                v = q->data[i];
                asn1_put_byte(&s->bb, (v >> 24) & 0xff);
                asn1_put_byte(&s->bb, (v >> 16) & 0xff);
                asn1_put_byte(&s->bb, (v >> 8) & 0xff);
                asn1_put_byte(&s->bb, (v) & 0xff);
            }
        }
        if (q == &tmp)
            asn1_integer_clear(q);
    }
    return 0;
}
#endif /* ASN1_USE_LARGE_INTEGER */

static int asn1_ber_encode_integer(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data, unsigned int tag)
{
    int val, i;
    uint32_t flags;

    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_ber_encode_large_integer(s, p, data, tag);
#else
        return -1;
#endif
    }
    asn1_ber_put_tag(&s->bb, tag, 0);
    val = *(int *)data;
    if (asn1_is_uint32(p) && (unsigned int)val >= (1U << 31)) {
        asn1_ber_put_len(&s->bb, 5);
        asn1_put_byte(&s->bb, 0);
        for(i = 3; i >= 0; i--) {
            asn1_put_byte(&s->bb, ((unsigned int)val >> (8 * i)) & 0xff);
        }
    } else {
        asn1_ber_encode_int32(&s->bb, val);
    }
    return 0;
}

static inline void asn1_ber_encode_bit_string1(ASN1EncodeState *s, int tag, 
                                               const uint8_t *buf, int bit_len)
{
    int byte_len, n, k;

    byte_len = (bit_len + 7) / 8;
    put_tl(s, tag, 0, byte_len + 1);
    asn1_put_byte(&s->bb, (-bit_len) & 7);
    n = bit_len >> 3;
    asn1_put_bytes(&s->bb, buf, n);
    /* for the last octet, we force trailing bits to zero */
    k = bit_len & 7;
    if (k != 0) {
        asn1_put_byte(&s->bb, buf[n] & (0xff00 >> k));
    }
}

static int asn1_ber_encode_bit_string(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data, unsigned int tag)
{
    ASN1BitString *str;
    int byte_len;

    str = (ASN1BitString *)data;
#ifdef DEBUG
    printf("bit string: len=%d\n", (int)str->len);
#endif
    byte_len = (str->len + 7) / 8;
    if (s->params.string_max_len == 0 ||
        (byte_len + 1) <= s->params.string_max_len) {
        asn1_ber_encode_bit_string1(s, tag, str->buf, str->len);
    } else {
        /* for testing: generate constructed encoding */
        uint32_t start_pos, bit_len, l;
        const uint8_t *buf;

        asn1_ber_put_tag(&s->bb, tag, 1);
        start_pos = put_len_start(s);
        bit_len = str->len;
        buf = str->buf;
        while (bit_len != 0) {
            l = min_int(bit_len, (s->params.string_max_len - 1) * 8);
            asn1_ber_encode_bit_string1(s, ASN1_TAG_UNIVERSAL + 3, 
                                        buf, l);
            bit_len -= l;
            buf += l >> 3;
        }
        put_len_end(s, start_pos);
    }
    return 0;
}

static int asn1_ber_encode_octet_string1(ASN1EncodeState *s, unsigned int tag,
                                         const uint8_t *buf, int len)
{
#ifdef DEBUG
    printf("octet string: len=%d\n", len);
#endif
    if (s->params.string_max_len == 0 ||
        len <= s->params.string_max_len) {
        put_tl(s, tag, 0, len);
        asn1_put_bytes(&s->bb, buf, len);
    } else {
        uint32_t start_pos, l;

        asn1_ber_put_tag(&s->bb, tag, 1);
        start_pos = put_len_start(s);
        while (len != 0) {
            asn1_ber_put_tag(&s->bb, ASN1_TAG_UNIVERSAL + 4, 0);
            l = min_int(len, s->params.string_max_len);
            asn1_ber_put_len(&s->bb, l);
            asn1_put_bytes(&s->bb, buf, l);
            buf += l;
            len -= l;
        }
        put_len_end(s, start_pos);
    }
    return 0;
}

static int asn1_ber_encode_octet_string(ASN1EncodeState *s, const ASN1CType *p, 
                                        const uint8_t *data, unsigned int tag)
{
    ASN1String *str;
    str = (ASN1String *)data;
    return asn1_ber_encode_octet_string1(s, tag, str->buf, str->len); 
}

static int asn1_ber_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data);

static int asn1_ber_encode_sequence(ASN1EncodeState *s, const ASN1CType *p, 
                                    const uint8_t *data, unsigned int tag)
{
    int nb_fields, i, flag, ret, present;
    const ASN1SequenceField *f, *f1;
    uint32_t val, start_pos;
    
    asn1_ber_put_tag(&s->bb, tag, 1);
    start_pos = put_len_start(s);

    p++;
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;
#ifdef DEBUG 
    printf("seq nb_fields=%d\n", nb_fields);
#endif
    /* compute if each field is present or not */
    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            present = *(BOOL *)(data + f->u.option_offset);
        } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
            val = *(uint32_t *)(data + ASN1_GET_SEQ_OFFSET(f));
            present = (val != f->u.default_value);
        } else {
            present = 1;
        }
        if (present) {
            ret = asn1_ber_encode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret) 
                return ret;
        }
    }
    put_len_end(s, start_pos);
    return 0;
}

typedef struct {
    unsigned int tag;
    const ASN1SequenceField *f;
} SequenceField;

/* return -1 if error */
static int get_tag(const ASN1CType *p, const uint8_t *data)
{
    uint32_t flags, choice_val, nb_fields, tag;
    int type;
    const ASN1ChoiceField *f;

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
        choice_val = *(uint32_t *)(data + p[0]);
        data += p[1];
        p += 2;
        /* check choice consistency */
        if (choice_val >= nb_fields)
            return -1;
        f = (const ASN1ChoiceField *)p + choice_val;
        return get_tag(f->type, data);
    } else {
        return tag;
    }
}

static int cmp_sequence_field(const void *a1, const void *a2)
{
    const SequenceField *f1 = a1;
    const SequenceField *f2 = a2;
    if (f1->tag < f2->tag)
        return -1;
    else if (f1->tag == f2->tag)
        return 0;
    else
        return 1;
}                             

/* encode SET by sorting using the tags */
static int asn1_ber_encode_set_der(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data, unsigned int tag)
{
    int nb_fields, ret, nb_table_fields, i, flag, present;
    uint32_t start_pos, val;
    SequenceField *table_fields;
    const ASN1SequenceField *f;

    p++;
    nb_fields = *p++;
    p++;
    f = (const ASN1SequenceField *)p;

    table_fields = asn1_malloc2(sizeof(SequenceField), nb_fields);
    if (!table_fields)
        return -1;

    /* compute which fields are present and their tag */
    nb_table_fields = 0;
    for(i = 0; i < nb_fields; i++) {
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            present = *(BOOL *)(data + f->u.option_offset);
        } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
            val = *(uint32_t *)(data + ASN1_GET_SEQ_OFFSET(f));
            present = (val != f->u.default_value);
        } else {
            present = 1;
        }
        if (present) {
            table_fields[nb_table_fields].f = f;
            ret = get_tag(f->type, data + ASN1_GET_SEQ_OFFSET(f));
            if (ret == -1) 
                goto fail;
            table_fields[nb_table_fields].tag = ret;
            nb_table_fields++;
        }
        f++;
    }
    
    /* sort according to tag */
    qsort(table_fields, nb_table_fields, sizeof(SequenceField), 
          cmp_sequence_field);
    
    /* encode */
    asn1_ber_put_tag(&s->bb, tag, 1);
    start_pos = put_len_start(s);
    
    ret = 0;
    for(i = 0; i < nb_table_fields; i++) {
        f = table_fields[i].f;
        ret = asn1_ber_encode_type(s, f->type, data + ASN1_GET_SEQ_OFFSET(f));
        if (ret)
            break;
    }
    put_len_end(s, start_pos);
 fail:
    asn1_free(table_fields);
    return ret;
}

static int asn1_ber_encode_sequence_of(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data, unsigned int tag)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, i;
    ASN1String *str;
    uint32_t start_pos;

    str = (ASN1String *)data;
#ifdef DEBUG
    printf("seq of: len=%d\n", (int)str->len);
#endif
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    
    asn1_ber_put_tag(&s->bb, tag, 1);
    start_pos = put_len_start(s);
    
    ret = 0;
    for(i = 0; i < str->len; i++) {
        ret = asn1_ber_encode_type(s, f->type, str->buf + i * f->elem_size);
        if (ret)
            break;
    }
    put_len_end(s, start_pos);
    return ret;
}

static int cmp_set_of(const void *a1, const void *a2)
{
    int n, ret;
    const ASN1String *str1 = a1;
    const ASN1String *str2 = a2;
    
    n = min_int(str1->len, str2->len);
    if (n == 0) {
        ret = 0;
    } else {
        ret = memcmp(str1->buf, str2->buf, n);
    }
    if (ret != 0)
        return ret;
    if (str1->len < str2->len)
        return -1;
    else if (str1->len == str2->len)
        return 0;
    else
        return 1;
}

static int asn1_ber_encode_set_of_der(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data, unsigned int tag)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, i;
    ASN1String *str, *table;
    uint32_t start_pos;
    ASN1ByteBuffer saved_bb;

    str = (ASN1String *)data;
#ifdef DEBUG
    printf("seq of: len=%d\n", (int)str->len);
#endif
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    
    table = asn1_mallocz2(sizeof(ASN1String), str->len);
    if (!table)
        return -1;
    saved_bb = s->bb;
    
    /* generate each element in a separate buffer */
    ret = 0;
    for(i = 0; i < str->len; i++) {
        asn1_byte_buffer_init(&s->bb);
        ret = asn1_ber_encode_type(s, f->type, str->buf + i * f->elem_size);
        table[i].buf = s->bb.buf;
        table[i].len = s->bb.len;
        if (ret) {
            break;
        } else if (s->bb.has_error) {
            ret = -1;
            break;
        }
    }
    
    s->bb = saved_bb;

    if (!ret) {
        /* sort them */
        qsort(table, str->len, sizeof(ASN1String), cmp_set_of);
        
        /* write sorted elements */
        asn1_ber_put_tag(&s->bb, tag, 1);
        start_pos = put_len_start(s);
        
        for(i = 0; i < str->len; i++) {
            asn1_put_bytes(&s->bb, table[i].buf, table[i].len);
        }
        
        put_len_end(s, start_pos);
    }
    for(i = 0; i < str->len; i++) {
        asn1_free(table[i].buf);
    }
    asn1_free(table);
    return ret;
}

static int asn1_ber_encode_choice(ASN1EncodeState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int nb_fields, ret;
    uint32_t choice_val, flags, tag, start_pos;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) 
        nb_fields += *p++;
    p++;
    choice_val = *(uint32_t *)(data + p[0]);
    data += p[1];
    p += 2;
    /* check choice consistency */
    if (choice_val >= nb_fields)
        return -1;

    f = (const ASN1ChoiceField *)p + choice_val;

    /* if a tag is present, it is always explicit */
    tag = ASN1_GET_TAG(flags);
    if (tag != ASN1_NO_TAG) {
        asn1_ber_put_tag(&s->bb, tag, 1);
        start_pos = put_len_start(s);
    } else {
        start_pos = 0; /* avoids warning */
    }
    ret = asn1_ber_encode_type(s, f->type, data);
    if (tag != ASN1_NO_TAG) {
        put_len_end(s, start_pos);
    }
    return ret;
}

static int asn1_ber_encode_enumerated(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data, unsigned int tag)
{
    uint32_t val, nb_fields, flags;

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
        if (val >= nb_fields)
            return -1;
        /* convert value */
        val = p[val];
    }
    asn1_ber_put_tag(&s->bb, tag, 0);
    return asn1_ber_encode_int32(&s->bb, val);
}

static int asn1_ber_encode_null(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data, unsigned int tag)
{
    asn1_ber_put_tag(&s->bb, tag, 0);
    asn1_ber_put_len(&s->bb, 0);
    return 0;
}

static int asn1_ber_encode_real(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data, unsigned int tag)
{
    uint8_t buf[ASN1_DOUBLE_DER_MAX_LEN];
    int len;
    
    len = asn1_encode_real_der(buf, *(double *)data);
    asn1_ber_put_tag(&s->bb, tag, 0);
    asn1_ber_put_len(&s->bb, len);
    asn1_put_bytes(&s->bb, buf, len);
    return 0;
}

static int asn1_ber_encode_char_string(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data, unsigned int tag)
{
    int char_string_type, shift;
    ASN1String *str;

    str = (ASN1String *)data;
    char_string_type = p[1];
    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;
    if (shift == 0) {
        asn1_ber_encode_octet_string1(s, tag, str->buf, str->len);
    } else {
        int len, i;
        uint32_t c;
        uint8_t *buf1;
        
        len = str->len;
        /* XXX: could be more efficient */
        buf1 = asn1_malloc(len << shift);
        if (!buf1)
            return -1;

        if (shift == 1) {
            for(i = 0; i < len; i++) {
                c = ((uint16_t *)str->buf)[i];
                buf1[2 * i] = c >> 8;
                buf1[2 * i + 1] = c;
            }
        } else {
            for(i = 0; i < len; i++) {
                c = ((uint32_t *)str->buf)[i];
                buf1[4 * i] = c >> 24;
                buf1[4 * i + 1] = c >> 16;
                buf1[4 * i + 2] = c >> 8;
                buf1[4 * i + 3] = c;
            }
        }
        asn1_ber_encode_octet_string1(s, tag, buf1, len << shift);
        asn1_free(buf1);
    }
    return 0;
}

static int asn1_ber_encode_any(ASN1EncodeState *s, const ASN1CType *p, 
                               const uint8_t *data)
{
    const ASN1OpenType *str;
    int ret;
    uint32_t start_pos, flags, tag;

    flags = p[0];
    /* same as CHOICE: same encoding as the included type. If a tag is
       present, it is always explicit */
    tag = ASN1_GET_TAG(flags);
    if (tag != ASN1_NO_TAG) {
        asn1_ber_put_tag(&s->bb, tag, 1);
        start_pos = put_len_start(s);
    } else {
        start_pos = 0; /* avoids warning */
    }
    
    str = (const ASN1OpenType *)data;
    if (!str->type) {
        asn1_put_bytes(&s->bb, str->u.octet_string.buf, 
                       str->u.octet_string.len);
        ret = 0;
    } else {
        ret = asn1_ber_encode_type(s, str->type, str->u.data);
    }
    if (tag != ASN1_NO_TAG) {
        put_len_end(s, start_pos);
    }
    return ret;
}

static int asn1_ber_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    int type, ret;
    unsigned int tag, flags;

 redo1:
    flags = p[0];
    tag = ASN1_GET_TAG(flags);
 redo:
    type = ASN1_GET_CTYPE(flags);
#ifdef DEBUG
    printf("type=%s tag=0x%x\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?",
           tag);
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
        ret = asn1_ber_encode_sequence(s, p, data, tag);
        break;
    case ASN1_CTYPE_SET:
        if (s->params.use_sorted_set) 
            ret = asn1_ber_encode_set_der(s, p, data, tag);
        else
            ret = asn1_ber_encode_sequence(s, p, data, tag);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
        ret = asn1_ber_encode_sequence_of(s, p, data, tag);
        break;
    case ASN1_CTYPE_SET_OF:
        if (s->params.use_sorted_set_of)
            ret = asn1_ber_encode_set_of_der(s, p, data, tag);
        else
            ret = asn1_ber_encode_sequence_of(s, p, data, tag);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_ber_encode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_ber_encode_enumerated(s, p, data, tag);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_ber_encode_boolean(s, p, data, tag);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_ber_encode_integer(s, p, data, tag);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_ber_encode_null(s, p, data, tag);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_ber_encode_octet_string(s, p, data, tag);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_ber_encode_bit_string(s, p, data, tag);
        break;
    case ASN1_CTYPE_TAGGED:
        /* get pointer indirection */
        if (flags & ASN1_CTYPE_HAS_POINTER)
            data = *(void **)data;
        /* if no tag, use normal tag from pointed type */
        if (tag == ASN1_NO_TAG) {
            p = (ASN1CType *)p[1];
            goto redo1;
        } else if (flags & ASN1_CTYPE_HAS_EXPLICIT) {
            uint32_t start_pos;
            /* explicit tag */
            asn1_ber_put_tag(&s->bb, tag, 1);
            start_pos = put_len_start(s);
            ret = asn1_ber_encode_type(s, (ASN1CType *)p[1], data);
            put_len_end(s, start_pos);
        } else {
            /* implicit tag: overrides type tag */
            p = (ASN1CType *)p[1];
            flags = p[0];
            goto redo;
        }
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        {
            ASN1String *str;
            str = (ASN1String *)data;
            
            put_tl(s, tag, 0, str->len);
            asn1_put_bytes(&s->bb, str->buf, str->len);
            ret = 0;
        }
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_ber_encode_real(s, p, data, tag);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_ber_encode_char_string(s, p, data, tag);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_ber_encode_any(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

asn1_ssize_t asn1_ber_encode(uint8_t **pbuf, const ASN1CType *p, 
                             const void *data, const ASN1BERParams *params)
{
    ASN1EncodeState s_s, *s = &s_s;
    int ret;

    asn1_byte_buffer_init(&s->bb);
    s->params = *params;
    
    ret = asn1_ber_encode_type(s, p, data);
    if (ret) {
        goto fail;
    } else if (s->bb.has_error) {
        ret = -1;
    fail:
        asn1_free(s->bb.buf);
        *pbuf = NULL;
        return ret;
    } else {
        *pbuf = s->bb.buf;
        return s->bb.len;
    }
}
    
asn1_ssize_t asn1_der_encode(uint8_t **pbuf, const ASN1CType *p, 
                             const void *data)
{
    ASN1BERParams params;

    memset(&params, 0, sizeof(params));
    params.indefinite_len_for_constructed_encoding = FALSE;
    params.string_max_len = 0;
    params.use_sorted_set = TRUE;
    params.use_sorted_set_of = TRUE;
    return asn1_ber_encode(pbuf, p, data, &params);
}
