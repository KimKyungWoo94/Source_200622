/* 
 * ASN1 XER decoder
 * Copyright (C) 2011-2018 Fabrice Bellard
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

enum {
    TOK_XT_START,
    TOK_XT_END,
    TOK_XT_CONTENT,
    TOK_ERROR,
    TOK_EOF,
};

typedef struct {
    char *name;
    /* attributes are not used yet */
} ASN1XMLTag;

typedef struct {
    int val;
    union {
        ASN1XMLTag tag;
        ASN1String content;
    } u;
} ASN1Token;

typedef struct ASN1DecodeState {
    const uint8_t *buf_ptr;
    const uint8_t *buf_end;

    int line_num;

    ASN1Token tok;
    BOOL empty_tag;
    ASN1ValueStack *top_value;

    ASN1Error error;
} ASN1DecodeState;

static int asn1_xer_decode_typed_value(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data);
static int asn1_xer_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data);

static int parse_error_internal(ASN1DecodeState *s)
{
    s->error.line_num = s->line_num;
    return -1;
}

static __attribute__((format(printf, 2, 3))) 
    int parse_error(ASN1DecodeState *s, const char *fmt, ...)
{
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(s->error.msg, sizeof(s->error.msg), fmt, ap);
    va_end(ap);
    return parse_error_internal(s);
}

static int mem_error(ASN1DecodeState *s)
{
    return parse_error(s, "not enough memory");
}

static void free_token(ASN1Token *tok)
{
    switch(tok->val) {
    case TOK_XT_START:
    case TOK_XT_END:
        asn1_free(tok->u.tag.name);
        tok->u.tag.name = NULL;
        break;
    case TOK_XT_CONTENT:
        asn1_free(tok->u.content.buf);
        tok->u.content.buf = NULL;
        break;
    default:
        break;
    }
}

#ifdef DEBUG
static void dump_token(const ASN1Token *tok)
{
    switch(tok->val) {
    case TOK_XT_START:
        printf("<%s>", tok->u.tag.name);
        break;
    case TOK_XT_END:
        printf("</%s>", tok->u.tag.name);
        break;
    case TOK_XT_CONTENT:
        printf("{%s}", tok->u.content.buf);
        break;
    case TOK_ERROR:
        printf("ERROR");
        break;
    case TOK_EOF:
        printf("EOF");
        break;
    default:
        printf("Unknown token %d", tok->val);
        break;
    }
}
#endif

/* '*p' must be '!' or '?' */
static int skip_comment(ASN1DecodeState *s, const uint8_t **pp)
{
    const uint8_t *p;

    p = *pp;
    if ((p + 2) < s->buf_end && 
        p[0] == '!' && p[1] == '-' && p[2] == '-') {
        /* skip comment */
        p += 2;
        for(;;) {
            if ((p + 2) >= s->buf_end) {
                parse_error(s, "unexpected end of stream in comment");
                *pp = p;
                return -1;
            }
            if (p[0] == '-' && p[1] == '-' && p[2] == '>')
                break;
            if (*p == '\n')
                s->line_num++;
            p++;
        }
        p += 3;
    } else {
        p++;
        /* Note: we skip XML and SGML processing instructions */
        for(;;) {
            if (p >= s->buf_end) {
                parse_error(s, "unexpected end of stream in processing instruction");
                *pp = p;
                return -1;
            }
            if (*p == '>')
                break;
            if (*p == '\n')
                s->line_num++;
            p++;
        }
        p++;
    } 
    *pp = p;
    return 0;
}

static int next_token(ASN1DecodeState *s)
{
    const uint8_t *p;
    char tag_name[1024], *q;
    BOOL closing_tag, both_tag;

    if (s->empty_tag) {
        s->empty_tag = FALSE;
        s->tok.val = TOK_XT_END;
        return 0;
    }

    free_token(&s->tok);
    
    p = s->buf_ptr;

    /* skip spaces */
 redo:
    if (p >= s->buf_end) 
        goto eof;

    if (*p == '<') {
        /* start of tag */
        p++;
        if (p >= s->buf_end)
            goto eof;
        if (*p == '!' || *p == '?') {
            if (skip_comment(s, &p) < 0)
                goto error;
            goto redo;
        } else {
            closing_tag = FALSE;
            if (*p == '/') {
                closing_tag = TRUE;
                p++;
            }

            /* get tag name */
            q = tag_name;
            for(;;) {
                if (p >= s->buf_end)
                    goto error_end_tag;
                if (isspace(*p) || *p == '/' || *p == '>')
                    break;
                if ((q - tag_name) >= sizeof(tag_name) - 1) {
                    parse_error(s, "tag name too long");
                    goto error;
                }
                *q++ = *p++;
            }
            *q = '\0';

            /* skip up to the end */
            while (*p != '>' && *p != '/') {
                if (*p == '\n')
                    s->line_num++;
                p++;
                if (p >= s->buf_end)
                    goto error_end_tag;
            }

            both_tag = FALSE;
            if (*p == '/') {
                both_tag = TRUE;
                p++;
                if (p >= s->buf_end)
                    goto error_end_tag;
            }
            if (*p != '>') {
            error_end_tag:
                parse_error(s, "invalid tag syntax: expecting '>'");
                goto error;
            }
            p++;
            if (both_tag && closing_tag) {
                parse_error(s, "invalid closing tag syntax");
                goto error;
            }
            if (both_tag) {
                s->tok.val = TOK_XT_START;
                s->empty_tag = TRUE; /* the end tag will be synthetized */
            } else if (closing_tag) {
                s->tok.val = TOK_XT_END;
            } else {
                s->tok.val = TOK_XT_START;
            }
            s->tok.u.tag.name = asn1_strdup(tag_name);
        }
    } else {
        /* get the content */
        ASN1ByteBuffer bb;
        
        asn1_byte_buffer_init(&bb);
        for(;;) {
            if (p >= s->buf_end) 
                break;
            if (*p == '<') {
                if ((p + 1) < s->buf_end &&
                    (p[1] == '!' || p[1] == '?')) {
                    p++;
                    if (skip_comment(s, &p) < 0)
                        goto error;
                } else {
                    break;
                }
            } else if (*p == '&') {
                char entity_buf[128], *q;
                uint8_t buf[7];
                int c, l;
                p++;
                q = entity_buf;
                for(;;) {
                    if (p >= s->buf_end) {
                        goto entity_error;
                    } else if (isspace(*p)) {
                        goto entity_error;
                    } else if (*p == ';') {
                        p++;
                        break;
                    }
                    if ((q - entity_buf) >= sizeof(entity_buf) - 1)
                        goto entity_error;
                    *q++ = *p++;
                }
                *q = '\0';
                if (entity_buf[0] == '#') {
                    /* XXX: error handling */
                    if (entity_buf[1] == 'x')
                        c = strtoul(entity_buf + 2, NULL, 16);
                    else
                        c = strtoul(entity_buf + 1, NULL, 10);
                } else if (!strcmp(entity_buf, "gt")) {
                    c = '>';
                } else if (!strcmp(entity_buf, "lt")) {
                    c = '<';
                } else if (!strcmp(entity_buf, "amp")) {
                    c = '&';
                } else {
                entity_error:
                    parse_error(s, "invalid XML entity syntax");
                    goto error;
                }
                l = asn1_to_utf8(buf, c);
                asn1_put_bytes(&bb, buf, l);
            } else if (*p == '\n') {
                p++;
                s->line_num++;
                asn1_put_byte(&bb, '\n');
            } else if (*p == '\r') {
                if ((p + 1) < s->buf_end && p[1] == '\n') {
                    p += 2;
                } else {
                    p++;
                }
                s->line_num++;
                asn1_put_byte(&bb, '\n');
            } else {
                asn1_put_byte(&bb, *p++);
            }
        }
        /* Note: we add a trailing '\0' to ease parsing */
        asn1_put_byte(&bb, '\0');
        if (bb.has_error) {
            mem_error(s);
            asn1_free(bb.buf);
            goto error;
        }
        s->tok.val = TOK_XT_CONTENT;
        s->tok.u.content.buf = bb.buf;
        s->tok.u.content.len = bb.len - 1;
    }
    s->buf_ptr = p;
#ifdef DEBUG
    dump_token(&s->tok);
#endif
    return 0;
 eof:
    s->buf_end = p;
    s->tok.val = TOK_EOF;
    return 0;
 error:
    s->buf_end = p;
    s->tok.val = TOK_ERROR;
    return -1;
}

static int skip_spaces(ASN1DecodeState *s)
{
    const uint8_t *r, *r_end;
    if (s->tok.val == TOK_XT_CONTENT) {
        r = s->tok.u.content.buf;
        r_end = r + s->tok.u.content.len;
        while (r < r_end) {
            if (!isspace(*r)) {
                return parse_error(s, "unexpected XML content");
            }
            r++;
        }
        if (next_token(s))
            return -1;
    }
    return 0;
}
static int skip_closing_tag(ASN1DecodeState *s, const char *name)
{
    if (skip_spaces(s))
        return -1;
    if (s->tok.val != TOK_XT_END ||
        strcmp(name, s->tok.u.tag.name) != 0) {
        return parse_error(s, "expecting closing tag </%s>", name);
    }
    return next_token(s);
}

static int asn1_xer_decode_boolean(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    uint32_t val;
    char *name;
    const uint8_t *r, *r_end;
    const char *value;

    if (s->tok.val == TOK_XT_CONTENT) {
        r = s->tok.u.content.buf;
        r_end = r + s->tok.u.content.len;
        while (r < r_end) {
            if (!isspace(*r)) {
                value = (char *)s->tok.u.content.buf;
                name = NULL;
                goto use_content;
            }
            r++;
        }
        if (next_token(s))
            return -1;
    }
    if (s->tok.val != TOK_XT_START) {
        name = NULL;
        goto fail1;
    }
    name = asn1_strdup(s->tok.u.tag.name);
    value = name;
 use_content:
    if (!strcmp(value, "true")) {
        val = 1;
    } else if (!strcmp(value, "false")) {
        val = 0;
    } else {
    fail1:
        parse_error(s, "expecting boolean value");
        goto fail;
    }
    if (next_token(s))
        goto fail;
    if (name) {
        if (skip_closing_tag(s, name))
            goto fail;
        asn1_free(name);
    }
    *(BOOL *)data = val;
    return 0;
 fail:
    asn1_free(name);
    return -1;
}

static int asn1_xer_decode_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    int flags, is_unsigned;
    int64_t val;
#ifdef ASN1_USE_LARGE_INTEGER
    int ret;
    ASN1Integer bint;
#endif
    const char *r;

    if (s->tok.val != TOK_XT_CONTENT)
        goto fail;
    r = (const char *)s->tok.u.content.buf;
    while (isspace(*r))
        r++;
#ifdef ASN1_USE_LARGE_INTEGER
    asn1_integer_init(&bint);
    if (asn1_integer_from_str(&bint, (char **)&r)) {
        asn1_integer_clear(&bint);
        return mem_error(s);
    }
#else
    {
        int is_neg = 0;

        if (*r == '-') {
            is_neg = 1;
            r++;
        }
        val = strtoul(r, (char **)&r, 10);
        if (is_neg)
            val = -val;
    }
#endif
    while (isspace(*r))
        r++;
    if (*r != '\0') {
#ifdef ASN1_USE_LARGE_INTEGER
        asn1_integer_clear(&bint);
#endif
    fail:
        return parse_error(s, "expecting integer value");
    }

    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        *(ASN1Integer *)data = bint;
#else
        goto too_large;
#endif
    } else {
        is_unsigned = asn1_is_uint32(p); 
#ifdef ASN1_USE_LARGE_INTEGER
        ret = asn1_integer_get_si64_ov(&bint, &val);
        asn1_integer_clear(&bint);
        if (ret) 
            goto too_large;
#endif
        if (is_unsigned) {
            if (val != (uint32_t)val)
                goto too_large;
        } else {
            if (val != (int32_t)val) {
            too_large:
                return parse_error(s, "integer too large");
            }
        }
        *(int32_t *)data = val;
    }
    return next_token(s);
}

static int asn1_xer_decode_bit_string(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    uint32_t pos, byte_len, max_byte_len;
    ASN1BitString *str;
    int c;
    const char *r;

    str = (ASN1BitString *)data;
    if (s->tok.val == TOK_XT_END) {
        /* empty string */
        str->buf = asn1_malloc(0);
        str->len = 0;
        return 0;
    } else {
        if (s->tok.val != TOK_XT_CONTENT) {
            return parse_error(s, "expecting bit string value");
        }
        r = (const char *)s->tok.u.content.buf;
        max_byte_len = (strlen(r) + 7) / 8;
        str->buf = asn1_mallocz(max_byte_len);
        if (!str->buf)
            return mem_error(s);
        pos = 0;
        for(;;) {
            c = *r++;
            if (c == '0' || c == '1') {
                put_bit(str->buf, pos++, c - '0');
            } else if (c == '\0') {
                break;
            } else if (isspace(c)) {
                /* skip */
            } else {
                return parse_error(s, "expecting binary digits in bit string");
            }
        }
        byte_len = (pos + 7) / 8;
        if (byte_len != max_byte_len) {
            /* avoids loosing memory */
            str->buf = asn1_realloc(str->buf, byte_len);
        }
        str->len = pos;
        return next_token(s);
    }
}

static int asn1_xer_decode_octet_string(ASN1DecodeState *s, const ASN1CType *p, 
                                        uint8_t *data)
{
    uint32_t max_byte_len, byte_len, pos;
    ASN1String *str;
    int c;
    const char *r;

    str = (ASN1String *)data;
    if (s->tok.val == TOK_XT_END) {
        /* empty string */
        str->buf = asn1_malloc(0);
        str->len = 0;
        return 0;
    } else {
        if (s->tok.val != TOK_XT_CONTENT) {
            return parse_error(s, "expecting octet string value");
        }
        
        r = (const char *)s->tok.u.content.buf;
        max_byte_len = (strlen(r) + 1) / 2;
        str->buf = asn1_mallocz(max_byte_len);
        if (!str->buf)
            return mem_error(s);
        pos = 0;
        for(;;) {
            c = *r++;
            if (c >= '0' && c <= '9') {
                c -= '0';
                goto hex_digit;
            } else if (c >= 'A' && c <= 'F') {
                c = c - 'A' + 10;
                goto hex_digit;
            } else if (c >= 'a' && c <= 'f') {
                c = c - 'a' + 10;
            hex_digit:
                str->buf[pos >> 1] |= c << (4 - 4 * (pos & 1));
                pos++;
            } else if (c == '\0') {
                break;
            } else if (isspace(c)) {
                /* skip */
            } else {
                return parse_error(s, "expecting hexadecimal digits in octet string");
            }
        }
        byte_len = (pos + 1) / 2;
        if (byte_len != max_byte_len) {
            /* avoids loosing memory */
            str->buf = asn1_realloc(str->buf, byte_len);
        }
        str->len = byte_len;
        return next_token(s);
    }
}

static int asn1_xer_decode_sequence(ASN1DecodeState *s, const ASN1CType *p1, 
                                     uint8_t *data)
{
    int nb_fields, ret, idx;
    const ASN1SequenceField *f, *f1;
    int flag;
    uint8_t *table_present;
    char *field_name;
    const ASN1CType *p;
    ASN1ValueStack value_entry;

    value_entry.prev = s->top_value;
    value_entry.type = p1;
    value_entry.data = data;
    s->top_value = &value_entry;

    p = p1;
    p++;
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;

    table_present = asn1_mallocz(nb_fields);
    if (!table_present)
        return mem_error(s);
    
    for(;;) {
        if (skip_spaces(s))
            goto fail1;
        if (s->tok.val != TOK_XT_START)
            break;
        field_name = asn1_strdup(s->tok.u.tag.name);
        /* find field description */
        f = NULL;
        for(idx = 0; idx < nb_fields; idx++) {
            f = f1 + idx;
            if (!strcmp(f->name, field_name))
                break;
        }
        if (idx == nb_fields) {
            parse_error(s, "sequence/set component '%s' not found", 
                        field_name);
            goto fail;
        }
        if (table_present[idx]) {
            parse_error(s, "sequence/set component %s defined twice", 
                        field_name);
            goto fail;
        }
        if (next_token(s))
            goto fail;
        
        ret = asn1_xer_decode_type(s, f->type, 
                                    data + ASN1_GET_SEQ_OFFSET(f));
        if (ret)
            goto fail;
        
        /* set field as present */
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            *(BOOL *)(data + f->u.option_offset) = TRUE;
        }
        table_present[idx] = 1;

        if (skip_closing_tag(s, field_name))
            goto fail;
        asn1_free(field_name);
        field_name = NULL;
    }

    if (asn1_sequence_set_default_fields(f1, nb_fields, data, table_present)) {
        ret = mem_error(s);
        goto fail1;
    }

    if (asn1_sequence_check_fields(f1, nb_fields, table_present, 
                                   s->error.msg, sizeof(s->error.msg))) {
        ret = parse_error_internal(s);
        goto fail1;
    }
    asn1_free(table_present);
    s->top_value = s->top_value->prev;
    return 0;
 fail:
    asn1_free(field_name);
 fail1:
    asn1_free(table_present);
    return -1;
}

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

static int asn1_xer_decode_sequence_of(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, type;
    ASN1String *str;
    uint8_t *buf;
    size_t size;
    
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) 
        p++;
    f = (const ASN1SequenceOfCType *)p;
    str = (ASN1String *)data;
    str->len = 0;
    str->buf = NULL;
    size = 0;
    for(;;) {
        if (skip_spaces(s))
            return -1;
        if (s->tok.val != TOK_XT_START)
            break;
        if ((str->len + 1) > size) {
            if (asn1_reallocz_buf(&str->buf, f->elem_size, &size, str->len + 1))
                return mem_error(s);
        }
        str->len++;
        if (f->name) {
            /* explicit field name */
            if (strcmp(f->name, s->tok.u.tag.name) != 0) {
                return parse_error(s, "expecting <%s> tag\n", f->name);
            }
            if (next_token(s))
                return -1;
            ret = asn1_xer_decode_type(s, f->type, 
                                       str->buf + (str->len - 1) * 
                                       f->elem_size);
            if (ret)
                return ret;
            if (skip_closing_tag(s, f->name))
                return -1;
        } else {
            type = get_untagged_type(f->type);
            if (type == ASN1_CTYPE_CHOICE) {
                ret = asn1_xer_decode_type(s, f->type, 
                                           str->buf + (str->len - 1) * 
                                           f->elem_size);
                if (ret)
                    return ret;
            } else {
                ret = asn1_xer_decode_typed_value(s, f->type, 
                                                  str->buf + (str->len - 1) * 
                                                  f->elem_size);
                if (ret)
                    return ret;
            }
        }
    }

    /* we realloc to the exact size to avoid wasting space */
    if (size != str->len) {
        buf = asn1_realloc(str->buf, str->len * f->elem_size);
        if (buf)
            str->buf = buf;
    }
    return 0;
}

static int asn1_xer_decode_choice(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    int nb_fields, flags, i, ret;
    uint32_t data_offset, choice_offset;
    const ASN1ChoiceField *f;
    char *name;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++;
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    
    if (skip_spaces(s))
        return -1;
    if (s->tok.val != TOK_XT_START) {
        return parse_error(s, "choice: expecting open tag");
    }
    name = asn1_strdup(s->tok.u.tag.name);

    f = (const ASN1ChoiceField *)p;
    for(i = 0; i < nb_fields; i++) {
        if (!strcmp(f->name, name))
            break;
        f++;
    }
    if (i == nb_fields) {
        parse_error(s, "choice '%s' not found", name);
        goto fail;
    }
    if (next_token(s))
        goto fail;
    *(uint32_t *)(data + choice_offset) = i;
    ret = asn1_xer_decode_type(s, f->type, data + data_offset);
    if (ret)
        goto fail;
    if (skip_closing_tag(s, name))
        goto fail;
    asn1_free(name);
    return 0;
 fail:
    asn1_free(name);
    return -1;
}

static int asn1_xer_decode_enumerated(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    int flags, nb_fields, val;
    char *name;
    const uint8_t *r, *r_end;
    const char *value;
    
    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    
    if (s->tok.val == TOK_XT_CONTENT) {
        r = s->tok.u.content.buf;
        r_end = r + s->tok.u.content.len;
        while (r < r_end) {
            if (!isspace(*r)) {
                value = (char *)s->tok.u.content.buf;
                name = NULL;
                goto use_content;
            }
            r++;
        }
        if (next_token(s))
            return -1;
    }
    if (s->tok.val != TOK_XT_START) {
        return parse_error(s, "expecting empty tag for enumerated type");
    }
    name = asn1_strdup(s->tok.u.tag.name);
    value = name;
 use_content:
    for(val = 0; val < nb_fields; val++) {
        if (!strcmp((char *)p[val], value))
            break;
    }
    if (val == nb_fields) {
        if (!name) {
            char *r1;
            /* try to parse the content as a number */
            val = strtol(value, &r1, 0);
            if (*r1 != '\0')
                goto not_found;
            val = asn1_find_enum_index(p, nb_fields, val, flags);
            if (val < 0)
                goto not_found;
        } else {
        not_found:
            parse_error(s, "enumerated '%s' not found", value);
            goto fail;
        }
    }
    if (next_token(s))
        goto fail;
    if (name) {
        if (skip_closing_tag(s, name))
            goto fail;
        asn1_free(name);
    }
    *(uint32_t *)data = val;
    return 0;
 fail:
    asn1_free(name);
    return -1;
}

static int asn1_xer_decode_null(ASN1DecodeState *s, const ASN1CType *p, 
                                 uint8_t *data)
{
    return 0;
}

static int asn1_xer_decode_object_identifier(ASN1DecodeState *s, const ASN1CType *p, 
                                              uint8_t *data)
{
    ASN1String *str;
    ASN1ByteBuffer bb;
    int v, val, i, val1, n, j;
    BOOL is_relative;
    const char *r, *r1;

    is_relative = ASN1_GET_CTYPE(p[0]) == ASN1_CTYPE_RELATIVE_OID;
    if (s->tok.val != TOK_XT_CONTENT) {
        return parse_error(s, "expecting %sobject identifier",
                           is_relative ? "relative " : "");
    }

    r = (const char *)s->tok.u.content.buf;

    asn1_byte_buffer_init(&bb);
    j = 0;
    val1 = 0; /* avoids warning */
    for(;;) {
        while (isspace(*r))
            r++;
        val = strtoul(r, (char **)&r1, 10);
        if (r1 == r) {
            return parse_error(s, "object identifier: expecting positive integer");
        }
        r = r1;
        if (!is_relative && j == 0) {
            if (val > 2)
                return parse_error(s, "object identifier: integer too large");
            val1 = val;
        } else {
            if (!is_relative && j == 1) {
                if (val > 39)
                    return parse_error(s, "object identifier: integer too large");
                val = val1 * 40 + val;
            } 

            v = val;
            n = 0;
            do {
                n++;
                v >>= 7;
            } while (v != 0);
            for(i = n - 1; i >= 1; i--) {
                asn1_put_byte(&bb, ((val >> (i * 7)) & 0x7f) | 0x80);
            }
            asn1_put_byte(&bb, val & 0x7f);
        }
        while (isspace(*r))
            r++;
        if (*r == '\0')
            break;
        if (*r != '.') {
            return parse_error(s, "object identifier: expecting '.'");
        }
        r++;
        j++;
    }
    if (bb.has_error) {
        asn1_free(bb.buf);
        return mem_error(s);
    }
    str = (ASN1String *)data;
    str->len = bb.len;
    str->buf = bb.buf;
    return next_token(s);
}

static int asn1_xer_decode_real(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    uint64_t mant;
    int e, sg;
    const char *r;
    double d;

    if (s->tok.val != TOK_XT_CONTENT) {
        return parse_error(s, "expecting real number");
    }
    r = (const char *)s->tok.u.content.buf;
    while (isspace(*r))
        r++;
    if (!strcmp(r, "INF")) {
        e = 0x7ff;
        mant = 0;
        sg = 0;
        goto special;
    } else if (!strcmp(r, "-INF")) {
        e = 0x7ff;
        mant = 0;
        sg = 1;
        goto special;
    } else if (!strcmp(r, "NaN")) {
        e = 0x7ff;
        mant = (uint64_t)1 << 51;
        sg = 0;
    special:
        *(uint64_t *)data = ((uint64_t)sg << 63) | ((uint64_t)e << 52) | mant;
    } else {
        d = strtod(r, NULL);
        *(double *)data = d;
    }
    return next_token(s);
}

typedef struct {
    int val;
    const char *tag_name;
} XMLStringTag;

static const XMLStringTag xml_string_tags[] = {
 {0    , "nul" },
 {1    , "soh" },
 {2    , "stx" },
 {3    , "etx" },
 {4    , "eot" },
 {5    , "enq" },
 {6    , "ack" },
 {7    , "bel" },
 {8    , "bs" },
 {11   , "vt" },
 {12   , "ff" },
 {14   , "so" },
 {15   , "si" },
 {16  , "dle" },
 {17  , "dc1" },
 {18  , "dc2" },
 {19  , "dc3" },
 {20  , "dc4" },
 {21  , "nak" },
 {22  , "syn" },
 {23  , "etb" },
 {24  , "can" },
 {25  , "em" },
 {26  , "sub" },
 {27  , "esc" },
 {28  , "is4" },
 {29  , "is3" },
 {30  , "is2" },
 {31  , "is1" },
};

static int asn1_xer_decode_char_string(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    ASN1String *str;
    const uint8_t *r, *r_end;
    int char_string_type, shift;
    int c;
    size_t len, size;
    
    p++;
    char_string_type = *p++;

    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;

    str = (ASN1String *)data;
    size = 0;
    len = 0;
    str->buf = NULL;
    for(;;) {
        if (s->tok.val == TOK_XT_CONTENT) {
            r = s->tok.u.content.buf;
            r_end = r + s->tok.u.content.len;
            
            while (r < r_end) {
                if (char_string_type == ASN1_CSTR_UTF8String) {
                    c = *r++;
                } else {
                    c = asn1_from_utf8(r, r_end - r, &r);
                    if (c < 0) {
                        return parse_error(s, "invalid UTF-8 code");
                    }
                }
                if (asn1_realloc_buf(&str->buf, 1 << shift, &size, len + 1))
                    return mem_error(s);
                if (shift == 0)
                    ((uint8_t *)str->buf)[len] = c;
                else if (shift == 1) 
                    ((uint16_t *)str->buf)[len] = c;
                else
                    ((uint32_t *)str->buf)[len] = c;
                len++;
            }
            if (next_token(s))
                return -1;
        } else if (s->tok.val == TOK_XT_START) {
            char *name;
            int i;
            
            name = s->tok.u.tag.name;
            /* search code name */
            for(i = 0; i < countof(xml_string_tags); i++) {
                if (!strcmp(name, xml_string_tags[i].tag_name))
                    break;
            }
            if (i == countof(xml_string_tags))
                return parse_error(s, "unexpected tag <%s> in character string", name);
            name = asn1_strdup(name);
            if (next_token(s))
                return -1;
            if (skip_closing_tag(s, name)) {
                asn1_free(name);
                return -1;
            }
            asn1_free(name);
            
            c = xml_string_tags[i].val;
            if (asn1_realloc_buf(&str->buf, 1 << shift, &size, len + 1))
                return mem_error(s);
            if (shift == 0)
                ((uint8_t *)str->buf)[len] = c;
            else if (shift == 1) 
                ((uint16_t *)str->buf)[len] = c;
            else
                ((uint32_t *)str->buf)[len] = c;
            len++;
        } else {
            break;
        }
    }
    if (len < size) {
        str->buf = asn1_realloc2(str->buf, len, 1 << shift);
    }
    str->len = len;
    return 0;
}

static int asn1_xer_decode_any(ASN1DecodeState *s, const ASN1CType *p, 
                               uint8_t *data)
{
    int ret;
    ASN1OpenType *str;

    str = (ASN1OpenType *)data;
    str->type = (ASN1CType *)asn1_get_constrained_type(s->top_value, p);
    if (!str->type) {
        if (skip_spaces(s))
            return -1;
        if (s->tok.val != TOK_XT_START || 
            strcmp(s->tok.u.tag.name, "OCTET_STRING") != 0) {
            return parse_error(s, "expecting open type");
        }
        if (next_token(s))
            return -1;
        /* Note: we code it as an octet string */
        ret = asn1_xer_decode_octet_string(s, p, 
                                           (uint8_t *)&str->u.octet_string);
        if (ret)
            return ret;
        return skip_closing_tag(s, "OCTET_STRING");
    } else {
        str->u.data = asn1_mallocz_value(str->type);
        if (!str->u.data)
            return mem_error(s);
        return asn1_xer_decode_typed_value(s, str->type, str->u.data);
    }
}

static int asn1_xer_decode_tagged(ASN1DecodeState *s, const ASN1CType *p, 
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
    return asn1_xer_decode_type(s, (ASN1CType *)p[1], data1);
}

static int asn1_xer_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
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
        ret = asn1_xer_decode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_xer_decode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_xer_decode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_xer_decode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_xer_decode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_xer_decode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_xer_decode_null(s, p, data);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_xer_decode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_xer_decode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_xer_decode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_xer_decode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_xer_decode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_xer_decode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_xer_decode_any(s, p, data);
        break;
    default:
        ret = parse_error(s, "unsupported type (%d)", type);
    }
    return ret;
}

static int asn1_xer_decode_typed_value(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    const char *type_name;
    char *tag_name;
    int ret;

    if (skip_spaces(s))
        return -1;
    if (s->tok.val != TOK_XT_START)
        return parse_error(s, "expecting starting tag");
    tag_name = asn1_strdup(s->tok.u.tag.name);
    type_name = asn1_get_type_name(p);
    if (!strcmp(type_name, tag_name)) {
        /* OK */
    } else {
        /* XXX: also accept generic type name ? */
        ret = parse_error(s, "expecting tag <%s>", type_name);
    fail:
        asn1_free(tag_name);
        return -1;
    }
    if (next_token(s))
        goto fail;
    ret = asn1_xer_decode_type(s, p, data);
    if (ret) 
        goto fail;
    if (skip_closing_tag(s, tag_name))
        goto fail;
    asn1_free(tag_name);
    return 0;
}

/* warning: buf must be zero terminated i.e. buf[buf_len] = '\0'. */
asn1_ssize_t asn1_xer_decode(void **pdata, const ASN1CType *p, const uint8_t *buf, 
                             size_t buf_len, ASN1Error *err)
{
    ASN1DecodeState s_s, *s = &s_s;
    void *data;
    int ret;

    /* set dummy error */
    memset(s, 0, sizeof(*s));
    s->error.line_num = 1;
    s->error.msg[0] = '\0';

    s->buf_ptr = buf;
    s->buf_end = buf + buf_len;
    s->line_num = 1;

    s->empty_tag = FALSE;
    s->tok.val = TOK_EOF; /* set for free_token() */
    s->top_value = NULL;

    data = asn1_mallocz_value(p);
    if (!data) {
        mem_error(s);
        goto fail;
    }

    if (next_token(s)) {
        ret = -1;
    } else {
        ret = asn1_xer_decode_typed_value(s, p, data);
    }
    free_token(&s->tok);
    if (ret) {
        asn1_free_value(p, data);
    fail:
        if (err)
            *err = s->error;
        *pdata = NULL;
        return -1;
    } else {
        *pdata = data;
        return buf_len;
    }
}
