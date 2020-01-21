/* 
 * ASN1 GSER (text) encoder
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

typedef struct ASN1EncodeState {
    ASN1ByteBuffer bb;
    ASN1ByteBuffer *f;
    int indent;
    int hex_bit_string_enabled;
    void *filter_opaque;
    ASN1OutputFilterFunc *filter_func;
} ASN1EncodeState;

static int asn1_gser_encode_boolean(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int val;
    val = *(BOOL *)data != 0;
    if (val)
        asn1_printf(s->f, "TRUE");
    else
        asn1_printf(s->f, "FALSE");
    return 0;
}

/* contrainted signed or unsigned 32 bit number */
static int asn1_gser_encode_integer(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int flags;
    
    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        const ASN1Integer *val;
        char *ptr, *buf;
        val = (const ASN1Integer *)data;
        ptr = asn1_integer_to_str(val, &buf, NULL);
        if (!ptr)
            return -1;
        asn1_puts(s->f, ptr);
        asn1_free(buf);
#else
        return -1;
#endif
    } else {
        int val;
        val = *(int *)data;
        if (asn1_is_uint32(p))  {
            asn1_printf(s->f, "%u", val);
        } else {
            asn1_printf(s->f, "%d", val);
        }
    }
    return 0;
}

static void print_hex(ASN1EncodeState *s, const uint8_t *buf, int nb_bits)
{
    int nb_digits, i, c;

    asn1_put_byte(s->f, '\'');
    nb_digits = nb_bits / 4;
    for(i = 0; i < nb_digits; i++) {
        c = (buf[i >> 1] >> (4 - 4 * (i & 1))) & 0xf;
        if (c >= 10)
            c = c - 10 + 'A';
        else
            c += '0';
        asn1_put_byte(s->f, c);
    }
    asn1_put_byte(s->f, '\'');
    asn1_put_byte(s->f, 'H');
}

static int asn1_gser_encode_bit_string(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    const ASN1BitString *str;
    int i;

    str = (const ASN1BitString *)data;
    if (s->hex_bit_string_enabled && (str->len % 4) == 0) {
        print_hex(s, str->buf, str->len);
    } else {
        asn1_put_byte(s->f, '\'');
        for(i = 0; i < str->len; i++) {
            asn1_put_byte(s->f, '0' + get_bit(str->buf, i));
        }
        asn1_put_byte(s->f, '\'');
        asn1_put_byte(s->f, 'B');
    }
    return 0;
}

static int asn1_gser_encode_octet_string(ASN1EncodeState *s, const ASN1CType *p, 
                                        const uint8_t *data)
{
    const ASN1String *str;

    str = (const ASN1String *)data;
    print_hex(s, str->buf, str->len * 8);
    return 0;
}

static int asn1_gser_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data);

static void print_indent(ASN1EncodeState *s)
{
    int i;
    for(i = 0; i < s->indent; i++)
        asn1_printf(s->f, "  ");
}

static int asn1_gser_encode_sequence(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    int nb_fields, present, i, ret, flag, count;
    const ASN1SequenceField *f;
    uint32_t val;

    p++;
    nb_fields = *p++;
    p++;
    f = (const ASN1SequenceField *)p;

    asn1_printf(s->f, "{\n");
    s->indent++;
    count = 0;

    for(i = 0; i < nb_fields; i++) {
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            val = *(BOOL *)(data + f->u.option_offset);
            present = (val != 0);
        } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
            val = *(uint32_t *)(data + ASN1_GET_SEQ_OFFSET(f));
            present = (val != f->u.default_value);
        } else {
            present = 1;
        }
        if (present) {
            if (count != 0) {
                asn1_printf(s->f, ",\n");
            }
            print_indent(s);
            asn1_printf(s->f, "%s ", f->name);
            ret = asn1_gser_encode_type(s, f->type, 
                                        data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
            count++;
        }
        f++;
    }

    if (count != 0)
        asn1_printf(s->f, "\n");
    s->indent--;
    print_indent(s);
    asn1_printf(s->f, "}");
    return 0;
}

static int asn1_gser_encode_sequence_of(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    uint32_t i;
    int ret, flags;
    ASN1String *str;

    str = (ASN1String *)data;
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    asn1_printf(s->f, "{\n");
    s->indent++;
    for(i = 0; i < str->len; i++) {
        print_indent(s);
        ret = asn1_gser_encode_type(s, f->type, str->buf + i * f->elem_size);
        if (ret)
            return ret;
        if (i != (str->len - 1))
            asn1_printf(s->f, ",");
        asn1_printf(s->f, "\n");
    }
    s->indent--;
    print_indent(s);
    asn1_printf(s->f, "}");
    return 0;
}

static int asn1_gser_encode_choice(ASN1EncodeState *s, const ASN1CType *p, 
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
    asn1_printf(s->f, "%s: ", f->name);
    ret = asn1_gser_encode_type(s, f->type, data + data_offset);
    return ret;
}

static int asn1_gser_encode_enumerated(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    int has_ext, nb_fields, nb_ext_fields;
    uint32_t val;
    const char *name;

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
    name = *(char **)(p + val);
    asn1_printf(s->f, "%s", name);
    return 0;
}

static int asn1_gser_encode_null(ASN1EncodeState *s, const ASN1CType *p, 
                                 const uint8_t *data)
{
    asn1_printf(s->f, "NULL");
    return 0;
}

static int asn1_gser_encode_object_identifier(ASN1EncodeState *s, const ASN1CType *p, 
                                              const uint8_t *data)
{
    const ASN1String *str;
    BOOL is_relative, is_first;
    int v, b;
    const uint8_t *r, *buf_end;
    
    is_relative = ASN1_GET_CTYPE(p[0]) == ASN1_CTYPE_RELATIVE_OID;

    str = (const ASN1String *)data;
    r = str->buf;
    buf_end = str->buf + str->len;
    is_first = TRUE;
    while (r < buf_end) {
        b = *r++;
        v = b & 0x7f;
        while (b & 0x80) {
            if (r >= buf_end)
                return -1;
            b = *r++;
            v = (v << 7) | (b & 0x7f);
        }
        if (!is_first)
            asn1_printf(s->f, ".");
        if (is_first && !is_relative) {
            asn1_printf(s->f, "%d.%d", v / 40, v % 40);
        } else {
            asn1_printf(s->f, "%d", v);
        }
        is_first = FALSE;
    }
    return 0;
}

static int asn1_gser_encode_char_string(ASN1EncodeState *s, const ASN1CType *p, 
                                        const uint8_t *data)
{
    const ASN1String *str;
    uint32_t c;
    int len, i, char_string_type, shift, l;
    uint8_t buf[ASN1_UTF8_MAX_LEN];

    char_string_type = p[1];
    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;

    str = (const ASN1String *)data;
    len = str->len;
    asn1_put_byte(s->f, '\"');
    for(i = 0; i < len; i++) {
        if (shift == 0)
            c = ((uint8_t *)str->buf)[i];
        else if (shift == 1)
            c = ((uint16_t *)str->buf)[i];
        else
            c = ((uint32_t *)str->buf)[i];
        if (c >= 0x80000000)
            return -1;
        if (char_string_type == ASN1_CSTR_UTF8String) {
            asn1_put_byte(s->f, c);
        } else {
            l = asn1_to_utf8(buf, c);
            asn1_put_bytes(s->f, buf, l);
        }
        if (c == '\"')
            asn1_put_byte(s->f, '\"');
    }
    asn1_put_byte(s->f, '\"');

    return 0;
}

static int asn1_gser_encode_real(ASN1EncodeState *s, const ASN1CType *p, 
                                 const uint8_t *data)
{
    union {
        double d;
        uint64_t v;
    } u;
    int sgn, e;
    uint64_t mant;
    char buf[64];
    const char *r;

    u.v = *(uint64_t *)data;
    mant = u.v & (((uint64_t)1 << 52) - 1);
    e = (u.v >> 52) & 0x7ff;
    sgn = u.v >> 63;
    
    if (e == 0 && mant == 0) {
        if (sgn)
            asn1_printf(s->f, "-");
        asn1_printf(s->f, "0");
    } else if (e == 0x7ff) {
        if (mant == 0) {
            if (sgn)
                asn1_printf(s->f, "MINUS-INFINITY");
            else
                asn1_printf(s->f, "PLUS-INFINITY");
        } else {
            asn1_printf(s->f, "NOT-A-NUMBER");
        }
    } else {
        snprintf(buf, sizeof(buf), "%0.18E", u.d);
        /* don't put a plus sign in the exponent */
        r = buf;
        while (*r) {
            asn1_put_byte(s->f, *r);
            if (*r == 'E' && r[1] == '+') {
                r++;
            }
            r++;
        }
    }
    return 0;
}

static int asn1_gser_encode_any(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    const ASN1OpenType *str;

    str = (const ASN1OpenType *)data;
    if (!str->type) {
        print_hex(s, str->u.octet_string.buf, str->u.octet_string.len * 8);
        return 0;
    } else {
        return asn1_gser_encode_type(s, str->type, str->u.data);
    }
}

static int asn1_gser_encode_tagged(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int flags;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data = *(void **)data;
    }
    return asn1_gser_encode_type(s, (ASN1CType *)p[1], data);
}

static int asn1_gser_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data)
{
    int type, ret;

    if (unlikely(s->filter_func)) {
        uint8_t *buf;
        int buf_len;
        buf_len = s->filter_func(s->filter_opaque, &buf, p, data, s->indent);
        if (buf_len >= 0) {
            asn1_put_bytes(s->f, buf, buf_len);
            asn1_free(buf);
            return 0;
        }
    }

    type = ASN1_GET_CTYPE(p[0]);
#ifdef DEBUG
    printf("type=%s\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?");
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_gser_encode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_gser_encode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_gser_encode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_gser_encode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_gser_encode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_gser_encode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_gser_encode_null(s, p, data);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_gser_encode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_gser_encode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_gser_encode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_gser_encode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_gser_encode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_gser_encode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_gser_encode_any(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

/* GSER text encoding to buffer. Return 0 if OK, -1 if error. */
asn1_ssize_t asn1_gser_encode2(uint8_t **pbuf, const ASN1CType *p, const void *data,
                           const ASN1GSERParams *params)
{
    ASN1EncodeState s_s, *s = &s_s;
    int ret;
    
    asn1_byte_buffer_init(&s->bb);
    s->f = &s->bb;
    s->indent = params->indent;
    s->hex_bit_string_enabled = !params->hex_bit_string_disabled;
    s->filter_opaque = params->filter_opaque;
    s->filter_func = params->filter_func;

    ret = asn1_gser_encode_type(s, p, data);
    asn1_put_byte(s->f, '\n');
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

asn1_ssize_t asn1_gser_encode(uint8_t **pbuf, const ASN1CType *p, const void *data)
{
    ASN1GSERParams params;
    memset(&params, 0, sizeof(params));
    return asn1_gser_encode2(pbuf, p, data, &params);
}
