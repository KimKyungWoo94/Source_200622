/* 
 * ASN1 GSER (text) decoder
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

//#define DUMP_PARSE
//#define DEBUG

enum {
    TOK_EOF = 128,
    TOK_ERROR,
    TOK_CINT, /* constant integer (64 bits) */
    TOK_CREAL,
    TOK_CSTR, /* constant string */
    TOK_CBIN_STR, /* constant binary string */
    TOK_CHEX_STR, /* constant hexadecimal string */
    TOK_IDENT,

    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL,
};

#define ASN1_IDENTIFIER_MAX_LEN 256

typedef struct {
    int val;
    union {
        ASN1String cstr;
        ASN1UniversalString custr;
#ifdef ASN1_USE_LARGE_INTEGER
        ASN1Integer cint;
#else
        int64_t cint;
#endif
        double creal;
        char ident[ASN1_IDENTIFIER_MAX_LEN + 1];
    } u;
} ASN1Token;

typedef struct ASN1DecodeState {
    const uint8_t *buf_ptr;
    const uint8_t *buf_end;

    int line_num;

    ASN1Token tok;
    ASN1ValueStack *top_value;
    
    ASN1Error error;
} ASN1DecodeState;

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

/* only works for simple tokens */
static char *get_token_str(char *buf, int buf_size, int val)
{
    if (val < 128) {
        snprintf(buf, buf_size, "%c", val);
    } else {
        switch(val) {
        case TOK_EOF:
            asn1_pstrcpy(buf, buf_size, "EOF");
            break;
        case TOK_TRUE:
            asn1_pstrcpy(buf, buf_size, "TRUE");
            break;
        case TOK_FALSE:
            asn1_pstrcpy(buf, buf_size, "FALSE");
            break;
        case TOK_NULL:
            asn1_pstrcpy(buf, buf_size, "NULL");
            break;
        default:
            snprintf(buf, buf_size, "[unknown token %d]", val);
            break;
        }
    }
    return buf;
}

#ifdef DUMP_PARSE
static __attribute__((unused)) void dump_token(FILE *f, const ASN1Token *tok)
{
    char buf[256];

    switch(tok->val) {
    case TOK_CSTR:
        {
            int c, i;
            fputc('\"', f);
            for(i = 0; i < tok->u.cstr.len; i++) {
                c = tok->u.custr.buf[i];
                if (c >= 32 && c <= 126)
                    fputc(c & 0xff, f);
                else
                    fputc('.', f);
                if (c == '\"')
                    fputc('\"', f);
            }
            fputc('\"', f);
        }
        break;
    case TOK_CBIN_STR:
        {
            int c, i;
            fputc('\'', f);
            for(i = 0; i < tok->u.cstr.len; i++) {
                c = get_bit(tok->u.cstr.buf, i);
                fputc(c + '0', f);
            }
            fputc('\'', f);
            fputc('B', f);
        }
        break;
    case TOK_CHEX_STR:
        {
            int c, i, len1;
            fputc('\'', f);
            len1 = tok->u.cstr.len / 4;
            for(i = 0; i < len1; i++) {
                c = (tok->u.cstr.buf[i >> 1] >> (4 - 4 * (i & 1))) & 0xf;
                if (c >= 10)
                    fputc(c - 10 + 'A', f);
                else
                    fputc(c + '0', f);
            }
            fputc('\'', f);
            fputc('H', f);
        }
        break;
    case TOK_CINT:
        {
            char *p, *buf;
            p = asn1_integer_to_str(&tok->u.cint, &buf, NULL);
            fputs(p, f);
            asn1_free(buf);
        }
        break;
    case TOK_IDENT:
        fprintf(f, "%s", tok->u.ident);
        break;
    default:
        get_token_str(buf, sizeof(buf), tok->val);
        fprintf(f, "%s", buf);
        break;
    }
}
#endif

static inline void free_token(ASN1Token *tok)
{
    switch(tok->val) {
    case TOK_CSTR:
    case TOK_CBIN_STR:
    case TOK_CHEX_STR:
        asn1_free(tok->u.cstr.buf);
        tok->u.cstr.buf = NULL; /* fail safe */
        tok->val = TOK_EOF;
        break;
    case TOK_CINT:
#ifdef ASN1_USE_LARGE_INTEGER
        asn1_integer_clear(&tok->u.cint);
#endif
        tok->val = TOK_EOF;
        break;
    }
}

static inline int is_num(int c)
{
    return c >= '0' && c <= '9';
}

static int next_token(ASN1DecodeState *s)
{
    const uint8_t *p;
    int c;

    free_token(&s->tok);
    
    p = s->buf_ptr;
 redo:
    if (p >= s->buf_end) 
        goto eof;
    c = *p;
    switch(c) {
    case '\n':
        p++;
        s->line_num++;
#ifdef DUMP_PARSE
        printf("\n");
#endif
        goto redo;
    case ' ':
    case '\t':
    case '\f':
    case '\v':
    case '\r':
        p++;
        goto redo;

    case '{':
    case '}':
    case ',':
    case '(':
    case ')':
    case ';':
    case ':':
    case '.':
    simple_token:
        p++;
        s->tok.val = c;
        break;
    case '-':
        if ((p + 1) < s->buf_end && p[1] == '-') {
            /* single line comment */
            p += 2;
            while (p < s->buf_end && *p != '\n')
                p++;
            goto redo;
        } else {
            goto simple_token;
        }
        break;
    case '/':
        if ((p + 1) < s->buf_end && p[1] == '*') {
            p += 2;
            for(;;) {
                if (p >= s->buf_end) {
                    parse_error(s, "expecting end of multi-line comment");
                    goto fail;
                }
                c = *p;
                if (c == '\n') {
                    s->line_num++;
                } else if (c == '*' && (p + 1) < s->buf_end && p[1] == '/') {
                    p += 2;
                    break;
                }
                p++;
            }
            goto redo;
        } else {
            goto unexpected_char;
        }
        break;
    case '\'':
        {
            int len, i, byte_len;
            uint8_t *buf1;
            const uint8_t *p1;

            /* binary or hexa string */
            p++;
            p1 = p;
            len = 0;
            for(;;) {
                if (p >= s->buf_end)
                    goto bit_string_end_error;
                c = *p;
                if (c == '\n') {
                bit_string_end_error:
                    parse_error(s, "unexpected end of binary or hexadecimal string");
                    goto fail;
                }
                if (c == '\'')
                    break;
                if (!isspace(c)) {
                    len++;
                }
                p++;
            }
            p++;
            if (p >= s->buf_end)
                goto bit_string_end_error;
            if (*p == 'H') {
                /* check hex digits */
                byte_len = (len + 1) / 2;
                buf1 = asn1_mallocz(byte_len);
                if (!buf1) {
                    mem_error(s);
                    goto fail;
                }
                for(i = 0; i < len; i++) {
                    do {
                        c = *p1++;
                    } while (isspace(c));
                    if (c >= '0' && c <= '9') {
                        c -= '0';
                    } else if (c >= 'A' && c <= 'F') {
                        c = c - 'A' + 10;
                    } else {
                        parse_error(s, "expecting hexadecimal digits");
                        goto fail;
                    }
                    buf1[i >> 1] |= c << (4 - 4 * (i & 1));
                }
                s->tok.val = TOK_CHEX_STR;
                len *= 4;
            } else if (*p == 'B') {
                /* check binary digits */
                byte_len = (len + 7) / 8;
                buf1 = asn1_mallocz(byte_len);
                if (!buf1) {
                    mem_error(s);
                    goto fail;
                }
                for(i = 0; i < len; i++) {
                    do {
                        c = *p1++;
                    } while (isspace(c));
                    if (c >= '0' && c <= '1') {
                        c -= '0';
                    } else {
                        parse_error(s, "expecting binary digits");
                        goto fail;
                    }
                    put_bit(buf1, i, c);
                }
                s->tok.val = TOK_CBIN_STR;
            } else {
                parse_error(s, "expecting 'B' or 'H'");
                goto fail;
            }
            p++;
            s->tok.u.cstr.buf = buf1;
            s->tok.u.cstr.len = len;
        }
        break;
    case '\"':
        {
            uint32_t *buf;
            size_t len, size;
            p++;
            len = 0;
            size = 0;
            buf = NULL;
            for(;;) {
                if (p >= s->buf_end) {
                    parse_error(s, "unterminated string");
                    goto fail;
                }
                c = asn1_from_utf8(p, s->buf_end -  p, &p);
                if (c < 0) {
                    parse_error(s, "invalid UTF-8 code");
                    goto fail;
                }
                if (c == '\"') {
                    if (p < s->buf_end && *p == '\"') {
                        p++;
                    } else {
                        break;
                    }
                }
                if (asn1_realloc_buf((uint8_t **)&buf, sizeof(uint32_t), 
                                     &size, len + 1)) {
                    return mem_error(s);
                }
                buf[len++] = c;
            }
            s->tok.u.custr.buf = buf;
            s->tok.u.custr.len = len;
            s->tok.val = TOK_CSTR;
        }
        break;
    case '0' ... '9':
        /* number */
        {
            ASN1ByteBuffer bb;
            char *r;
            const uint8_t *p1;

            asn1_byte_buffer_init(&bb);
            asn1_put_byte(&bb, c);
            p++;
            for(;;) {
                if (p >= s->buf_end) {
                    c = '\0';
                    break;
                }
                c = *p;
                if (!is_num(c))
                    break;
                p++;
                asn1_put_byte(&bb, c);
            }
            p1 = p;
            /* Note: we expect an explicit exponent for real numbers
               to differentiate them from dotted object identifiers */
            /* real number */
            if (c == '.') {
                asn1_put_byte(&bb, c);
                p++;
                for(;;) {
                    if (p >= s->buf_end) {
                        c = '\0';
                        break;
                    }
                    c = *p;
                    if (!is_num(c))
                        break;
                    p++;
                    asn1_put_byte(&bb, c);
                }
            }
            if (c == 'E') {
                asn1_put_byte(&bb, c);
                p++;
                if (p >= s->buf_end)
                    c = '\0';
                else
                    c = *p;
                if (c == '-' || c == '+') {
                    asn1_put_byte(&bb, c);
                    p++;
                    if (p >= s->buf_end)
                        c = '\0';
                    else
                        c = *p;
                }
                while (is_num(c)) {
                    asn1_put_byte(&bb, c);
                    p++;
                    if (p >= s->buf_end)
                        c = '\0';
                    else
                        c = *p;
                }
                asn1_put_byte(&bb, '\0');
                if (bb.has_error) {
                    asn1_free(bb.buf);
                    mem_error(s);
                    goto fail;
                }
                s->tok.val = TOK_CREAL;
                s->tok.u.creal = strtod((char *)bb.buf, &r);
            } else {
                asn1_put_byte(&bb, '\0');
                if (bb.has_error) {
                    asn1_free(bb.buf);
                    mem_error(s);
                    goto fail;
                }
                p = p1;
                s->tok.val = TOK_CINT;
                r = (char *)bb.buf;
#ifdef ASN1_USE_LARGE_INTEGER
                asn1_integer_init(&s->tok.u.cint);
                if (asn1_integer_from_str(&s->tok.u.cint, &r)) {
                    asn1_free(bb.buf);
                    mem_error(s);
                    goto fail;
                }
#else
                s->tok.u.cint = strtoul(r, &r, 10);
                if (*r != '\0') {
                    asn1_free(bb.buf);
                    parse_error(s, "integer overflow");
                    goto fail;
                }
#endif
            }
            asn1_free(bb.buf);
        }
        break;
    case 'A' ... 'Z':
    case 'a' ... 'z':
        {
            char *q, *buf;

            /* identifier or keyword */
            buf = s->tok.u.ident;
            q = buf;
            *q++ = c;
            p++;
            for(;;) {
                if (p >= s->buf_end)
                    break;
                c = *p;
                if ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'Z') ||
                    (c >= 'a' && c <= 'z') ||
                    (c == '-')) {
                    if ((q - buf) >= ASN1_IDENTIFIER_MAX_LEN) {
                        parse_error(s, "identifier too long");
                        goto fail;
                    }
                    *q++ = c;
                    p++;
                } else {
                    break;
                }
            }
            *q = '\0';
            if (!strcmp(buf, "TRUE")) {
                s->tok.val = TOK_TRUE;
            } else if (!strcmp(buf, "FALSE")) {
                s->tok.val = TOK_FALSE;
            } else if (!strcmp(buf, "NULL")) {
                s->tok.val = TOK_NULL;
            } else {
                s->tok.val = TOK_IDENT;
            }
        }
        break;
    default:
    unexpected_char:
        parse_error(s, "unexpected character '%c'", c);
    fail:
        s->tok.val = TOK_ERROR;
        s->buf_ptr = p;
        return -1;
    }
    s->buf_ptr = p;
#ifdef DUMP_PARSE
    dump_token(stdout, &s->tok);
    printf(" ");
#endif
    return 0;
 eof:
    s->tok.val = TOK_EOF;
    s->buf_ptr = p;
    return 0;
}

static int skip_token(ASN1DecodeState *s, int val)
{
    char buf[256];
    if (s->tok.val != val)
        return parse_error(s, "expecting '%s'", get_token_str(buf, sizeof(buf), val));
    if (next_token(s))
        return -1;
    return 0;
}

static int asn1_gser_decode_boolean(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    uint32_t val;

    if (s->tok.val == TOK_TRUE) {
        if (next_token(s))
            return -1;
        val = 1;
    } else if (s->tok.val == TOK_FALSE) {
        if (next_token(s))
            return -1;
        val = 0;
    } else {
        return parse_error(s, "expecting boolean value");
    }
    *(BOOL *)data = val;
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
static int asn1_gser_decode_large_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                          uint8_t *data, int sg)
{
    ASN1Integer *val;

    val = (ASN1Integer *)data;
    asn1_integer_init(val);
    if (asn1_integer_set(val, &s->tok.u.cint)) {
        mem_error(s);
        return -1;
    }
    val->negative = sg;
    if (next_token(s))
        return -1;
    return 0;
}
#endif

static int asn1_gser_decode_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                    uint8_t *data)
{
    int flags, sg;
    int64_t val;

    sg = 0;
    if (s->tok.val == '-') {
        sg = 1;
        if (next_token(s))
            return -1;
    }
    if (s->tok.val != TOK_CINT) 
        return parse_error(s, "expecting integer value");
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_gser_decode_large_integer(s, p, data, sg);
#else
        return -1;
#endif
    }

#ifdef ASN1_USE_LARGE_INTEGER
    if (asn1_integer_get_si64_ov(&s->tok.u.cint, &val))
        goto overflow;
#else
    val = s->tok.u.cint;
#endif
    if (next_token(s))
        return -1;
    if (sg)
        val = -val;
    if (asn1_is_uint32(p)) {
        if (val != (uint32_t)val)
            goto overflow;
    } else {
        if (val != (int32_t)val) {
        overflow:
            return parse_error(s, "integer too large");
        }
    }
    *(int *)data = val;
    return 0;
}

static int asn1_gser_decode_bit_string(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    uint32_t byte_len;
    ASN1BitString *str;
    const ASN1String *tok_str;

    str = (ASN1BitString *)data;
    if (s->tok.val == TOK_CBIN_STR ||
        s->tok.val == TOK_CHEX_STR) {
        tok_str = &s->tok.u.cstr;
        str->len = tok_str->len;
        byte_len = (tok_str->len + 7) / 8;
        str->buf = asn1_malloc(byte_len);
        if (!str->buf)
            return mem_error(s);
        memcpy(str->buf, tok_str->buf, byte_len);
        if (next_token(s))
            return -1;
    } else {
        /* Note: could accept integers too as an extension if fixed
           length bit string */
        return parse_error(s, "expecting bit string value");
    }
    return 0;
}

static int asn1_gser_decode_octet_string(ASN1DecodeState *s, const ASN1CType *p, 
                                         uint8_t *data)
{
    uint32_t byte_len;
    ASN1String *str, *tok_str;

    str = (ASN1String *)data;
    if (s->tok.val == TOK_CBIN_STR ||
        s->tok.val == TOK_CHEX_STR) {
        tok_str = &s->tok.u.cstr;
        if ((tok_str->len % 8) != 0)
            return parse_error(s, "number of bits must be multiple of 8");
        byte_len = tok_str->len / 8;
    } else if (s->tok.val == TOK_CSTR) {
        tok_str = &s->tok.u.cstr;
        byte_len = tok_str->len;
    } else {
        return parse_error(s, "expecting octet string value");
    }

    str->len = byte_len;
    str->buf = asn1_malloc(byte_len);
    if (!str->buf)
        return mem_error(s);
    memcpy(str->buf, tok_str->buf, byte_len);

    if (next_token(s))
        return -1;
    return 0;
}

static int asn1_gser_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data);

static int asn1_gser_decode_sequence(ASN1DecodeState *s, const ASN1CType *p1, 
                                     uint8_t *data)
{
    int nb_fields, ret, idx;
    const ASN1SequenceField *f, *f1;
    int flag;
    uint8_t *table_present;
    char *field_name;
    const ASN1CType *p;
    ASN1ValueStack value_entry;

    if (skip_token(s, '{'))
        return -1;

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
    
    if (s->tok.val != '}') {
        for(;;) {
            if (s->tok.val != TOK_IDENT) {
                parse_error(s, "expecting sequence/set component name");
                goto fail;
            }
            field_name = s->tok.u.ident;
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
                return -1;

            ret = asn1_gser_decode_type(s, f->type, 
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

            if (s->tok.val == ',') {
                if (next_token(s))
                    return -1;
            } else if (s->tok.val == '}') {
                break;
            } else {
                parse_error(s, "expecting ',' or '}'");
                goto fail;
            }
        }
    }
    if (skip_token(s, '}'))
        goto fail;

    if (asn1_sequence_set_default_fields(f1, nb_fields, data, table_present)) {
        mem_error(s);
        goto fail;
    }

    if (asn1_sequence_check_fields(f1, nb_fields, table_present, 
                                   s->error.msg, sizeof(s->error.msg))) {
        ret = parse_error_internal(s);
        goto fail;
    }
    asn1_free(table_present);
    s->top_value = s->top_value->prev;
    return 0;
 fail:
    asn1_free(table_present);
    return -1;
}

static int asn1_gser_decode_sequence_of(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    int ret, flags;
    size_t size;
    ASN1String *str;
    uint8_t *buf;

    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    str = (ASN1String *)data;
    str->len = 0;
    str->buf = NULL;
    size = 0;

    if (skip_token(s, '{'))
        return -1;
    if (s->tok.val != '}') {
        for(;;) {
            if ((str->len + 1) > size) {
                if (asn1_reallocz_buf(&str->buf, f->elem_size, &size, str->len + 1))
                    return mem_error(s);
            }
            str->len++;
            ret = asn1_gser_decode_type(s, f->type, 
                                        str->buf + (str->len - 1) * 
                                        f->elem_size);
            if (ret)
                return ret;
            if (s->tok.val == ',') {
                if (next_token(s))
                    return -1;
            } else if (s->tok.val == '}') {
                break;
            } else {
                return parse_error(s, "expecting ',' or '}'");
            }
        }
    }

    /* we realloc to the exact size to avoid wasting space */
    if (size != str->len) {
        buf = asn1_realloc2(str->buf, str->len, f->elem_size);
        if (buf)
            str->buf = buf;
    }

    if (skip_token(s, '}'))
        return -1;
    return 0;
}

static int asn1_gser_decode_choice(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    int nb_fields, flags, i;
    uint32_t data_offset, choice_offset;
    const ASN1ChoiceField *f;
    const char *name;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++;
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    
    if (s->tok.val != TOK_IDENT) {
        return parse_error(s, "expecting choice identifier");
    }
    name = s->tok.u.ident;

    f = (const ASN1ChoiceField *)p;
    for(i = 0; i < nb_fields; i++) {
        if (!strcmp(f->name, name))
            break;
        f++;
    }
    if (i == nb_fields) {
        return parse_error(s, "choice '%s' not found", name);
    }
    *(uint32_t *)(data + choice_offset) = i;
    if (next_token(s))
        return -1;
    if (skip_token(s, ':'))
        return -1;
    return asn1_gser_decode_type(s, f->type, data + data_offset);
}

static int asn1_gser_decode_enumerated(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    int flags, nb_fields, i;
    const char *name;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    
    if (s->tok.val != TOK_IDENT) {
        return parse_error(s, "expecting enumerated identifier");
    }
    name = s->tok.u.ident;
    for(i = 0; i < nb_fields; i++) {
        if (!strcmp((char *)p[i], name))
            break;
    }
    if (i == nb_fields)
        return parse_error(s, "enumerated '%s' not found", name);
    if (next_token(s))
        return -1;
    *(uint32_t *)data = i;
    return 0;
}

static int asn1_gser_decode_null(ASN1DecodeState *s, const ASN1CType *p, 
                                 uint8_t *data)
{
    return skip_token(s, TOK_NULL);
}

static int asn1_gser_decode_object_identifier(ASN1DecodeState *s, const ASN1CType *p, 
                                              uint8_t *data)
{
    ASN1String *str;
    ASN1ByteBuffer bb;
    int v, val, i, val1, n, j;
    BOOL is_relative;

    is_relative = ASN1_GET_CTYPE(p[0]) == ASN1_CTYPE_RELATIVE_OID;
    asn1_byte_buffer_init(&bb);
    j = 0;
    val1 = 0; /* avoids warning */
    for(;;) {
        if (s->tok.val != TOK_CINT)
            return parse_error(s, "object identifier: expecting positive integer");
#ifdef ASN1_USE_LARGE_INTEGER
        if (asn1_integer_get_si_ov(&s->tok.u.cint, &val))
            return parse_error(s, "object identifier: integer too large");
#else
        val = s->tok.u.cint;
#endif
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
        if (next_token(s))
            return -1;
        if (s->tok.val != '.')
            break;
        if (next_token(s))
            return -1;
        j++;
    }
    if (bb.has_error) {
        asn1_free(bb.buf);
        return mem_error(s);
    }
    
    str = (ASN1String *)data;
    str->len = bb.len;
    str->buf = bb.buf;
    return 0;
}

static int asn1_gser_decode_real(ASN1DecodeState *s, const ASN1CType *p, 
                                 uint8_t *data)
{
    uint64_t mant;
    int e, sg;
    if (s->tok.val == TOK_IDENT) {
        if (!strcmp(s->tok.u.ident, "PLUS-INFINITY")) {
            e = 0x7ff;
            mant = 0;
            sg = 0;
        } else if (!strcmp(s->tok.u.ident, "MINUS-INFINITY")) {
            e = 0x7ff;
            mant = 0;
            sg = 1;
        } else if (!strcmp(s->tok.u.ident, "NOT-A-NUMBER")) {
            e = 0x7ff;
            mant = (uint64_t)1 << 51;
            sg = 0;
        } else {
            goto fail;
        }
        if (next_token(s))
            return -1;
        *(uint64_t *)data = ((uint64_t)sg << 63) | ((uint64_t)e << 52) | mant;
    } else {
        sg = 1;
        if (s->tok.val == '-') {
            if (next_token(s))
                return -1;
            sg = -1;
        }
        /* XXX: test minus zero */
        if (s->tok.val == TOK_CREAL) {
            *(double *)data = s->tok.u.creal * sg;
        } else if (s->tok.val == TOK_CINT) {
            int64_t val;
#ifdef ASN1_USE_LARGE_INTEGER
            if (asn1_integer_get_si64_ov(&s->tok.u.cint, &val))
                goto fail;
#else
            val = s->tok.u.cint;
#endif
            *(double *)data = (double)val * sg;
        } else {
        fail:
            return parse_error(s, "expecting real number");
        }
        if (next_token(s))
            return -1;
    }
    return 0;
}

static int asn1_gser_decode_char_string(ASN1DecodeState *s, const ASN1CType *p, 
                                        uint8_t *data)
{
    ASN1String *str;
    uint32_t c, len, i, j;
    int char_string_type, shift;
    const uint32_t *tok_buf;

    if (s->tok.val != TOK_CSTR) {
        return parse_error(s, "expecting character string");
    }
    tok_buf = s->tok.u.custr.buf;
    len = s->tok.u.custr.len;
    str = (ASN1String *)data;
    
    p++;
    char_string_type = *p++;

    if (char_string_type <= ASN1_CSTR_UniversalString) {
        if (char_string_type == ASN1_CSTR_BMPString)
            shift = 1;
        else if (char_string_type == ASN1_CSTR_UniversalString)
            shift = 2;
        else
            shift = 0;
        
        str->buf = asn1_malloc2(len, 1 << shift);
        if (!str->buf) 
            return mem_error(s);
        str->len = len;
        for(i = 0; i < len; i++) {
            c = tok_buf[i];
            if (shift == 0)
                ((uint8_t *)str->buf)[i] = c;
            else if (shift == 1) 
                ((uint16_t *)str->buf)[i] = c;
            else
                ((uint32_t *)str->buf)[i] = c;
        }
    } else if (char_string_type == ASN1_CSTR_UTF8String) {
        ASN1ByteBuffer bb;
        uint8_t buf[ASN1_UTF8_MAX_LEN];
        int l, i;

        asn1_byte_buffer_init(&bb);
        for(i = 0; i < len; i++) {
            l = asn1_to_utf8(buf, tok_buf[i]);
            for(j = 0; j < l; j++)
                asn1_put_byte(&bb, buf[j]);
        }
        if (bb.has_error) {
            asn1_free(bb.buf);
            return mem_error(s);
        }
        str->len = bb.len;
        str->buf = bb.buf;
    } else {
        str->buf = asn1_malloc(len);
        if (!str->buf)
            return mem_error(s);
        str->len = len;
        for(i = 0; i < len; i++)
            str->buf[i] = tok_buf[i];
    }
    if (next_token(s))
        return -1;
    return 0;
}

static int asn1_gser_decode_any(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    uint32_t byte_len;
    ASN1String *tok_str;
    ASN1OpenType *str;
    
    str = (ASN1OpenType *)data;
    str->type = (ASN1CType *)asn1_get_constrained_type(s->top_value, p);
    if (!str->type) {
        if (s->tok.val != TOK_CHEX_STR)
            return parse_error(s, "any: expecting hex string");
        tok_str = &s->tok.u.cstr;
        if ((tok_str->len % 8) != 0)
            return parse_error(s, "number of bits must be multiple of 8");
        byte_len = tok_str->len / 8;
        
        str->u.octet_string.len = byte_len;
        str->u.octet_string.buf = asn1_malloc(byte_len);
        if (!str->u.octet_string.buf)
            return mem_error(s);
        memcpy(str->u.octet_string.buf, tok_str->buf, byte_len);

        if (next_token(s))
            return -1;
        return 0;
    } else {
        str->u.data = asn1_mallocz_value(str->type);
        if (!str->u.data)
            return mem_error(s);
        return asn1_gser_decode_type(s, str->type, str->u.data);
    }
}

static int asn1_gser_decode_tagged(ASN1DecodeState *s, const ASN1CType *p, 
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
    return asn1_gser_decode_type(s, (ASN1CType *)p[1], data1);
}

static int asn1_gser_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
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
        ret = asn1_gser_decode_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_gser_decode_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_gser_decode_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_gser_decode_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_gser_decode_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_gser_decode_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_gser_decode_null(s, p, data);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_gser_decode_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_gser_decode_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_gser_decode_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_gser_decode_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_gser_decode_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_gser_decode_char_string(s, p, data);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_gser_decode_any(s, p, data);
        break;
    default:
        ret = parse_error(s, "unsupported type (%d)", type);
    }
    return ret;
}

asn1_ssize_t asn1_gser_decode(void **pdata, const ASN1CType *p, const uint8_t *buf, 
                          size_t buf_len, ASN1Error *err)
{
    ASN1DecodeState s_s, *s = &s_s;
    void *data;
    int ret;

    /* set dummy error */
    s->error.line_num = 1;
    s->error.msg[0] = '\0';

    s->buf_ptr = buf;
    s->buf_end = buf + buf_len;
    s->line_num = 1;
    s->top_value = NULL;

    s->tok.val = TOK_EOF; /* set for free_token() */

    data = asn1_mallocz_value(p);
    if (!data) {
        mem_error(s);
        goto fail;
    }

    if (next_token(s)) {
        ret = -1;
    } else {
        ret = asn1_gser_decode_type(s, p, data);
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
        /* XXX: compute position */
        return buf_len;
    }
}
