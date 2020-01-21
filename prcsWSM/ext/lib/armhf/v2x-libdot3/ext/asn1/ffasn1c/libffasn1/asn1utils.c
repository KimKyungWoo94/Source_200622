/* 
 * ASN1 runtime utilities
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
#include <math.h>

#include "asn1defs_int.h"

#if defined(CONFIG_VERSION) && 0
const char __asn1_version_string[] = "ffasn1 runtime version " CONFIG_VERSION " - Copyright 2011-2018 Fabrice Bellard";
#endif

const char *asn1_ctype_names[ASN1_CTYPE_COUNT] = {
    "SEQUENCE",
    "SEQUENCE_OF",
    "SET",
    "SET_OF",
    "CHOICE",
    "INTEGER",
    "BOOLEAN",
    "BIT_STRING",
    "OCTET_STRING",
    "ENUMERATED",
    "NULL",
    "TAGGED",
    "OBJECT_IDENTIFIER",
    "RELATIVE_OID",
    "REAL",
    "CHAR_STRING",
    "ANY",
    "OBJECT_CLASS",
};

const char *asn1_char_string_str[ASN1_CSTR_COUNT] = {
    "NumericString",
    "PrintableString",
    "VisibleString",
    "IA5String",
    "BMPString",
    "UniversalString",

    "UTF8String",
    "GeneralString",
    "GraphicString",
    "TeletexString",
    "VideotexString",
};

void *asn1_mallocz(size_t size)
{
    void *ptr;
    ptr = asn1_malloc(size);
    if (!ptr)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
}

char *asn1_strdup(const char *str)
{
    size_t len = strlen(str);
    char *s;
    s = asn1_malloc(len + 1);
    if (!s)
        return NULL;
    memcpy(s, str, len + 1);
    return s;
}

void asn1_pstrcpy(char *buf, size_t buf_size, const char *str)
{
    int c;
    char *q = buf;

    if (buf_size <= 0)
        return;

    for(;;) {
        c = *str++;
        if (c == 0 || q >= buf + buf_size - 1)
            break;
        *q++ = c;
    }
    *q = '\0';
}

/* precondition: len > *psize */
asn1_exception int __asn1_realloc_buf(uint8_t **pbuf, size_t elem_size, 
                                      size_t *psize, size_t len, BOOL zero_init)
{
    size_t size, new_size;
    uint8_t *buf;

    size = *psize;
    /* realloc buffer */
    new_size = size + (size / 2);
    if (new_size < 4)
        new_size = 4;
    if (new_size < len)
        new_size = len;
    /* we don't allow a size which cannot be represented with a
       ssize_t (or intptr_t) */
    if (new_size > INTPTR_MAX)
        return -1;
    buf = asn1_realloc2(*pbuf, new_size, elem_size);
    if (!buf)
        return -1;
    if (zero_init) {
        memset(buf + size * elem_size, 0, 
               (new_size - size) * elem_size);
    }
    *pbuf = buf;
    *psize = new_size;
    return 0;
}

void asn1_byte_buffer_init(ASN1ByteBuffer *s)
{
    s->buf = NULL;
    s->len = 0;
    s->size = 0;
    s->has_error = FALSE;
}

int __asn1_byte_buffer_realloc(ASN1ByteBuffer *s, size_t size)
{
    size_t new_size;
    uint8_t *new_buf;

    if (s->has_error)
        return -1;
    new_size = s->size + (s->size / 2);
    if (new_size < 16)
        new_size = 16;
    if (new_size < size)
        new_size = size;
    /* we don't allow a size which cannot be represented with a
       ssize_t (or intptr_t) */
    if (new_size > INTPTR_MAX) {
        s->has_error = TRUE;
        return -1;
    }
    new_buf = asn1_realloc(s->buf, new_size);
    if (!new_buf) {
        s->has_error = TRUE;
        return -1;
    }
    s->size = new_size;
    s->buf = new_buf;
    return 0;
}

void asn1_put_bytes(ASN1ByteBuffer *s, const uint8_t *buf, size_t count)
{
    if (unlikely(asn1_byte_buffer_expand(s, count)))
        return;
    memcpy(s->buf + s->len, buf, count);
    s->len += count;
}

void asn1_puts(ASN1ByteBuffer *s, const char *str)
{
    asn1_put_bytes(s, (const uint8_t *)str, strlen(str));
}

void asn1_vprintf(ASN1ByteBuffer *s, const char *fmt, va_list ap)
{
    int ret;
    size_t size;
    
    size = 512;
    for(;;) {
        if (unlikely(asn1_byte_buffer_expand(s, size)))
            return;
        size = s->size - s->len;
        ret = vsnprintf((char *)(s->buf + s->len), size, fmt, ap);
        if (ret >= size) {
            /* C99 case */
            size = ret + 1;
        } else if (ret < 0) {
            /* Windows / old glibc case */
            size *= 2;
        } else {
            s->len += ret;
            break;
        }
    }
}

void asn1_printf(ASN1ByteBuffer *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    asn1_vprintf(s, fmt, ap);
    va_end(ap);
}

/* type names */

const char *asn1_get_generic_type_name(const ASN1CType *p)
{
    int type, char_string_type;
    const char *str;

    for(;;) {
        type = ASN1_GET_CTYPE(p[0]);
        if (type != ASN1_CTYPE_TAGGED)
            break;
        p = (ASN1CType *)p[1];
    }
    /* XXX: get real type name when available */
    if (type == ASN1_CTYPE_CHAR_STRING) {
        char_string_type = p[1];
        if (char_string_type >= ASN1_CSTR_COUNT)
            goto invalid_type;
        str = asn1_char_string_str[char_string_type];
    } else if (type >= ASN1_CTYPE_COUNT || type == ASN1_CTYPE_ANY) {
    invalid_type:
        str = "?";
    } else {
        str = asn1_ctype_names[type];
    }
    return str;
}

const char *asn1_get_user_type_name(const ASN1CType *p)
{
    int flags, type, nb_fields;

    flags = *p++;
    if (!(flags & ASN1_CTYPE_HAS_NAME))
        return NULL;
    type = ASN1_GET_CTYPE(flags);
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        nb_fields = *p++;
        p++;
        p += nb_fields * 4;
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        p++;
        if (flags & ASN1_CTYPE_HAS_HIGH)
            p++;
        p += 3;
        break;
    case ASN1_CTYPE_CHOICE:
        nb_fields = *p++;
        if (flags & ASN1_CTYPE_HAS_EXT)
            nb_fields += *p++;
        p += 3;
        p += 2 * nb_fields;
        break;
    case ASN1_CTYPE_ENUMERATED:
        nb_fields = *p++;
        if (flags & ASN1_CTYPE_HAS_EXT)
            nb_fields += *p++;
        p += nb_fields;
        break;
    case ASN1_CTYPE_INTEGER:
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_BIT_STRING:
        if (flags & ASN1_CTYPE_HAS_LOW) 
            p++;
        if (flags & ASN1_CTYPE_HAS_HIGH) 
            p++;
        break;
    case ASN1_CTYPE_BOOLEAN:
    case ASN1_CTYPE_NULL:
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
    case ASN1_CTYPE_ANY:
    case ASN1_CTYPE_TAGGED:
    case ASN1_CTYPE_REAL:
        break;
    case ASN1_CTYPE_CHAR_STRING:
        {
            int char_string_type;
            char_string_type = *p++;
            if (char_string_type <= ASN1_CSTR_UniversalString) {
                if (flags & ASN1_CTYPE_HAS_LOW) 
                    p++;
                if (flags & ASN1_CTYPE_HAS_HIGH) 
                    p++;
                nb_fields = *p++;
                p += nb_fields * 2;
            }
        }
        break;
    case ASN1_CTYPE_OBJECT_CLASS:
        nb_fields = *p++;
        p += nb_fields * 4;
        break;
    default:
        return NULL;
    }
    return (char *)(*p);
}

const char *asn1_get_type_name(const ASN1CType *p)
{
    const char *name;
    name = asn1_get_user_type_name(p);
    if (!name)
        name = asn1_get_generic_type_name(p);
    return name;
}

/* free ASN1 objects */

static void asn1_free_sequence(const ASN1CType *p, void *data)
{
    int nb_fields, i, flag;
    const ASN1SequenceField *f;
    BOOL present;

    p++;
    nb_fields = *p++;
    p++; /* sizeof(struct) */
    f = (const ASN1SequenceField *)p;
    for(i = 0; i < nb_fields; i++) {
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            present = *(BOOL *)((uint8_t *)data + f->u.option_offset);
        } else {
            present = 1;
        }
        if (present) {
            asn1_free_value_struct(f->type, (uint8_t *)data + ASN1_GET_SEQ_OFFSET(f));
        }
        f++;
    }
}

static void asn1_free_sequence_of(const ASN1CType *p, void *data)
{
    const ASN1SequenceOfCType *f;
    int flags;
    uint32_t i;
    ASN1String *str;

    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_LOW)
        p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    str = (ASN1String *)data;
    for(i = 0; i < str->len; i++) {
        asn1_free_value_struct(f->type, str->buf + i * f->elem_size);
    }
    asn1_free(str->buf);
}

static void asn1_free_choice(const ASN1CType *p, void *data)
{
    int flags, nb_fields;
    uint32_t val;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++; /* sizeof(struct) */
    val = *(uint32_t *)((uint8_t *)data + p[0]);
    data = (uint8_t *)data + p[1];
    p += 2;
    if (val < nb_fields) {
        f = (const ASN1ChoiceField *)p + val;
        asn1_free_value_struct(f->type, data);
    }
}

void asn1_free_value_struct(const ASN1CType *p, void *data)
{
    int type, flags;

    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        asn1_free_sequence(p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        asn1_free_sequence_of(p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        asn1_free_choice(p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
            asn1_integer_clear(data);
#else
            abort();
#endif
        }
        break;
    case ASN1_CTYPE_ENUMERATED:
    case ASN1_CTYPE_BOOLEAN:
    case ASN1_CTYPE_NULL:
    case ASN1_CTYPE_REAL:
        break;
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_BIT_STRING:
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
    case ASN1_CTYPE_CHAR_STRING:
         {
             ASN1String *str = (ASN1String *)data;
             asn1_free(str->buf);
         }
         break;
    case ASN1_CTYPE_ANY:
        {
            ASN1OpenType *str = (ASN1OpenType *)data;
            if (str->type) {
                asn1_free_value(str->type, str->u.data);
            } else {
                asn1_free(str->u.octet_string.buf);
            }
        }
        break;
    case ASN1_CTYPE_TAGGED:
        if (flags & ASN1_CTYPE_HAS_POINTER) {
            asn1_free_value((ASN1CType *)p[1], *(void **)data);
        } else {
            asn1_free_value_struct((ASN1CType *)p[1], data);
        }
        break;
    default:
        abort();
    }
}

void asn1_free_value(const ASN1CType *p, void *data)
{
    asn1_free_value_struct(p, data);
    asn1_free(data);
}

/* return < 0 if error */
asn1_ssize_t asn1_get_size(const ASN1CType *p)
{
    int type;
    uint32_t flags;
    size_t size;
    
    flags = *p++;
    type = ASN1_GET_CTYPE(flags);
    
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        p++;
        size = *p;
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        size = sizeof(ASN1String);
        break;
    case ASN1_CTYPE_CHOICE:
        p++;
        if (flags & ASN1_CTYPE_HAS_EXT)
            p++;
        size = *p;
        break;
    case ASN1_CTYPE_ENUMERATED:
        size = sizeof(int);
        break;
    case ASN1_CTYPE_INTEGER:
        if (flags & ASN1_CTYPE_HAS_LARGE) {
            size = sizeof(ASN1Integer);
        } else {
            size = sizeof(int);
        }
        break;
    case ASN1_CTYPE_BOOLEAN:
        size = sizeof(BOOL);
        break;
    case ASN1_CTYPE_NULL:
        size = sizeof(ASN1Null);
        break;
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_BIT_STRING:
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
    case ASN1_CTYPE_CHAR_STRING:
        size = sizeof(ASN1String);
        break;
    case ASN1_CTYPE_ANY:
        size = sizeof(ASN1OpenType);
        break;
    case ASN1_CTYPE_TAGGED:
        if (flags & ASN1_CTYPE_HAS_POINTER)
            size = sizeof(void *);
        else
            size = asn1_get_size((ASN1CType *)*p);
        break;
    case ASN1_CTYPE_REAL:
        size = sizeof(double);
        break;
    case ASN1_CTYPE_OBJECT_CLASS:
        size = (*p++) * sizeof(ASN1CType);
        break;
    default:
        size = -1;
        break;
    }
    return size;
}

void *asn1_mallocz_value(const ASN1CType *p)
{
    asn1_ssize_t size;
    size = asn1_get_size(p);
    if (size < 0)
        return NULL;
    return asn1_mallocz(size);
}

/* comparison */

static inline int cmp_int(int a, int b)
{
    if (a < b)
        return -1;
    else if (a == b)
        return 0;
    else
        return 1;
}

static inline int cmp_uint(unsigned int a, unsigned int b)
{
    if (a < b)
        return -1;
    else if (a == b)
        return 0;
    else
        return 1;
}

static inline uint64_t normalize_real(double a)
{
    union {
        double d;
        uint64_t v;
    } u;
    uint64_t v;

    u.d = a;
    v = u.v;
    if ((v & UINT64_C(0x7fffffffffffffff)) > 
        UINT64_C(0x7ff0000000000000)) {
        /* NaN case: largest */
        v = UINT64_C(0xffffffffffffffff);
    } else if (v & UINT64_C(0x8000000000000000)) {
        /* negative */
        v = ~v;
    } else {
        /* positive */
        v |= UINT64_C(0x8000000000000000);
    }
    return v;
}

/* real comparison with ASN1 rules (Nan is the largest, -0 < 0) */
int asn1_cmp_real(double a1, double a2)
{
    uint64_t v1, v2;
    v1 = normalize_real(a1);
    v2 = normalize_real(a2);
    if (v1 < v2)
        return -1;
    else if (v1 == v2)
        return 0;
    else
        return 1;
}

static int asn1_cmp_sequence(const ASN1CType *p, const uint8_t *data1,
                             const uint8_t *data2)
{
    int nb_fields, i, flag, ret;
    const ASN1SequenceField *f;
    BOOL present, present2;

    p++;
    nb_fields = *p++;
    p++; /* sizeof(struct) */
    f = (const ASN1SequenceField *)p;
    for(i = 0; i < nb_fields; i++) {
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            present = *(BOOL *)(data1 + f->u.option_offset);
            present2 = *(BOOL *)(data2 + f->u.option_offset);
            ret = cmp_int(present, present2);
            if (ret)
                return ret;
        } else {
            present = 1;
        }
        if (present) {
            ret = asn1_cmp_value(f->type, data1 + ASN1_GET_SEQ_OFFSET(f),
                                 data2 + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
        }
        f++;
    }
    return 0;
}

/* XXX: should compare set of assuming no order by sorting them */
static int asn1_cmp_sequence_of(const ASN1CType *p, const uint8_t *data1,
                                const uint8_t *data2)
{
    const ASN1SequenceOfCType *f;
    int flags, ret;
    uint32_t i;
    const ASN1String *str1, *str2;

    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_LOW)
        p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    str1 = (ASN1String *)data1;
    str2 = (ASN1String *)data2;
    ret = cmp_uint(str1->len, str2->len);
    if (ret)
        return ret;
    for(i = 0; i < str1->len; i++) {
        ret = asn1_cmp_value(f->type, str1->buf + i * f->elem_size,
                             str2->buf + i * f->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_cmp_choice(const ASN1CType *p, const uint8_t *data1,
                           const uint8_t *data2)
{
    int flags, nb_fields, ret;
    uint32_t val, val2, choice_offset, data_offset;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++; /* sizeof(struct) */
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    val = *(uint32_t *)(data1 + choice_offset);
    val2 = *(uint32_t *)(data2 + choice_offset);
    ret = cmp_uint(val, val2);
    if (ret)
        return ret;
    if (val >= nb_fields)
        return 0;
    f = (const ASN1ChoiceField *)p + val;
    return asn1_cmp_value(f->type, data1 + data_offset, data2 + data_offset);
}

/* Return < 0, 0 or > 0 depending on the comparison result. A total
   order is defined among the values.

   Note: we assume no trailing zeros for bit strings and booleans
   represented as 0 or 1 */
int asn1_cmp_value(const ASN1CType *p, const void *data1,
                   const void *data2)
{
    int type, flags, ret;

    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_cmp_sequence(p, data1, data2);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_cmp_sequence_of(p, data1, data2);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_cmp_choice(p, data1, data2);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = cmp_int(*(BOOL *)data1, *(BOOL *)data2);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = cmp_int(*(int *)data1, *(int *)data2);
        break;
    case ASN1_CTYPE_INTEGER:
        if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
            ret = asn1_integer_cmp(data1, data2);
#else
            abort();
#endif
        } else {
            if ((flags & ASN1_CTYPE_HAS_LOW) && 
                !(flags & ASN1_CTYPE_HAS_EXT) && 
                (int)p[1] >= 0) {
                ret = cmp_uint(*(uint32_t *)data1, *(uint32_t *)data2);
            } else {
                ret = cmp_int(*(int *)data1, *(int *)data2);
            }
        }
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        {
            ASN1String *str1 = (ASN1String *)data1;
            ASN1String *str2 = (ASN1String *)data2;
            ret = cmp_uint(str1->len, str2->len);
            if (ret == 0) {
                ret = memcmp(str1->buf, str2->buf, str1->len);
            }
        }
        break;
    case ASN1_CTYPE_ANY:
        {
            ASN1OpenType *str1 = (ASN1OpenType *)data1;
            ASN1OpenType *str2 = (ASN1OpenType *)data2;
            if (str1->type != str2->type) {
                if (str1->type < str2->type)
                    ret = -1;
                else
                    ret = 1;
            } else {
                if (str1->type) {
                    ret = asn1_cmp_value(str1->type, str1->u.data, str2->u.data);
                } else {
                    ret = cmp_uint(str1->u.octet_string.len,
                                   str2->u.octet_string.len);
                    if (ret == 0) {
                        ret = memcmp(str1->u.octet_string.buf, str2->u.octet_string.buf, str1->u.octet_string.len);
                    }
                }
            }
        }
        break;
    case ASN1_CTYPE_BIT_STRING:
        {
            ASN1BitString *str1 = (ASN1BitString *)data1;
            ASN1BitString *str2 = (ASN1BitString *)data2;
            ret = cmp_uint(str1->len, str2->len);
            if (ret == 0) {
                /* Note: we assume zero padded */
                ret = memcmp(str1->buf, str2->buf, (str1->len + 7) / 8);
            }
        }
        break;
    case ASN1_CTYPE_TAGGED:
        {
            if (flags & ASN1_CTYPE_HAS_POINTER) {
                data1 = *(void **)data1;
                data2 = *(void **)data2;
            }
            ret = asn1_cmp_value((ASN1CType *)p[1], data1, data2);
        }
        break;
    case ASN1_CTYPE_REAL:
        {
            /* Note: we do a bit compare, which gives a different
               result than comparing the double values */
            ret = asn1_cmp_real(*(double *)data1, *(double *)data2);
        }
        break;
    case ASN1_CTYPE_CHAR_STRING:
        {
            ASN1String *str1 = (ASN1String *)data1;
            ASN1String *str2 = (ASN1String *)data2;
            int i, len;
            ret = cmp_uint(str1->len, str2->len);
            if (ret == 0) {
                len = str1->len;
                if (p[1] == ASN1_CSTR_BMPString) {
                    const uint16_t *tab1 = (const uint16_t *)str1->buf;
                    const uint16_t *tab2 = (const uint16_t *)str2->buf;
                    for(i = 0; i < len; i++) {
                        if (tab1[i] != tab2[i]) {
                            ret = cmp_uint(tab1[i], tab2[i]);
                            goto done;
                        }
                    }
                    ret = 0;
                } else if (p[1] == ASN1_CSTR_UniversalString) {
                    const uint32_t *tab1 = (const uint32_t *)str1->buf;
                    const uint32_t *tab2 = (const uint32_t *)str2->buf;
                    for(i = 0; i < len; i++) {
                        if (tab1[i] != tab2[i]) {
                            ret = cmp_uint(tab1[i], tab2[i]);
                            goto done;
                        }
                    }
                    ret = 0;
                } else {
                    ret = memcmp(str1->buf, str2->buf, len);
                }
            done: ;
            }
        }
        break;
    default:
        abort();
    }
    return ret;
}

/* copy/cloning */

static int asn1_copy_sequence(const ASN1CType *p, uint8_t *data1,
                              const uint8_t *data2)
{
    int nb_fields, i, flag, ret;
    const ASN1SequenceField *f;
    BOOL present;

    p++;
    nb_fields = *p++;
    p++; /* sizeof(struct) */
    f = (const ASN1SequenceField *)p;
    for(i = 0; i < nb_fields; i++) {
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
            (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
            present = *(BOOL *)(data2 + f->u.option_offset);
            *(BOOL *)(data1 + f->u.option_offset) = present;
        } else {
            present = 1;
        }
        if (present) {
            ret = asn1_copy_value(f->type, data1 + ASN1_GET_SEQ_OFFSET(f),
                                  data2 + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
        }
        f++;
    }
    return 0;
}

static int asn1_copy_sequence_of(const ASN1CType *p, uint8_t *data1,
                                 const uint8_t *data2)
{
    const ASN1SequenceOfCType *f;
    int flags, ret;
    uint32_t i;
    ASN1String *str1;
    const ASN1String *str2;

    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_LOW)
        p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        p++;
    f = (const ASN1SequenceOfCType *)p;
    str1 = (ASN1String *)data1;
    str2 = (const ASN1String *)data2;
    str1->len = str2->len;
    str1->buf = asn1_mallocz2(f->elem_size, str2->len);
    if (!str1->buf)
        return -1;
    for(i = 0; i < str2->len; i++) {
        ret = asn1_copy_value(f->type, str1->buf + i * f->elem_size,
                              str2->buf + i * f->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_copy_choice(const ASN1CType *p, uint8_t *data1,
                            const uint8_t *data2)
{
    int flags, nb_fields;
    uint32_t val, choice_offset, data_offset;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++; /* sizeof(struct) */
    choice_offset = p[0];
    data_offset = p[1];
    p += 2;
    val = *(uint32_t *)(data2 + choice_offset);
    *(uint32_t *)(data1 + choice_offset) = val;
    if (val >= nb_fields)
        return 0;
    f = (const ASN1ChoiceField *)p + val;
    return asn1_copy_value(f->type, data1 + data_offset, data2 + data_offset);
}

static int asn1_copy_string(ASN1String *str1,
                            const ASN1String *str2)
{
    int len = str2->len;
    str1->len = len;
    str1->buf = asn1_malloc(len);
    if (!str1->buf)
        return -1;
    memcpy(str1->buf, str2->buf, len);
    return 0;
}

/* Copy data2 to data1 assuming data1 is allocated. Return 0 if OK, < 0
   if error. */
int asn1_copy_value(const ASN1CType *p, void *data1, const void *data2)
{
    int type, flags, ret;

    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
#if 0
    printf("clone: type=%s\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?");
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_copy_sequence(p, data1, data2);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_copy_sequence_of(p, data1, data2);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_copy_choice(p, data1, data2);
        break;
    case ASN1_CTYPE_BOOLEAN:
        *(BOOL *)data1 = *(BOOL *)data2;
        ret = 0;
        break;
    case ASN1_CTYPE_ENUMERATED:
        *(int *)data1 = *(int *)data2;
        ret = 0;
        break;
    case ASN1_CTYPE_INTEGER:
        if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
            ASN1Integer *aint1 = (ASN1Integer *)data1;
            const ASN1Integer *aint2 = (const ASN1Integer *)data2;
            asn1_integer_init(aint1);
            ret = asn1_integer_set(aint1, aint2);
#else
            abort();
#endif
        } else {
            *(int *)data1 = *(int *)data2;
            ret = 0;
        }
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_copy_string((ASN1String *)data1,
                               (const ASN1String *)data2);
        break;
    case ASN1_CTYPE_ANY:
        {
            ASN1OpenType *str1 = (ASN1OpenType *)data1;
            const ASN1OpenType *str2 = (const ASN1OpenType *)data2;
            str1->type = str2->type;
            if (str2->type) {
                str1->u.data = asn1_clone_value(str2->type, str2->u.data);
                ret = 0;
            } else {
                ret = asn1_copy_string(&str1->u.octet_string,
                                       &str2->u.octet_string);
            }
        }
        break;
    case ASN1_CTYPE_BIT_STRING:
        {
            ASN1BitString *str1 = (ASN1BitString *)data1;
            const ASN1BitString *str2 = (const ASN1BitString *)data2;
            str1->len = str2->len;
            str1->buf = asn1_malloc((unsigned)(str2->len + 7) / 8);
            if (!str1->buf)
                return -1;
            memcpy(str1->buf, str2->buf, (unsigned)(str2->len + 7) / 8);
            ret = 0;
        }
        break;
    case ASN1_CTYPE_TAGGED:
        {
            if (flags & ASN1_CTYPE_HAS_POINTER) {
                *(void **)data1 = asn1_clone_value((ASN1CType *)p[1], 
                                                   *(void **)data2);
                ret = 0;
            } else {
                ret = asn1_copy_value((ASN1CType *)p[1], data1, data2);
            }
        }
        break;
    case ASN1_CTYPE_REAL:
        {
            *(double *)data1 = *(double *)data2;
            ret = 0;
        }
        break;
    case ASN1_CTYPE_CHAR_STRING:
        {
            ASN1String *str1 = (ASN1String *)data1;
            ASN1String *str2 = (ASN1String *)data2;
            int len, size;
            len = str2->len;
            str1->len = len;
            if (p[1] == ASN1_CSTR_BMPString) {
                size = len << 1;
            } else if (p[1] == ASN1_CSTR_UniversalString) {
                size = len << 2;
            } else {
                size = len;
            }
            str1->buf = asn1_malloc(size);
            if (!str1->buf)
                return -1;
            memcpy(str1->buf, str2->buf, size);
            ret = 0;
        }
        break;
    default:
        abort();
    }
    return ret;
}

/* Clone the value 'data' of type 'p'. return NULL if error */
void *asn1_clone_value(const ASN1CType *p, const void *data)
{
    void *dst;
    dst = asn1_mallocz_value(p);
    if (!dst)
        return NULL;
    if (asn1_copy_value(p, dst, data) != 0) {
        asn1_free_value(p, dst);
        return NULL;
    }
    return dst;
}

asn1_exception int asn1_sequence_set_default_value(void *data, const ASN1SequenceField *f)
{
    ASN1CType *p = f->type;
    int flags, type;
    flags = p[0];

    data = (uint8_t *)data + ASN1_GET_SEQ_OFFSET(f);
    /* Specific handling for large integers */
    type = ASN1_GET_CTYPE(flags);
    if (type == ASN1_CTYPE_INTEGER && (flags & ASN1_CTYPE_HAS_LARGE)) {
#ifdef ASN1_USE_LARGE_INTEGER
        ASN1Integer *ai = data;
        int32_t v;
        asn1_integer_init(ai);
        /* XXX: we assume it is signed, should handle any large
           integer value */
        v = f->u.default_value;
        if (asn1_integer_set_si(ai, v))
            return -1;
#else
        abort();
#endif
    } else {
        *(uint32_t *)data = f->u.default_value;
    }
    return 0;
}

/* initialize the default fields which is not marked as present in
   'table_present' */
asn1_exception int asn1_sequence_set_default_fields(const ASN1SequenceField *f1,
                                                    int nb_fields, void *data,
                                                    const uint8_t *table_present)
{
    int i, flag;
    const ASN1SequenceField *f;

    for(i = 0; i < nb_fields; i++) {
        f = f1 + i;
        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_DEFAULT && !table_present[i]) {
            if (asn1_sequence_set_default_value(data, f))
                return -1;
        }
    }
    return 0;
}

/* check if mandatory fields are present. The check is more
   complicated for extensions */
int asn1_sequence_check_fields(const ASN1SequenceField *f1, int nb_fields, 
                               const uint8_t *table_present, 
                               char *msg, int msg_size)
{
    int i, flag, extension_forbidden, group_present, nb_group_fields, j;
    int present;
    const ASN1SequenceField *f;

    extension_forbidden = 0;
    for(i = 0; i < nb_fields;) {
        f = f1 + i;
        if (!ASN1_IS_SEQ_EXT(f)) {
            flag = ASN1_GET_SEQ_FLAG(f);
            if (flag == ASN1_SEQ_FLAG_NORMAL && 
                !table_present[i]) {
                snprintf(msg, msg_size, "component '%s' must be present",
                         f->name);
                return -1;
            }
            i++;
        } else {
            if (ASN1_IS_SEQ_EXT_GROUP_START(f)) {
                nb_group_fields = 0;
                for(;;) {
                    f = f1 + i + nb_group_fields;
                    group_present |= table_present[i];
                    nb_group_fields++;
                    if (ASN1_IS_SEQ_EXT_GROUP_END(f))
                        break;
                }

                group_present = 0;
                for(j = 0; j < nb_group_fields; j++) {
                    f = f1 + i + j;
                    present = table_present[i + j];
                    group_present |= present;
                    if (extension_forbidden && present) 
                        goto forbidden_ext;
                }
                
                if (group_present) {
                    /* all mandatory fields must be present */
                    for(j = 0; j < nb_group_fields; j++) {
                        f = f1 + i + j;
                        flag = ASN1_GET_SEQ_FLAG(f);
                        if (flag == ASN1_SEQ_FLAG_NORMAL && 
                            !table_present[i + j]) {
                            snprintf(msg, msg_size, "extension group component '%s' must be present", f->name);
                            return -1;
                        }
                    }
                } else {
                    /* group not present, so following extensions are
                       forbidden if there was a mandatory field */
                    for(j = 0; j < nb_group_fields; j++) {
                        f = f1 + i + j;
                        flag = ASN1_GET_SEQ_FLAG(f);
                        if (flag == ASN1_SEQ_FLAG_NORMAL) {
                            extension_forbidden = TRUE;
                            break;
                        }
                    }
                }
                i += nb_group_fields;
            } else {
                if (extension_forbidden && table_present[i]) {
                forbidden_ext:
                    snprintf(msg, msg_size, "extension '%s' cannot be present because a previous mandatory extension was absent", f->name);
                    return -1;
                }
                flag = ASN1_GET_SEQ_FLAG(f);
                /* this extension should be present and it is not, so
                   no following extension can be present */
                if (flag == ASN1_SEQ_FLAG_NORMAL && !table_present[i])
                    extension_forbidden = TRUE;
                i++;
            }
        }
    }
    return 0;
}

/* Note: at most 31 bits are encoded. At most 6 bytes are output. */
int asn1_to_utf8(uint8_t *buf, unsigned int c)
{
    uint8_t *q = buf;

    if (c < 0x80) {
        *q++ = c;
    } else {
        if (c < 0x800) {
            *q++ = (c >> 6) | 0xc0;
        } else {
            if (c < 0x10000) {
                *q++ = (c >> 12) | 0xe0;
            } else {
                if (c < 0x00200000) {
                    *q++ = (c >> 18) | 0xf0;
                } else {
                    if (c < 0x04000000) {
                        *q++ = (c >> 24) | 0xf8;
                    } else if (c < 0x80000000) {
                        *q++ = (c >> 30) | 0xfc;
                        *q++ = ((c >> 24) & 0x3f) | 0x80;
                    } else {
                        return 0;
                    }
                    *q++ = ((c >> 18) & 0x3f) | 0x80;
                }
                *q++ = ((c >> 12) & 0x3f) | 0x80;
            }
            *q++ = ((c >> 6) & 0x3f) | 0x80;
        }
        *q++ = (c & 0x3f) | 0x80;
    }
    return q - buf;
}

static const unsigned int utf8_min_code[5] = {
    0x80, 0x800, 0x10000, 0x00200000, 0x04000000,
};

static const unsigned char utf8_first_code_mask[5] = {
    0x1f, 0xf, 0x7, 0x3, 0x1,
};

/* return -1 if error. max_len must be >= 1. The maximum length for an
   UTF8 character is 6 bytes. */
int asn1_from_utf8(const uint8_t *p, int max_len, const uint8_t **pp)
{
    int l, c, b, i;

    c = *p++;
    if (c < 0x80) {
        *pp = p;
        return c;
    }
    switch(c) {
    case 0xc0 ... 0xdf:
        l = 1;
        break;
    case 0xe0 ... 0xef:
        l = 2;
        break;
    case 0xf0 ... 0xf7:
        l = 3;
        break;
    case 0xf8 ... 0xfb:
        l = 4;
        break;
    case 0xfc ... 0xfd:
        l = 5;
        break;
    default:
        return -1;
    }
    /* check that we have enough characters */
    if (l > (max_len - 1))
        return -1;
    c &= utf8_first_code_mask[l - 1];
    for(i = 0; i < l; i++) {
        b = *p++;
        if (b < 0x80 || b > 0xc0)
            return -1;
        c = (c << 6) | (b & 0x3f);
    }
    if (c < utf8_min_code[l - 1])
        return -1;
    *pp = p;
    return c;
}

int asn1_encode_real_der(uint8_t *buf, double d)
{
    union {
        double d;
        uint64_t v;
    } u;
    int sg, e, e_len, mant_len, i, F;
    uint64_t mant, v;
    uint8_t *q;

    u.d = d;
    mant = u.v & (((uint64_t)1 << 52) - 1);
    e = (u.v >> 52) & 0x7ff;
    sg = u.v >> 63;

    q = buf;
    if (e == 0 && mant == 0) {
        /* zero */
        if (sg)
            *q++ = 0x43;
    } else if (e == 0x7ff) {
        if (mant == 0) {
            if (sg)
                *q++ = 0x41; /* -inf */
            else
                *q++ = 0x40; /* +inf */
        } else {
            *q++ = 0x42; /* NaN */
        }
    } else {
        /* subnormal has no leading 1 */
        if (e != 0)
            mant |= (uint64_t)1 << 52;
        else
            e = 1;
        e = e - 1023 - 52;
        /* DER encoding: normalize mantissa */
        while ((mant & 1) == 0) {
            mant >>= 1;
            e++;
        }

        if ((int8_t)e == e)
            e_len = 1;
        else
            e_len = 2;
        /* base 2 encoding */
        F = 0;
        *q++ = 0x80 | (sg << 6) | (F << 2) | (e_len - 1); 
        if (e_len == 1) {
            *q++ = e;
        } else {
            *q++ = (uint32_t)e >> 8;
            *q++ = e;
        }
        mant_len = 0;
        v = mant;
        do {
            mant_len++;
            v >>= 8;
        } while (v != 0);
        for(i = mant_len - 1; i >= 0; i--)
            *q++ = mant >> (i * 8);
    }
    return q - buf;
}

int asn1_decode_real_ber(const uint8_t *buf, int len, double *pd)
{
    const uint8_t *p;
    double d;
    int e, h, i;

    if (len == 0) {
        /* zero */
        d = 0.0;
    } else {
        p = buf;
        h = *p++;
        len--;
        if ((h & 0xc0) == 0x40) {
            /* special values */
            switch(h) {
            case 0x40: /* +inf */
                d = INFINITY;
                break;
            case 0x41: /* -inf */
                d = -INFINITY;
                break;
            case 0x42: /* NAN */
                d = NAN;
                break;
            case 0x43: /* -0 */
                d = -0.0;
                break;
            default:
                return -1;
            }
        } else if ((h & 0xc0) == 0) {
            char buf1[64];
            /* base 10 */
            /* XXX: check */
            if (len >= sizeof(buf1) - 1) 
                return -1;
            memcpy(buf1, p, len);
            buf1[len] = '\0';
            errno = 0;
            d = strtod(buf1, NULL);
            if (errno == ERANGE)
                return -1;
        } else {
            /* base 2^n */
            int log2_base, F, e_len, shift, sg;

            switch((h >> 4) & 3) {
            case 0: 
                log2_base = 1;
                break;
            case 1: 
                log2_base = 3;
                break;
            case 2: 
                log2_base = 4;
                break;
            default:
                return -1;
            }
            F = (h >> 2) & 3;
            sg = (h >> 6) & 1;
            e_len = (h & 3) + 1;
            if (e_len == 4) {
                if (len == 0)
                    return -1;
                e_len = *p++;
                len--;
            }
            if (len < e_len || e_len > 4)
                return -1;
            e = 0;
            for(i = 0; i < e_len; i++) {
                e = (e << 8) | *p++;
            }
            shift = 32 - 8 * e_len;
            e = (e << shift) >> shift;
            len -= e_len;
            
            e = e * log2_base + F;
            /* XXX: precision loss */
            d = 0;
            while (len != 0) {
                d = ldexp(d, 8) + *p++;
                len--;
            }
            d = ldexp(d, e);
            if (sg)
                d = -d;
        }
    }
    *pd = d;
    return 0;
}

/* find enum index by dichotomy */
static int find_enum_index(const ASN1CType *p, int nb_fields, int val)
{
    int a, b, m, v;

    a = 0;
    b = nb_fields - 1;
    while (a <= b) {
        m = (a + b) >> 1;
        v = p[2 * m];
        if (v == val) {
            return p[2 * m + 1];
        } else if (v < val) {
            a = m + 1;
        } else {
            b = m - 1;
        }
    }
    return -1;
}

/* return < 0 if error. 'p' must point to the first field name in the
   enumerated description. */
int asn1_find_enum_index(const ASN1CType *p, int nb_fields, int val, int flags)
{
    if (flags & ASN1_CTYPE_HAS_CONV_TABLE) {
        /* slower case: need conversion table */
        p += nb_fields;
        if (flags & ASN1_CTYPE_HAS_NAME)
            p++;
        if (flags & ASN1_CTYPE_HAS_CONSTRAINT)
            p++;
        p += nb_fields;
        val = find_enum_index(p, nb_fields, val);
        if (val < 0)
            return -1;
    } else {
        if (val < 0 || val >= nb_fields)
            return -1;
    }
    return val;
}

/*****************************************************************/
/* Large integer support */

#ifdef ASN1_USE_LARGE_INTEGER

static int mp_cmp(const ASN1Limb *taba, const ASN1Limb *tabb, int len)
{
    int i;
    
    i = len - 1;
    while (i >= 0) {
        if (taba[i] < tabb[i]) {
            return -1;
        } else if (taba[i] > tabb[i]) {
            return 1;
        }
        i--;
    }
    return 0;
}

static ASN1Limb mp_add(ASN1Limb *res, const ASN1Limb *op1, const ASN1Limb *op2, 
                       int n, ASN1Limb carry)
{
    int i;
    ASN1Limb k, a, v, k1;
    
    k=carry;
    for(i=0;i<n;i++) {
        v = op1[i];
        a = v + op2[i];
        k1 = a < v;
        a = a + k;
        k = (a < k) | k1;
        res[i] = a;
    }
    return k;
}

static ASN1Limb mp_add_ui(ASN1Limb *tab, ASN1Limb b, int n)
{
    int i;
    ASN1Limb k, a;

    k=b;
    for(i=0;i<n;i++) {
        a = tab[i] + k;
        k = (a < k);
        tab[i] = a;
        if (k == 0)
            break;
    }
    return k;
}

static ASN1Limb mp_sub(ASN1Limb *res, const ASN1Limb *op1, const ASN1Limb *op2, 
                       int n, ASN1Limb carry)
{
    int i;
    ASN1Limb k, a, v, k1;
    
    k=carry;
    for(i=0;i<n;i++) {
        v = op1[i];
        a = v - op2[i];
        k1 = a > v;
        v = a - k;
        k = (v > a) | k1;
        res[i] = v;
    }
    return k;
}

static ASN1Limb mp_sub_ui(ASN1Limb *tab, ASN1Limb b, int n)
{
    int i;
    ASN1Limb k, a, v;
    
    k=b;
    for(i=0;i<n;i++) {
        v = tab[i];
        a = v - k;
        k = a > v;
        tab[i]=a;
        if (k == 0)
            break;
    }
    return k;
}

/* taba[] = taba[] * b + l. Return the high carry */
static ASN1Limb mp_mul1(ASN1Limb *tabr, const ASN1Limb *taba, int n, 
                        ASN1Limb b, ASN1Limb l)
{
    int i;
    uint64_t v;

    for(i = 0; i < n; i++) {
        v = (uint64_t)taba[i] * (uint64_t)b + l;
        tabr[i] = v;
        l = v >> 32;
    }
    return l;
}

static ASN1Limb mp_div1(ASN1Limb *tabr, const ASN1Limb *taba, int na, 
                      ASN1Limb b, ASN1Limb r)
{
    int i;
    uint64_t v;
    ASN1Limb q;
    
    for(i = na - 1; i >= 0; i--) {
        v = taba[i] | ((uint64_t)r << 32);
        q = v / b;
        r = v % b;
        tabr[i] = q;
    }
    return r;
}

void asn1_integer_init(ASN1Integer *r)
{
    r->data = NULL;
    r->len = 0;
    r->allocated_len = 0;
    r->negative = 0;
}

void asn1_integer_clear(ASN1Integer *r)
{
    asn1_free(r->data);
    r->data = NULL; /* fail safe */
}

void asn1_integer_renorm(ASN1Integer *r)
{
    while (r->len > 0 && r->data[r->len - 1] == 0)
        r->len--;
}

asn1_exception int asn1_integer_resize(ASN1Integer *r, uint32_t len)
{
    uint32_t new_size;
    ASN1Limb *new_data;
    
    if (unlikely(len > r->allocated_len)) {
        new_size = r->allocated_len + (r->allocated_len / 2);
        if (len > new_size)
            new_size = len;
        /* ensure that the number of bits fits a signed 32 bit integer */
        if (new_size > (INT32_MAX / (sizeof(ASN1Limb) * 8)))
            return -1;
        new_data = asn1_realloc2(r->data, new_size, sizeof(ASN1Limb));
        if (!new_data)
            return -1;
        r->data = new_data;
        r->allocated_len = new_size;
    }
    r->len = len;
    return 0;
}

static inline asn1_exception int asn1_integer_resize_sign(ASN1Integer *r,
                                                          uint32_t len, int neg)
{
    if (asn1_integer_resize(r, len))
        return -1;
    r->negative = neg;
    return 0;
}

asn1_exception int asn1_integer_set(ASN1Integer *r, const ASN1Integer *a)
{
    if (unlikely(r == a))
        return 0;
    if (asn1_integer_resize(r, a->len))
        return -1;
    memcpy(r->data, a->data, a->len * sizeof(ASN1Limb));
    r->negative = a->negative;
    return 0;
}

asn1_exception int asn1_integer_set_ui(ASN1Integer *r, uint32_t v)
{
    if (v == 0) {
        if (asn1_integer_resize(r, 0))
            return -1;
    } else {
        if (asn1_integer_resize(r, 1))
            return -1;
        r->data[0] = v;
    }
    r->negative = 0;
    return 0;
}

asn1_exception int asn1_integer_set_ui64(ASN1Integer *r, uint64_t v)
{
    if (v == 0) {
        if (asn1_integer_resize(r, 0))
            return -1;
    } else if (v <= 0xffffffff) {
        if (asn1_integer_resize(r, 1))
            return -1;
        r->data[0] = v;
    } else {
        if (asn1_integer_resize(r, 2))
            return -1;
        r->data[0] = v;
        r->data[1] = v >> 32;
    }
    r->negative = 0;
    return 0;
}

asn1_exception int asn1_integer_set_si(ASN1Integer *r, int32_t v)
{
    if (v < 0) {
        if (asn1_integer_set_ui(r, -v))
            return -1;
        r->negative = 1;
    } else {
        if (asn1_integer_set_ui(r, v))
            return -1;
    }
    return 0;
}

asn1_exception int asn1_integer_set_si64(ASN1Integer *r, int64_t v)
{
    if (v < 0) {
        if (asn1_integer_set_ui64(r, -v))
            return -1;
        r->negative = 1;
    } else {
        if (asn1_integer_set_ui64(r, v))
            return -1;
    }
    return 0;
}

/* return 0 if OK, -1 if overflow */
int asn1_integer_get_si_ov(const ASN1Integer *r, int32_t *pval)
{
    uint32_t val;
    if (r->len == 0) {
        val = 0;
    } else if (r->len == 1) {
        val = r->data[0];
        if (r->negative) {
            if (val > 0x80000000)
                return -1;
            val = -val;
        } else {
            if (val >= 0x80000000)
                return -1;
        }
    } else {
        return -1;
    }
    *pval = val;
    return 0;
}

int asn1_integer_get_si64_ov(const ASN1Integer *r, int64_t *pval)
{
    uint64_t val;
    if (r->len == 0) {
        val = 0;
    } else if (r->len == 1) {
        val = r->data[0];
        if (r->negative)
            val = -val;
    } else if (r->len == 2) {
        val = r->data[0] | ((uint64_t)r->data[1] << 32);
        if (r->negative) {
            if (val > UINT64_C(0x8000000000000000))
                return -1;
            val = -val;
        } else {
            if (val >= UINT64_C(0x8000000000000000))
                return -1;
        }
    } else {
        return -1;
    }
    *pval = val;
    return 0;
}

static int asn1_integer_cmpabs(const ASN1Integer *a, const ASN1Integer *b)
{
    if (a->len < b->len) {
        return -1;
    } else if (a->len > b->len) {
        return 1;
    } else {
        return mp_cmp(a->data, b->data, a->len);
    }
}

/* return -1 if a < b, 0 if a == b, 1 if a > b */
int asn1_integer_cmp(const ASN1Integer *a, const ASN1Integer *b)
{
    /* warning: negative is not defined for zero, so the handling is a
       little more complicated than expected */
    if (a->len == 0) {
        if (b->len == 0)
            return 0;
        else
            return 2 * b->negative - 1; /* a = 0: -sign of b */
    } else if (b->len == 0) {
        return -2 * a->negative + 1; /* b = 0: sign of a */
    } else if (a->negative != b->negative) {
        return -2 * a->negative + 1;
    } else {
        return asn1_integer_cmpabs(a, b) * (-2 * a->negative + 1);
    }
}

int asn1_integer_cmp_si(const ASN1Integer *a, int32_t b)
{
    ASN1Integer tmp;
    int ret;
    ASN1Limb tab[1];
    
    /* here we avoid a memory allocation */
    asn1_integer_init(&tmp);
    if (b == 0) {
        /* nothing to do */
    } else if (b < 0) {
        tab[0] = -(uint32_t)b;
        tmp.data = tab;
        tmp.len = 1;
        tmp.negative = 1;
    } else {
        tab[0] = b;
        tmp.data = tab;
        tmp.len = 1;
    }
    ret = asn1_integer_cmp(a, &tmp);
    return ret;
}

/* Return < 0 in case of memory allocation error. In this case, the
   content of 'r' is undefined. */
asn1_exception int asn1_integer_add_internal(ASN1Integer *r, const ASN1Integer *a, const ASN1Integer *b, int b_neg)
{
    int carry, ret;
    int a_len, b_len;
    ASN1Integer r_tmp;

    b_neg ^= b->negative;
    if (a->negative == b_neg) {
        /* both operands have same sign: add them */
        /* swap so that a->len >= b->len */
        if (a->len < b->len) {
            const ASN1Integer *tmp;
            tmp = a;
            a = b;
            b = tmp;
        }
        b_len = b->len;
        if (b_len == 0) {
            if (asn1_integer_set(r, a))
                goto fail;
        }
        a_len = a->len;
        if (r == a) {
            /* we use 'a' to store the result */
            r->negative = b_neg;
        } else {
            if (r == b) {
                asn1_integer_init(&r_tmp);
                r = &r_tmp;
            }
            if (asn1_integer_resize_sign(r, a_len, b_neg))
                goto fail;
        }
        carry = mp_add(r->data, a->data, b->data, b_len, 0);
        if (b_len < a_len) {
            if (r != a) {
                memcpy(r->data + b_len, a->data + b_len, 
                       (a_len - b_len) * sizeof(ASN1Limb));
            }
            carry = mp_add_ui(r->data + b_len, carry,
                               a_len - b_len);
        }
        if (carry) {
            if (asn1_integer_resize(r, a_len + 1))
                goto fail;
            r->data[a_len] = carry;
        }
        asn1_integer_renorm(r);
        if (r == &r_tmp) {
            asn1_integer_clear((ASN1Integer *)b);
            *(ASN1Integer *)b = r_tmp;
        }
    } else {
        /* different sign: substract */
        /* swap operands so that a is the biggest */
        ret = asn1_integer_cmpabs(a, b);
        if (ret == 0) {
            if (asn1_integer_set_ui(r, 0))
                goto fail;
        } else {
            if (ret < 0) {
                const ASN1Integer *tmp;
                tmp = a;
                a = b;
                b = tmp;
            } else {
                b_neg = a->negative;
            }
            a_len = a->len;
            b_len = b->len;
            if (r == a) {
                r->negative = b_neg;
            } else {
                if (r == b) {
                    asn1_integer_init(&r_tmp);
                    r = &r_tmp;
                }
                if (asn1_integer_resize_sign(r, a_len, b_neg))
                    goto fail;
            }
            carry = mp_sub(r->data, a->data, b->data,
                           b_len, 0);
            if (b_len < a_len) {
                if (r != a) {
                    memcpy(r->data + b_len, a->data + b_len, 
                           (a_len - b_len) * sizeof(ASN1Limb));
                }
                if (carry) {
                    mp_sub_ui(r->data + b_len, 1, 
                              a_len - b_len);
                }
            }
            asn1_integer_renorm(r);
            if (r == &r_tmp) {
                asn1_integer_clear((ASN1Integer *)b);
                *(ASN1Integer *)b = r_tmp;
            }
        }
    }
    return 0;
 fail:
    if (r == &r_tmp)
        asn1_integer_clear((ASN1Integer *)r);
    return -1;
}

/* return the remainder. */
uint32_t asn1_integer_div_ui(ASN1Integer *r, uint32_t b)
{
    uint32_t l;
    if (r->len == 0)
        return 0;
    l = mp_div1(r->data, r->data, r->len, b, 0);
    asn1_integer_renorm(r);
    return l;
}

asn1_exception int asn1_integer_from_str(ASN1Integer *r, char **pp)
{
    static const uint32_t pow10[10] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
    };
    uint32_t h, v;
    int i, c;
    char *p;

    p = *pp;
    r->negative = 0;
    if (*p == '-') {
        r->negative = 1;
        p++;
    }
    r->len = 0;
    for(;;) {
        c = *p;
        if (c < '0' || c > '9')
            break;
        v = c - '0';
        p++;
        for(i = 1; i < 9; i++) {
            c = *p;
            if (c < '0' || c > '9')
                break;
            v = v * 10 + c - '0';
            p++;
        }
        if (r->len == 0) {
            if (asn1_integer_resize(r, 1))
                return -1;
            r->data[0] = v;
        } else {
            h = mp_mul1(r->data, r->data, r->len, pow10[i], v);
            if (h != 0) {
                if (asn1_integer_resize(r, r->len + 1))
                    return -1;
                r->data[r->len - 1] = h;
            }
        }
    }
    if (r->len == 1 && r->data[0] == 0) {
        r->len = 0;
        r->negative = 0;
    }
    *pp = p;
    return 0;
}

/* return NULL in case of memory allocation error */
char *asn1_integer_to_str(const ASN1Integer *r, char **pbuf, int *plen)
{
    char *buf, *p;
    ASN1Integer tmp;
    uint32_t v;
    int max_size, i;

    if (r->len == 0) {
        buf = asn1_malloc(2);
        if (!buf)
            goto fail;
        p = buf;
        p[0] = '0';
        p[1] = '\0';
        if (plen)
            *plen = 1;
    } else {
        /* overestimate the number of digits */
        max_size = 10 * r->len + 2;
        buf = asn1_malloc(max_size);
        if (!buf)
            goto fail;
        p = buf + max_size;
        *--p = '\0';
        asn1_integer_init(&tmp);
        if (asn1_integer_set(&tmp, r)) {
            asn1_free(buf);
            goto fail;
        }
#if 0
        {
            int i;
            for(i = tmp.len - 1; i >= 0; i--)
                printf(" %08x", r->data[i]);
            printf("\n");
        }
#endif
        while (tmp.len != 0) {
            if (tmp.len == 1) {
                v = tmp.data[0];
                while (v != 0) {
                    *--p = '0' + v % 10;
                    v /= 10;
                }
                break;
            } else {
                v = asn1_integer_div_ui(&tmp, 1000000000);
                for(i = 0; i < 9; i++) {
                    *--p = '0' + v % 10;
                    v /= 10;
                }
            }
        }
        asn1_integer_clear(&tmp);
        if (r->negative)
            *--p = '-';
        if (plen)
            *plen = buf + max_size - 1 - p;
    }
    *pbuf = buf;
    return p;
 fail:
    *pbuf = NULL;
    return NULL;
}

asn1_exception int asn1_integer_out_str(FILE *f, const ASN1Integer *r)
{
    char *p, *buf;
    int len;

    p = asn1_integer_to_str(r, &buf, &len);
    if (!p)
        return -1;
    fwrite(p, 1, len, f);
    asn1_free(buf);
    return 0;
}

/* return the number of bits of the absolute value of r */
int asn1_integer_get_bit_size(const ASN1Integer *r)
{
    if (r->len == 0)
        return 0;
    else
        return (r->len - 1) * ASN1_LIMB_BITS + 32 - clz32(r->data[r->len - 1]);
}

/* return the minimum number of bits to store the integer 'a' in two's
   complement notation. Return 0 if OK, < 0 if memory allocation error. */
/* XXX: could avoid the memory allocation */
asn1_exception int asn1_integer_get_bit_size_2comp(int *pnb_bits,
                                                   const ASN1Integer *a)
{
    int nb_bits;
    if (asn1_integer_cmp_si(a, 0) >= 0) {
        nb_bits = asn1_integer_get_bit_size(a) + 1;
    } else {
        ASN1Integer q_s, *q = &q_s;
        asn1_integer_init(q);
        if (asn1_integer_set_ui(q, 1))
            goto fail;
        if (asn1_integer_sub(q, a, q)) {
        fail:
            asn1_integer_clear(q);
            return -1;
        }
        nb_bits = asn1_integer_get_bit_size(q) + 1;
        asn1_integer_clear(q);
    }
    *pnb_bits = nb_bits;
    return 0;
}

/* convert from two complement notation to internal representation */
asn1_exception int asn1_integer_from_2comp(ASN1Integer *r)
{
    int len, sg, i;
    ASN1Limb carry;

    len = r->len;
    if (len == 0)
        return 0;
    sg = (int)r->data[len - 1] < 0;
    if (sg) {
        for(i = 0; i < len; i++)
            r->data[i] = ~r->data[i];
        carry = mp_add_ui(r->data, 1, len);
        if (carry) {
            if (asn1_integer_resize(r, len + 1))
                return -1;
            r->data[len] = carry;
        }
    }
    r->negative = sg;
    asn1_integer_renorm(r);
    return 0;
}

/* return NULL in case of memory allocation error */
ASN1Integer *asn1_integer_to_2comp(ASN1Integer *tmp, const ASN1Integer *r,
                                   int *pnb_bits)
{
    ASN1Integer *q;
    int i;

    if (r->negative) {
        /* negative number case: neg(a) = not(a-1) using 2
           complement notation. */
        q = tmp;
        asn1_integer_init(q);
        if (asn1_integer_set(q, r))
            return NULL;
        q->negative = 0;
        mp_sub_ui(q->data, 1, q->len);
        asn1_integer_renorm(q);
        *pnb_bits = asn1_integer_get_bit_size(q);
        for(i = 0; i < q->len; i++) {
            q->data[i] = ~q->data[i];
        }
    } else {
        q = (ASN1Integer *)r;
        *pnb_bits = asn1_integer_get_bit_size(q);
    }
    return q;
}

#endif /* ASN1_USE_LARGE_INTEGER */
