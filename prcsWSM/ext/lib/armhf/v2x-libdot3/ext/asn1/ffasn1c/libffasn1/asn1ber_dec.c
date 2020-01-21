/* 
 * ASN1 BER decoder
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

#define INDEFINITE_LEN (-2)
#define TAG_EOC (-2)

typedef struct ASN1DecodeState {
    const uint8_t *buf;
    size_t buf_len;
    size_t buf_index;
    int tag_index; /* only used to decode the ANY type */
    BOOL indefinite_len_state;
    BOOL got_eoc;
    ASN1ValueStack *top_value;
    ASN1Error error;
} ASN1DecodeState;

typedef struct {
    int buf_len;
    BOOL indefinite_len_state;
} ASN1ParserState;

static int asn1_ber_decode_type_tag(ASN1DecodeState *s, const ASN1CType *p, 
                                    uint8_t *data, int tag, 
                                    int is_constructed, int len);
static int asn1_ber_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
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

static int asn1_error_primitive(ASN1DecodeState *s)
{
    return asn1_error(s, "expecting primitive encoding");
}

static int asn1_error_constructed(ASN1DecodeState *s)
{
    return asn1_error(s, "expecting constructed encoding");
}

/* return -1 if end of stream */
static int get_byte(ASN1DecodeState *s)
{
    if (unlikely(s->buf_index >= s->buf_len))
        return asn1_error(s, "unexpected end of stream or tag");
    else
        return s->buf[s->buf_index++];
}

static int get_bytes(ASN1DecodeState *s, uint8_t *buf, int len)
{
    if (unlikely((s->buf_index + len) > s->buf_len)) {
        return asn1_error(s, "unexpected end of stream or tag");
    }
    memcpy(buf, s->buf + s->buf_index, len);
    s->buf_index += len;
    return 0;
}

static inline BOOL is_eos(ASN1DecodeState *s)
{
    return s->buf_index >= s->buf_len;
}

/* return -1 if error. */
static int decode_tag(ASN1DecodeState *s, int *pis_constructed, int b)
{
    int tag, c, n;

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
    *pis_constructed = (b >> 5) & 1;
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

/* return -1 if error, TAG_EOC if end of constructed tag. Otherwise
   return the tag and modify the parse state so that the stream ends
   of the end of the tag. decode_tag_end() must be called to end the
   tag. */ 
static int decode_tag_start(ASN1DecodeState *s, ASN1ParserState *ps,
                            int *pis_constructed, int *plen)
{
    int len, tag, is_constructed, b;

    s->tag_index = s->buf_index;
    b = get_byte(s);
    if (b < 0) {
        if (!s->indefinite_len_state)
            return TAG_EOC;
        return -1;
    }
    if (b == 0) {
        /* end of content tag */
        if (!s->indefinite_len_state)
            return -1;
        b = get_byte(s);
        if (b != 0)
            return asn1_error(s, "expecting second end of content byte");
        s->got_eoc = TRUE;
        return TAG_EOC;
    }
    tag = decode_tag(s, &is_constructed, b);
    if (tag < 0)
        return -1;
#ifdef DEBUG
    printf("tag=0x%x c=%d\n", tag, is_constructed);
#endif
    len = decode_len(s);
#ifdef DEBUG
    printf("  len=%d\n", len);
#endif
    if (len == -1)
        return -1;
    ps->indefinite_len_state = s->indefinite_len_state;
    ps->buf_len = s->buf_len;
    if (len == INDEFINITE_LEN) {
        if (!is_constructed)
            return asn1_error(s, "indefinite length forbidden in primtive encoding");
        s->indefinite_len_state = TRUE;
        s->got_eoc = FALSE;
    } else {
        if (len > (s->buf_len - s->buf_index))
            return -1;
        s->buf_len = s->buf_index + len;
        s->indefinite_len_state = FALSE;
    }
    *pis_constructed = is_constructed;
    *plen = len;
    return tag;
}

/* return -1 if error, 0 if OK */
static int decode_tag_end(ASN1DecodeState *s, ASN1ParserState *ps)
{
    int b;
    if (s->indefinite_len_state) {
        if (!s->got_eoc) {
            /* get EOC if it was not read before */
            b = get_byte(s);
            if (b < 0)
                return -1;
            if (b != 0)
                goto eoc_expected;
            b = get_byte(s);
            if (b < 0)
                return -1;
            if (b != 0) {
            eoc_expected:
                return asn1_error(s, "EOC expected");
            }
        }
    } else {
        if (s->buf_index != s->buf_len) {
            return asn1_error(s, "extraneous characters in tag");
        }
    }
    s->buf_len = ps->buf_len;
    s->indefinite_len_state = ps->indefinite_len_state;
    s->got_eoc = FALSE;
    return 0;
}

/* XXX: limit recursion level ? */
static int asn1_ber_skip_tag(ASN1DecodeState *s, int len)
{
    ASN1ParserState ps;
    int is_constructed, tag;

    if (len == INDEFINITE_LEN) {
        for(;;) {
            tag = decode_tag_start(s, &ps, &is_constructed, &len);
            if (tag == -1)
                return -1;
            else if (tag == TAG_EOC)
                break;
            asn1_ber_skip_tag(s, len);
            if (decode_tag_end(s, &ps))
                return -1;
        }
    } else {
        /* skip bytes */
        if (len > (s->buf_len - s->buf_index))
            return asn1_error(s, "unexpected end of stream");
        s->buf_index += len;
    }
    return 0;
}


static int asn1_ber_decode_boolean(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data, int tag, 
                                   int is_constructed, int len)
{
    int b;

    if (is_constructed)
        return asn1_error_primitive(s);
    if (len != 1)
        return asn1_error(s, "boolean must contain a single byte");
    b = get_byte(s);
    if (b < 0)
        return -1;
    *(BOOL *)data = (b != 0);
    return 0;
}

static int asn1_decode_int32(ASN1DecodeState *s, int len, int *pval)
{
    int i, val, b, shift;
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

#ifdef ASN1_USE_LARGE_INTEGER
static int asn1_ber_decode_large_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                         uint8_t *data, int tag,
                                         int is_constructed, int len)
{
    ASN1Integer *r;
    int nb_limbs, i, l, b;
    ASN1Limb v;
    uint8_t buf[ASN1_LIMB_BYTES];
    
    r = (ASN1Integer *)data;
    asn1_integer_init(r);
    if (len == 0) {
        /* not valid, but we accept it as zero */
    } else {
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
        v = ((int)v << (32 - l * 8)) >> (32 - l * 8);
        r->data[nb_limbs - 1] = v;
        for(i = nb_limbs - 2; i >= 0; i--) {
            if (get_bytes(s, buf, ASN1_LIMB_BYTES) < 0)
                return -1;
            r->data[i] = to_be32(buf);
        }
        
        if (asn1_integer_from_2comp(r))
            return mem_error(s);
    }
    return 0;
}
#endif

static int asn1_ber_decode_integer(ASN1DecodeState *s, const ASN1CType *p, 
                                   uint8_t *data, int tag,
                                   int is_constructed, int len)
{
    int val, b;

    if (is_constructed)
        return asn1_error_primitive(s);
    if (p[0] & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_ber_decode_large_integer(s, p, data, tag, is_constructed, len);
#else
        return asn1_error(s, "large integers are not supported");
#endif
    }
    if (len > 5) 
        goto overflow;
    if (len == 5) {
        /* we allow 32 bit unsigned integers too */
        b = get_byte(s);
        if (b < 0)
            return -1;
        if (b != 0)
            goto overflow;
        len--;
    }
    
    if (asn1_decode_int32(s, len, &val))
        return -1;
    *(int *)data = val;
    return 0;
 overflow:
    return asn1_error(s, "integer larger than 32 bits");
}

/* XXX: limit recursion level ? */
static int asn1_ber_decode_bit_string_rec(ASN1DecodeState *s, 
                                          ASN1BitString *str,
                                          int tag1, int is_constructed, int len, size_t *psize)
{
    ASN1ParserState ps;
    int tag;

    if (is_constructed) {
        for(;;) {
            tag = decode_tag_start(s, &ps, &is_constructed, &len);
            if (tag == -1)
                return -1;
            else if (tag == TAG_EOC)
                break;
            if (tag != tag1)
                return asn1_error(s, "unexpected tag in constructed bit string encoding (tag=0x%x expected=0x%x)", tag, tag1);
            asn1_ber_decode_bit_string_rec(s, str, tag1, is_constructed, len,
                                             psize);
            if (decode_tag_end(s, &ps))
                return -1;
        }
    } else {
        int first_byte, bit_len, len1;
        uint8_t *buf;

        if (len < 1)
            return asn1_error(s, "bit string: length must be >= 1");
        first_byte = get_byte(s);
        if (first_byte < 0)
            return -1;
        if (len == 1) {
            if (first_byte != 0)
                return asn1_error(s, "bit string: first byte must be zero");
        } else {
            if (first_byte > 7)
                return asn1_error(s, "bit string: first byte must be <= 7");
        }
        len--;
        bit_len = len * 8 - first_byte;
        if (*psize == 0) {
            /* optimization for the most common case: not constructed
               bit string */
            buf = asn1_malloc(len);
            if (!buf)
                return mem_error(s);
            str->buf = buf;
            str->len = bit_len;
            *psize = len;
        } else {
            if ((str->len & 7) != 0)
                return asn1_error(s, "bit string: segment bit length is not a multiple of 8");
            len1 = str->len >> 3;
            if (asn1_realloc_buf(&str->buf, 1, psize, len1 + len))
                return mem_error(s);
            str->len += bit_len;
            buf = str->buf + len1;
        }
        if (get_bytes(s, buf, len) < 0)
            return -1;
        /* mask unused bits in last byte */
        if (len > 0) {
            buf[len - 1] &= 0xff00 >> (8 - first_byte);
        }
    }
    return 0;
}

static int asn1_ber_decode_bit_string(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data, int tag, 
                                      int is_constructed, int len)
{
    ASN1BitString *str;
    int ret;
    size_t size;
    
    str = (ASN1BitString *)data;
    size = 0;
    ret = asn1_ber_decode_bit_string_rec(s, str, ASN1_TAG_UNIVERSAL + 3, 
                                         is_constructed, len, &size);
    if (ret)
        return ret;
#ifdef DEBUG
    printf("bit string: len=%d\n", (int)str->len);
#endif
    return 0;
}

/* XXX: limit recursion level ? */
static int asn1_ber_decode_octet_string_rec(ASN1DecodeState *s, ASN1String *str,
                                            int tag1, int is_constructed, int len, size_t *psize)
{
    ASN1ParserState ps;
    int tag;

    if (is_constructed) {
        for(;;) {
            tag = decode_tag_start(s, &ps, &is_constructed, &len);
            if (tag == -1)
                return -1;
            else if (tag == TAG_EOC)
                break;
            if (tag != tag1)
                return asn1_error(s, "unexpected tag in constructed string encoding (tag=0x%x expected=0x%x)", tag, tag1);
            asn1_ber_decode_octet_string_rec(s, str, tag1, is_constructed, len,
                                             psize);
            if (decode_tag_end(s, &ps))
                return -1;
        }
    } else {
        if (asn1_realloc_buf(&str->buf, 1, psize, str->len + len))
            return mem_error(s);
        if (get_bytes(s, str->buf + str->len, len) < 0)
            return -1;
        str->len += len;
    }
    return 0;
}

static int asn1_ber_decode_octet_string(ASN1DecodeState *s, const ASN1CType *p, 
                                        uint8_t *data, int tag, 
                                        int is_constructed, int len)
{
    ASN1String *str;
    int ret;
    size_t size;
    
    str = (ASN1String *)data;
    if (is_constructed) {
        str->buf = NULL;
        str->len = 0;
        size = 0;
        ret = asn1_ber_decode_octet_string_rec(s, str, ASN1_TAG_UNIVERSAL + 4, 1, len, &size);
        if (ret)
            return ret;
    } else {
        str->buf = asn1_malloc(len);
        if (!str->buf)
            return mem_error(s);
        str->len = len;
        if (get_bytes(s, str->buf, len) < 0)
            return -1;
    }
#ifdef DEBUG
    printf("octet string: len=%d\n", (int)str->len);
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

/* find a given tag among the 'count' fields. Return the matching
   index or -1 if not found */
static int sequence_find_tag(const ASN1SequenceField *f, 
                             int count, int tag)
{
    int i;

#ifdef DEBUG
    printf("searching comp tag: 0x%x\n", tag);
#endif
    for(i = 0; i < count; i++) {
        if (find_tag1(f->type, tag))
            return i;
        f++;
    }
    return -1;
}
                             
static int asn1_ber_decode_sequence(ASN1DecodeState *s, const ASN1CType *p1, 
                                    uint8_t *data, int tag1,
                                    int is_constructed, int len)
{
    int ret, is_set, field_idx, field_idx1, nb_fields;
    int tag, flags, flag, idx;
    BOOL is_last_normal;
    const ASN1CType *p;
    const ASN1SequenceField *f, *f1;
    uint8_t *table_present;
    ASN1ParserState ps;
    ASN1ValueStack value_entry;

    if (!is_constructed)
        return asn1_error_constructed(s);
    
    value_entry.prev = s->top_value;
    value_entry.type = p1;
    value_entry.data = data;
    s->top_value = &value_entry;

    p = p1;
    flags = *p++;
    is_set = (ASN1_GET_CTYPE(flags) == ASN1_CTYPE_SET);
    nb_fields = *p++;
    p++;
    f1 = (const ASN1SequenceField *)p;

    /* build the linear field list */
    table_present = asn1_mallocz(nb_fields);
    if (!table_present)
        return mem_error(s);

    field_idx = 0;
    for(;;) {
        tag = decode_tag_start(s, &ps, &is_constructed, &len);
        if (tag == -1) {
            ret = -1;
            goto fail;
        } else if (tag == TAG_EOC) {
            break;
        }

        if (field_idx >= nb_fields) {
            /* skip tag */
        skip_tag:
            ret = asn1_ber_skip_tag(s, len);
            if (ret)
                goto fail;
        } else {
            /* compute first and last possible fields */
            if (is_set) {
                is_last_normal = FALSE;
                field_idx1 = nb_fields;
            } else {
                field_idx1 = field_idx;
                is_last_normal = FALSE;
                for(;;) {
                    f = f1 + field_idx1;
                    flag = ASN1_GET_SEQ_FLAG(f);
                    field_idx1++;
                    
                    if (flags & ASN1_CTYPE_HAS_ROOT_AFTER_EXT) {
                        /* consider extension as optional in this case */
                        if (flag == ASN1_SEQ_FLAG_NORMAL && 
                            !ASN1_IS_SEQ_EXT(f)) {
                            is_last_normal = TRUE;
                            break;
                        }
                    } else {
                        if (flag == ASN1_SEQ_FLAG_NORMAL) {
                            is_last_normal = TRUE;
                            break;
                        }
                    }
                    if (field_idx1 >= nb_fields)
                        break;
                }
            }
            
            idx = sequence_find_tag(f1 + field_idx, 
                                    field_idx1 - field_idx, tag);
            if (idx < 0) {
                if (is_last_normal) {
                    ret = asn1_error(s, "tag 0x%x not found in SET/SEQUENCE",
                                     tag);
                    goto fail;
                } else {
                    if (!is_set)
                        field_idx = nb_fields;
                    goto skip_tag;
                }
            }
            idx += field_idx;
            f = f1 + idx;
            if (table_present[idx]) {
                ret = asn1_error(s, "component %s (tag=0x%x) present twice in SET/SEQUENCE", f->name, tag);
                goto fail;
            }
            ret = asn1_ber_decode_type_tag(s, f->type, 
                                           data + ASN1_GET_SEQ_OFFSET(f),
                                           tag, is_constructed, len);
            if (ret)
                goto fail;
            /* mark as present */
            flag = ASN1_GET_SEQ_FLAG(f);
            table_present[idx] = 1;
            if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
                (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
                *(BOOL *)(data + f->u.option_offset) = 1;
            }
            
            if (is_last_normal && idx == (field_idx1 - 1)) {
                /* move to next group of fields */
                field_idx = field_idx1;
            }
        }
        ret = decode_tag_end(s, &ps);
        if (ret)
            goto fail;
    }
    
    if (asn1_sequence_set_default_fields(f1, nb_fields, data, table_present)) {
        ret = mem_error(s);
        goto fail;
    }
    
    /* check that all mandatory components are present */
    if (asn1_sequence_check_fields(f1, nb_fields, table_present, 
                                   s->error.msg, sizeof(s->error.msg))) {
        ret = asn1_error_internal(s);
        goto fail;
    }
    ret = 0;
 fail:
    asn1_free(table_present);
    s->top_value = s->top_value->prev;
    return ret;
}

static int asn1_ber_decode_sequence_of(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data, int tag1,
                                       int is_constructed, int len)
{
    const ASN1SequenceOfCType *f;
    int ret, flags, tag;
    ASN1String *str;
    ASN1ParserState ps;
    uint8_t *buf;
    size_t size;

    if (!is_constructed) {
        return asn1_error_constructed(s);
    }
    flags = *p++;
    p++;
    if (flags & ASN1_CTYPE_HAS_HIGH) 
        p++;
    
    str = (ASN1String *)data;
    str->buf = NULL;
    str->len = 0;
    size = 0;

    f = (const ASN1SequenceOfCType *)p;
    for(;;) {
        tag = decode_tag_start(s, &ps, &is_constructed, &len);
        if (tag == -1)
            return -1;
        if (tag == TAG_EOC)
            break;
        if ((str->len + 1) > size) {
            if (asn1_reallocz_buf(&str->buf, f->elem_size, 
                                  &size, str->len + 1) < 0)
                return mem_error(s);
        }
        str->len++;
        ret = asn1_ber_decode_type_tag(s, f->type, 
                                       str->buf + (str->len - 1) * f->elem_size,
                                       tag, is_constructed, len);
        if (ret)
            return ret;
        ret = decode_tag_end(s, &ps);
        if (ret)
            return ret;
    }

    /* we realloc to the exact size to avoid wasting space */
    if (size != str->len) {
        buf = asn1_realloc2(str->buf, str->len, f->elem_size);
        if (buf)
            str->buf = buf;
    }

#ifdef DEBUG
    printf("seq of: len=%d\n", (int)str->len);
#endif
    return 0;
}

static int asn1_ber_decode_choice(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data, int tag,
                                  int is_constructed, int len)
{
    int nb_fields, i, flags, ret;
    uint32_t data_offset, choice_offset;
    const ASN1ChoiceField *f;
    ASN1ParserState ps;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++;
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    
    if (ASN1_GET_TAG(flags) != ASN1_NO_TAG) {
        if (!is_constructed)
            return asn1_error_constructed(s);
        tag = decode_tag_start(s, &ps, &is_constructed, &len);
        if (tag < 0)
            return tag;
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
    ret = asn1_ber_decode_type_tag(s, f->type, data + data_offset, 
                                   tag, is_constructed, len);
    if (ret)
        return ret;
    if (ASN1_GET_TAG(flags) != ASN1_NO_TAG) {
        ret = decode_tag_end(s, &ps);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_ber_decode_enumerated(ASN1DecodeState *s, const ASN1CType *p, 
                                      uint8_t *data, int tag,
                                      int is_constructed, int len)
{
    int nb_fields, val, flags;

    if (is_constructed)
        return asn1_error_primitive(s);
    
    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    if (len == 0 || len > 4)
        goto out_of_range;
    if (asn1_decode_int32(s, len, &val))
        return -1;
    val = asn1_find_enum_index(p, nb_fields, val, flags);
    if (val < 0)
        goto out_of_range;
    *(uint32_t *)data = val;
    return 0;
 out_of_range:
    return asn1_error(s, "enumerated value out of range");
}

static int asn1_ber_decode_null(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data, int tag, 
                                int is_constructed, int len)
{
    if (is_constructed)
        return asn1_error_primitive(s);
    if (len != 0)
        return asn1_error(s, "expecting zero length for null type");
    return 0;
}

static int asn1_ber_decode_tagged(ASN1DecodeState *s, const ASN1CType *p, 
                                  uint8_t *data, int tag, 
                                  int is_constructed, int len)
{
    uint8_t *data1;
    int flags, ret;
    ASN1CType *p1;

    flags = p[0];
    p1 = (ASN1CType *)p[1];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data1 = asn1_mallocz_value(p1);
        if (!data1)
            return mem_error(s);
        *(void **)data = data1;
    } else {
        data1 = data;
    }
    if (ASN1_GET_TAG(flags) == ASN1_NO_TAG) {
        ret = asn1_ber_decode_type_tag(s, p1, data1, 
                                       tag, is_constructed, len);
        if (ret)
            return ret;
    } else if (flags & ASN1_CTYPE_HAS_EXPLICIT) {
        if (!is_constructed)
            return asn1_error_constructed(s);
        ret = asn1_ber_decode_type(s, p1, data1);
        if (ret)
            return ret;
    } else {
        ret = asn1_ber_decode_type_tag(s, p1, data1, 
                                       ASN1_GET_TAG(p1[0]), is_constructed, 
                                       len);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_ber_decode_object_identifier(ASN1DecodeState *s, const ASN1CType *p, 
                                             uint8_t *data, int tag, 
                                             int is_constructed, int len)
{
    ASN1String *str;

    str = (ASN1String *)data;
    if (is_constructed)
        return asn1_error_primitive(s);
    str->buf = asn1_malloc(len);
    if (!str->buf)
        return mem_error(s);
    str->len = len;
    if (get_bytes(s, str->buf, len) < 0)
        return -1;
    return 0;
}

static int asn1_ber_decode_real(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data, int tag, 
                                int is_constructed, int len)
{
    double d;
    if (is_constructed)
        return asn1_error_primitive(s);
    if (asn1_decode_real_ber(s->buf + s->buf_index, len, &d))
        return asn1_error(s, "real: error in encoding");
    s->buf_index += len;
    *(double *)data = d;
    return 0;
}

static int asn1_ber_decode_char_string(ASN1DecodeState *s, const ASN1CType *p, 
                                       uint8_t *data, int tag, 
                                       int is_constructed, int len)
{
    ASN1String *str;
    int char_string_type, shift, ret;

    char_string_type = p[1];
    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;

    ret = asn1_ber_decode_octet_string(s, p, data, tag, is_constructed, len);
    if (ret)
        return ret;
    if (shift != 0) {
        int i, len;
        uint8_t *buf;

        str = (ASN1String *)data;
        len = str->len;
        if (len & ((1 << shift) - 1))
            return asn1_error(s, "character string: octet count must be multiple of %d", 1 << shift);
        len >>= shift;
        str->len = len;
        /* change endianness (XXX: optimize) */
        buf = str->buf;
        if (shift == 1) {
            uint16_t *buf1;
            buf1 = (uint16_t *)str->buf;
            for(i = 0; i < len; i++)
                buf1[i] = (buf[2 * i] << 8) | buf[2 * i + 1];
        } else {
            uint32_t *buf1;
            buf1 = (uint32_t *)str->buf;
            for(i = 0; i < len; i++)
                buf1[i] = (buf[4 * i] << 24) | (buf[4 * i + 1] << 16) |
                    (buf[4 * i + 2] << 8) | (buf[4 * i + 3]);
        }
    }
    return 0;
}

static int asn1_ber_decode_any(ASN1DecodeState *s, const ASN1CType *p, 
                               uint8_t *data, int tag, 
                               int is_constructed, int len)
{
    ASN1OpenType *str;
    int buf_start, ret, l, flags;
    ASN1ParserState ps;

    flags = p[0];
    
    if (ASN1_GET_TAG(flags) != ASN1_NO_TAG) {
        if (!is_constructed)
            return asn1_error_constructed(s);
        tag = decode_tag_start(s, &ps, &is_constructed, &len);
        if (tag < 0)
            return tag;
    }
    
    str = (ASN1OpenType *)data;
    str->type = (ASN1CType *)asn1_get_constrained_type(s->top_value, p);
    if (!str->type) {
        buf_start = s->tag_index;
        ret = asn1_ber_skip_tag(s, len);
        if (ret)
            return ret;
        l = s->buf_index - buf_start;
        str->u.octet_string.len = l;
        str->u.octet_string.buf = asn1_malloc(l);
        if (!str->u.octet_string.buf)
            return mem_error(s);
        memcpy(str->u.octet_string.buf, s->buf + buf_start, l);
    } else {
        str->u.data = asn1_mallocz_value(str->type);
        if (!str->u.data)
            return mem_error(s);
        ret = asn1_ber_decode_type_tag(s, str->type, str->u.data, 
                                       tag, is_constructed, len);
        if (ret)
            return ret;
    }

    if (ASN1_GET_TAG(flags) != ASN1_NO_TAG) {
        ret = decode_tag_end(s, &ps);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_ber_decode_type_tag(ASN1DecodeState *s, const ASN1CType *p, 
                                    uint8_t *data, int tag, 
                                    int is_constructed, int len)
{
    int type, ret, tag1, flags;
    
    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
    tag1 = ASN1_GET_TAG(flags);
#ifdef DEBUG
    printf("type=%s tag=0x%x\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?", tag1);
#endif
    if ((type == ASN1_CTYPE_CHOICE || type == ASN1_CTYPE_ANY) &&
        tag1 == ASN1_NO_TAG) {
        /* no tag to compare */
    } else {
        if (tag != tag1)
            return asn1_error(s, "unexpected tag 0x%x (expecting tag 0x%x)\n",
                              tag, tag1);
    }
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_ber_decode_sequence(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_ber_decode_sequence_of(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_ber_decode_choice(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_ber_decode_enumerated(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_ber_decode_boolean(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_ber_decode_integer(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_NULL:
        ret = asn1_ber_decode_null(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_ber_decode_octet_string(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_ber_decode_bit_string(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_ber_decode_tagged(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_ber_decode_object_identifier(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_ber_decode_real(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_ber_decode_char_string(s, p, data, tag, is_constructed, len);
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_ber_decode_any(s, p, data, tag, is_constructed, len);
        break;
    default:
        ret = asn1_error(s, "unsupported type (%d)", type);
        break;
    }
    return ret;
}

static int asn1_ber_decode_type(ASN1DecodeState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    int ret, tag, is_constructed, len;
    ASN1ParserState ps;
    
    tag = decode_tag_start(s, &ps, &is_constructed, &len);
    if (tag < 0)
        return tag;
    ret = asn1_ber_decode_type_tag(s, p, data, tag, is_constructed, len);
    if (ret != 0)
        return ret;
    return decode_tag_end(s, &ps);
}

/* BER decoding. Return the number of consumed bytes or < 0 if
   error. */
asn1_ssize_t asn1_ber_decode(void **pdata, const ASN1CType *p,
                         const uint8_t *buf, size_t buf_len, ASN1Error *err)
{
    ASN1DecodeState s_s, *s = &s_s;
    int ret;
    void *data;

    s->buf = buf;
    s->buf_len = buf_len;
    s->buf_index = 0;
    s->indefinite_len_state = FALSE;
    s->got_eoc = FALSE;
    s->top_value = NULL;

    s->error.bit_pos = 0;
    s->error.msg[0] = '\0';
    
    data = asn1_mallocz_value(p);
    if (!data) {
        mem_error(s);
        goto fail;
    }
    
    ret = asn1_ber_decode_type(s, p, data);
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

