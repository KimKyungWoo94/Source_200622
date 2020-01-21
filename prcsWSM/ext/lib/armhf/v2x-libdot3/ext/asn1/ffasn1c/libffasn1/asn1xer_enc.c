/* 
 * ASN1 XER encoder
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
    BOOL add_indentation;
    BOOL use_empty_tags;
    BOOL use_boolean_content;
    BOOL use_enumerated_content;
    int indent;
    char *pending_tag_name;
    BOOL content_in_tag;
} ASN1EncodeState;

static void print_indent(ASN1EncodeState *s)
{
    int i;
    if (!s->add_indentation)
        return;
    for(i = 0; i < s->indent; i++)
        asn1_printf(s->f, "  ");
}

/* start a new tag */
static void start_tag(ASN1EncodeState *s, const char *tag_name)
{
    if (s->pending_tag_name) {
        asn1_printf(s->f, "<%s>", s->pending_tag_name);
        if (s->add_indentation)
            asn1_printf(s->f, "\n");
        asn1_free(s->pending_tag_name);
        s->pending_tag_name = NULL;
    }
    print_indent(s);
    s->indent++;
    s->pending_tag_name = asn1_strdup(tag_name);
}

static void end_tag(ASN1EncodeState *s, const char *tag_name)
{
    s->indent--;
    if (s->pending_tag_name) {
        assert(!strcmp(tag_name, s->pending_tag_name));
        assert(!s->content_in_tag);
 
        if (s->use_empty_tags) {
            /* empty tag */
            asn1_printf(s->f, "<%s/>", tag_name);
            if (s->add_indentation)
                asn1_printf(s->f, "\n");
            asn1_free(s->pending_tag_name);
            s->pending_tag_name = NULL;
        } else {
            if (s->pending_tag_name) {
                asn1_printf(s->f, "<%s>", s->pending_tag_name);
                if (s->add_indentation)
                    asn1_printf(s->f, "\n");
                asn1_free(s->pending_tag_name);
                s->pending_tag_name = NULL;
            }
            goto next;
        }
    } else {
    next:
        if (s->content_in_tag) {
            /* if content was printed, we stay on the same line */
            s->content_in_tag = FALSE;
        } else {
            print_indent(s);
        }
        asn1_printf(s->f, "</%s>", tag_name);
        if (s->add_indentation)
            asn1_printf(s->f, "\n");
    }
}

/* must be called after start_tag() */
static void start_content(ASN1EncodeState *s)
{
    if (s->pending_tag_name) {
        asn1_printf(s->f, "<%s>", s->pending_tag_name);
        asn1_free(s->pending_tag_name);
        s->pending_tag_name = NULL;
    }
    s->content_in_tag = TRUE;
}

static void end_content(ASN1EncodeState *s)
{
}

static int asn1_xer_encode_boolean(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int val;
    const char *tag_name;
    val = *(BOOL *)data != 0;
    tag_name = val ? "true" : "false";
    if (s->use_boolean_content) {
        start_content(s);
        asn1_puts(s->f, tag_name);
        end_content(s);
    } else {
        start_tag(s, tag_name);
        end_tag(s, tag_name);
    }
    return 0;
}

/* contrainted signed or unsigned 32 bit number */
static int asn1_xer_encode_integer(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int flags;
    
    flags = *p;
    start_content(s);
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
        if (asn1_is_uint32(p)) 
            asn1_printf(s->f, "%u", val);
        else
            asn1_printf(s->f, "%d", val);
    }
    end_content(s);
    return 0;
}

static void print_hex(ASN1EncodeState *s, const uint8_t *buf, int nb_bits)
{
    int nb_digits, i, c;

    nb_digits = nb_bits / 4;
    for(i = 0; i < nb_digits; i++) {
        c = (buf[i >> 1] >> (4 - 4 * (i & 1))) & 0xf;
        if (c >= 10)
            c = c - 10 + 'A';
        else
            c += '0';
        asn1_put_byte(s->f, c);
    }
}

static int asn1_xer_encode_bit_string(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    const ASN1BitString *str;
    int i;

    str = (const ASN1BitString *)data;
    if (str->len > 0) {
        start_content(s);
        for(i = 0; i < str->len; i++) {
            asn1_put_byte(s->f, '0' + get_bit(str->buf, i));
        }
        end_content(s);
    }
    return 0;
}

static int asn1_xer_encode_octet_string(ASN1EncodeState *s, const ASN1CType *p, 
                                        const uint8_t *data)
{
    const ASN1String *str;

    str = (const ASN1String *)data;
    if (str->len > 0) {
        start_content(s);
        print_hex(s, str->buf, str->len * 8);
        end_content(s);
    }
    return 0;
}

static int asn1_xer_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
                                const uint8_t *data);
static int asn1_xer_encode_typed_value(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data);

static int get_untagged_type(const ASN1CType *p)
{
    int type;
    for(;;) {
        type = ASN1_GET_CTYPE(p[0]);
        if (type != ASN1_CTYPE_TAGGED)
            break;
        p = (ASN1CType *)p[1];
    }
    return type;
}

static int asn1_xer_encode_sequence(ASN1EncodeState *s, const ASN1CType *p, 
                                      const uint8_t *data)
{
    int nb_fields, present, i, ret, flag;
    const ASN1SequenceField *f;
    uint32_t val;

    p++;
    nb_fields = *p++;
    p++;
    f = (const ASN1SequenceField *)p;

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
            start_tag(s, f->name);
            ret = asn1_xer_encode_type(s, f->type, 
                                       data + ASN1_GET_SEQ_OFFSET(f));
            if (ret < 0)
                return ret;
            end_tag(s, f->name);
        }
        f++;
    }
    return 0;
}

static int asn1_xer_encode_sequence_of(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    uint32_t i;
    int ret, flags, type;
    ASN1String *str;

    str = (ASN1String *)data;
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    type = get_untagged_type(f->type);
    for(i = 0; i < str->len; i++) {
        if (f->name) {
            start_tag(s, f->name);
            ret = asn1_xer_encode_type(s, f->type, str->buf + i * f->elem_size);
            if (ret < 0)
                return ret;
            end_tag(s, f->name);
        } else if (type == ASN1_CTYPE_CHOICE) {
            ret = asn1_xer_encode_type(s, f->type, str->buf + i * f->elem_size);
            if (ret < 0)
                return ret;
        } else {
            ret = asn1_xer_encode_typed_value(s, f->type, str->buf + i * f->elem_size);
            if (ret < 0)
                return ret;
        }
    }
    return 0;
}

static int asn1_xer_encode_choice(ASN1EncodeState *s, const ASN1CType *p, 
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
    start_tag(s, f->name);
    ret = asn1_xer_encode_type(s, f->type, data + data_offset);
    if (ret < 0)
        return ret;
    end_tag(s, f->name);
    return 0;
}

static int asn1_xer_encode_enumerated(ASN1EncodeState *s, const ASN1CType *p, 
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
    if (s->use_enumerated_content) {
        start_content(s);
        asn1_puts(s->f, name);
        end_content(s);
    } else {
        start_tag(s, name);
        end_tag(s, name);
    }
    return 0;
}            

static int asn1_xer_encode_null(ASN1EncodeState *s, const ASN1CType *p, 
                                 const uint8_t *data)
{
    return 0;
}

static int asn1_xer_encode_object_identifier(ASN1EncodeState *s, const ASN1CType *p, 
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
    start_content(s);
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
    end_content(s);
    return 0;
}

static int asn1_xer_encode_char_string(ASN1EncodeState *s, const ASN1CType *p, 
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
    start_content(s);
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
            if (c < 32 || c == '&' || c == '<' || c == '>') {
                asn1_printf(s->f, "&#%d;", c);
            } else {
                asn1_put_byte(s->f, c);
            }
        } else {
            if (c < 32 || c == '&' || c == '<' || c == '>') {
                asn1_printf(s->f, "&#%d;", c);
            } else {
                l = asn1_to_utf8(buf, c);
                asn1_put_bytes(s->f, buf, l);
            }
        }
    }
    end_content(s);
    return 0;
}

static int asn1_xer_encode_real(ASN1EncodeState *s, const ASN1CType *p, 
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
    
    start_content(s);
    if (e == 0 && mant == 0) {
        if (sgn)
            asn1_printf(s->f, "-");
        asn1_printf(s->f, "0");
    } else if (e == 0x7ff) {
        if (mant == 0) {
            if (sgn)
                asn1_printf(s->f, "-INF");
            else
                asn1_printf(s->f, "INF");
        } else {
            asn1_printf(s->f, "NaN");
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
    end_content(s);
    return 0;
}

static int asn1_xer_encode_any(ASN1EncodeState *s, const ASN1CType *p, 
                               const uint8_t *data)
{
    const ASN1OpenType *str;

    str = (const ASN1OpenType *)data;
    if (!str->type) {
        start_tag(s, "OCTET_STRING");
        start_content(s);
        print_hex(s, str->u.octet_string.buf, str->u.octet_string.len * 8);
        end_content(s);
        end_tag(s, "OCTET_STRING");
        return 0;
    } else {
        return asn1_xer_encode_typed_value(s, str->type, str->u.data);
    }
}

static int asn1_xer_encode_tagged(ASN1EncodeState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int flags;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data = *(void **)data;
    }
    return asn1_xer_encode_type(s, (ASN1CType *)p[1], data);
}

static int asn1_xer_encode_type(ASN1EncodeState *s, const ASN1CType *p, 
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
        ret = asn1_xer_encode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_xer_encode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_xer_encode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_xer_encode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_xer_encode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_xer_encode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_xer_encode_null(s, p, data);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_xer_encode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_xer_encode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_xer_encode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_xer_encode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_xer_encode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_xer_encode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_xer_encode_any(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

static int asn1_xer_encode_typed_value(ASN1EncodeState *s, const ASN1CType *p, 
                                       const uint8_t *data)
{
    int ret;
    const char *type_name;
    
    type_name = asn1_get_type_name(p);

    start_tag(s, type_name);
    ret = asn1_xer_encode_type(s, p, data);
    if (ret < 0)
        return ret;
    end_tag(s, type_name);
    return ret;
}

/* XER text encoding. Return >= 0 if OK, -1 if error. */
asn1_ssize_t asn1_xer_encode2(uint8_t **pbuf, const ASN1CType *p, const void *data,
                              const ASN1XERParams *params)
{
    ASN1EncodeState s_s, *s = &s_s;
    int ret;
    
    memset(s, 0, sizeof(*s));

    asn1_byte_buffer_init(&s->bb);
    s->f = &s->bb;
    s->add_indentation = params->add_indentation;
    s->use_empty_tags = params->use_empty_tags;
    s->use_boolean_content = params->use_boolean_content;
    s->use_enumerated_content = params->use_enumerated_content;

    s->pending_tag_name = NULL;
    s->content_in_tag = FALSE;

    ret = asn1_xer_encode_typed_value(s, p, data);
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

asn1_ssize_t asn1_xer_encode(uint8_t **pbuf, const ASN1CType *p, const void *data)
{
    ASN1XERParams params;

    memset(&params, 0, sizeof(params));
    /* by default, output is close to canonical XER with indentation */
    params.add_indentation = TRUE;
    params.use_empty_tags = TRUE;
    params.use_boolean_content = FALSE;
    params.use_enumerated_content = FALSE;
    return asn1_xer_encode2(pbuf, p, data, &params);
}
