/*
 * ASN1 constraint check 
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

/* Return the address of the field 'field_index' of the set/sequence
   of type 'p'. Return NULL if invalid field or if it is not
   present. */
static const ASN1CType *get_sequence_field(void **pdata, 
                                           const ASN1CType *p, 
                                           int field_index, 
                                           void *data)
{
    int flags, type, flag, nb_fields;
    const ASN1SequenceField *f;
    BOOL present;

    flags = *p++;
    type = ASN1_GET_CTYPE(flags);
    if (type != ASN1_CTYPE_SEQUENCE && type != ASN1_CTYPE_SET)
        return NULL;
    nb_fields = *p++;
    p++;
    if (field_index < 0 || field_index >= nb_fields)
        return NULL;
    f = (const ASN1SequenceField *)p + field_index;
    flag = ASN1_GET_SEQ_FLAG(f);
    if (flag == ASN1_SEQ_FLAG_OPTIONAL ||
        (flag == ASN1_SEQ_FLAG_NORMAL && ASN1_IS_SEQ_EXT(f))) {
        present = *(BOOL *)((uint8_t *)data + f->u.option_offset);
    } else {
        present = 1;
    }
    if (!present)
        return NULL;
    *pdata = (uint8_t *)data + ASN1_GET_SEQ_OFFSET(f);
    return f->type;
}

static BOOL check_at_fields(const ASN1CType *class_type,
                            const ASN1ValueStack *vs1,
                            const ASN1CType *atfields,
                            const ASN1CType *obj)
{
    const ASN1CType *p1, *vp;
    void *vdata, *cdata;
    const ASN1ValueStack *vs;
    int i, j, field_index, atfield_index, nb_at_fields, level;
    const ASN1CClassField *f, *class_fields;

    class_fields = (const ASN1CClassField *)(class_type + 2);
    p1 = atfields;
    nb_at_fields = *p1++;
    for(i = 0; i < nb_at_fields; i++) {
        level = *p1++;
        /* go to the right level */
        vs = vs1;
        for(j = 0; j < level; j++) {
            if (!vs)
                return FALSE; /* should never happen */
            vs = vs->prev;
        }
        if (!vs)
            return FALSE; /* should never happen */
        vp = vs->type;
        vdata = vs->data;
        
        for(;;) {
            field_index = p1[0] & 0x7fffffff;
            vp = get_sequence_field(&vdata, vp, field_index, vdata);
            if (!vp)
                return FALSE; /* should never happen */
            if (p1[0] & (1U << 31))
                break;
            p1++;
        }
        p1++;
        atfield_index = *p1++;
        /* vdata is the select field value, vp is its type. cdata is
           the corresponding object set data */
        f = class_fields + atfield_index;
        /* we only supported fixed type values */
        if (ASN1_CCLASS_FIELD_GET_TYPE(f->flags) !=
            ASN1_CCLASS_FIELD_FIXED_TYPE_VALUE)
            return FALSE;
        cdata = (void *)obj[atfield_index];
        if (!cdata) {
            cdata = f->default_value;
        }
        /* compare */
        if (asn1_cmp_value(vp, vdata, cdata) != 0)
            return FALSE;
    }
    /* match ! */
    return TRUE;
}

/* return the constrainted type for an open type value. Return NULL if
   no type found. */
const ASN1CType *asn1_get_constrained_type(const ASN1ValueStack *vs,
                                           const ASN1CType *p)
{
    const ASN1CType *atfields, *type_found;
    const ASN1CType *obj_found, *obj;
    const ASN1CConstraintTable *tc;
    const ASN1CClassField *f;
    int flags, obj_index, obj_size, type, class_nb_fields;

    flags = *p++;
    type = ASN1_GET_CTYPE(flags);
    if (type != ASN1_CTYPE_ANY)
        return NULL;
    if (!(flags & ASN1_CTYPE_HAS_CONSTRAINT))
        return NULL;
    if (flags & ASN1_CTYPE_HAS_NAME) 
        p++;
    tc = (const ASN1CConstraintTable *)*p;
    if (tc->type != ASN1_CCONSTRAINT_TABLE)
        return NULL;
    atfields = (void *)(tc + 1);
#if 0
    printf("%s: class=%s objset_count=%d vs.type=%s\n", 
           __func__, asn1_get_type_name(tc->class_type), tc->objset_count,
           asn1_get_type_name(vs->type));
#endif
    class_nb_fields = tc->class_type[1];
    obj_size = class_nb_fields * sizeof(ASN1CType);

    /* iterate thru the object set and check if the at constraints are
       satisfied */
    obj_found = NULL;
    for(obj_index = 0; obj_index < tc->objset_count; obj_index++) {
        obj = (const ASN1CType *)((uint8_t *)tc->objset + obj_index * obj_size);
        if (check_at_fields(tc->class_type, vs, atfields, obj)) {
            /* more than one object matches: currently we consider it
               does not give a single type, although in theory it
               could give the same type */
            if (obj_found)
                return NULL;
            obj_found = obj;
        }
    }

    if (!obj_found)
        return NULL;
    
    /* find the exact type */
    type_found = (const ASN1CType *)obj_found[tc->class_field_index];
    if (!type_found) {
        /* get the default type */
        f = (const ASN1CClassField *)(tc->class_type + 2);
        f += tc->class_field_index;
        type_found = f->type;
    }
#if 0
    printf("type=%p %s\n", type_found, asn1_get_type_name(type_found));
#endif
    return type_found;
}

typedef struct {
    char msg[1024];
} ASN1CheckState;

static int asn1_check_value(ASN1CheckState *s,
                            const ASN1CType *p, const uint8_t *data);

static int asn1_check_error(ASN1CheckState *s, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(s->msg, sizeof(s->msg), fmt, ap);
    va_end(ap);
    return -1;
}

static int asn1_check_sequence(ASN1CheckState *s, 
                               const ASN1CType *p, const uint8_t *data)
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
            present = *(BOOL *)(data + f->u.option_offset);
        } else {
            present = 1;
        }
        if (present) {
            ret = asn1_check_value(s, f->type, data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
        }
        f++;
    }
    return 0;
}

static int asn1_check_sequence_of(ASN1CheckState *s, 
                                  const ASN1CType *p, const uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    int flags, ret;
    uint32_t i, range_min, range_max;
    const ASN1String *str;

    flags = *p++;
    range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    else
        range_max = UINT32_MAX;
    f = (const ASN1SequenceOfCType *)p;
    str = (ASN1String *)data;
    if (!(flags & ASN1_CTYPE_HAS_EXT)) {
        if (str->len < range_min || str->len > range_max)
            return asn1_check_error(s, "sequence of length not in range %u .. %u", range_min, range_max);
    }
    for(i = 0; i < str->len; i++) {
        ret = asn1_check_value(s, f->type, str->buf + i * f->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static int asn1_check_choice(ASN1CheckState *s, 
                             const ASN1CType *p, const uint8_t *data)
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
    val = *(uint32_t *)(data + choice_offset);
    if (val >= nb_fields) {
        return asn1_check_error(s, "invalid choice index (%d) range: %u .. %u",
                                val, 0, nb_fields - 1);
    }
    f = (const ASN1ChoiceField *)p + val;
    return asn1_check_value(s, f->type, data + data_offset);
}

static int asn1_check_enumerated(ASN1CheckState *s, 
                                 const ASN1CType *p, const uint8_t *data)
{
    int flags;
    uint32_t nb_fields;

    flags = *p++;
    p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    if (*(uint32_t *)data >= nb_fields)
        return asn1_check_error(s, "invalid enumerated value");
    return 0;
}

#ifdef ASN1_USE_LARGE_INTEGER
static int asn1_check_large_integer(ASN1CheckState *s, const ASN1CType *p, 
                                    const uint8_t *data)
{
    const ASN1Integer *val, *range_min, *range_max;
    int flags;

    flags = *p++;
    range_min = range_max = NULL;
    if (!(flags & ASN1_CTYPE_HAS_EXT)) {
        if (flags & ASN1_CTYPE_HAS_LOW) {
            range_min = (const ASN1Integer *)*p++;
        }
        if (flags & ASN1_CTYPE_HAS_HIGH) {
            range_max = (const ASN1Integer *)*p++;
        }
    }
    val = (ASN1Integer *)data;
    if ((range_min && asn1_integer_cmp(val, range_min) < 0) ||
        (range_max && asn1_integer_cmp(val, range_max) > 0)) {
        return asn1_check_error(s, "large integer not in range");
    }
    return 0;
}
#endif

static int asn1_check_integer(ASN1CheckState *s, const ASN1CType *p, 
                              const uint8_t *data)
{
    int range_min, range_max, flags, val;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_LARGE) {
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_check_large_integer(s, p, data);
#else
        return asn1_check_error(s, "large integers not supported");
#endif
    }
    p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        range_min = INT32_MIN;
        range_max = INT32_MAX;
    } else {
        if (flags & ASN1_CTYPE_HAS_LOW)
            range_min = *p++;
        else 
            range_min = INT32_MIN;
        if (flags & ASN1_CTYPE_HAS_HIGH) {
            range_max = *p++;
        } else {
            if (range_min < 0)
                range_max = INT32_MAX;
            else
                range_max = UINT32_MAX;
        }
    }
    val = *(const int *)data;
    if (range_min < 0) {
        if (val < range_min || val > range_max)
            return asn1_check_error(s, "integer range is %d .. %d",
                                    range_min, range_max);
    } else {
        if ((uint32_t)val < (uint32_t)range_min || 
            (uint32_t)val > (uint32_t)range_max)
            return asn1_check_error(s, "integer range is %u .. %u",
                                    range_min, range_max);
    }
    return 0;
}
    
static int asn1_check_octet_string(ASN1CheckState *s, const ASN1CType *p, 
                                   const uint8_t *data)
{
    int flags;
    uint32_t range_min, range_max;
    const ASN1String *str;

    flags = *p++;
    range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    else
        range_max = UINT32_MAX;
    str = (const ASN1String *)data;
    if (!(flags & ASN1_CTYPE_HAS_EXT)) {
        if (str->len < range_min || str->len > range_max)
            return asn1_check_error(s, "bit/octet string length (%u) not in range %u .. %u", str->len, range_min, range_max);
    }
    return 0;
}

static int asn1_check_char_string(ASN1CheckState *s, const ASN1CType *p, 
                                  const uint8_t *data)
{
    int flags, char_string_type;
    const ASN1String *str;

    str = (const ASN1String *)data;

    flags = *p++;
    char_string_type = *p++;
    if (char_string_type > ASN1_CSTR_UniversalString) {
        if (char_string_type == ASN1_CSTR_UTF8String) {
            const uint8_t *r, *r_end;
            int c;

            /* check UTF8 string contents */
            r = str->buf;
            r_end = str->buf + str->len;
            while (r < r_end) {
                c = asn1_from_utf8(r, r_end - r, &r);
                if (c < 0) {
                    return asn1_check_error(s, "invalid UTF-8 code");
                }
            }
        }
    } else {
        int shift, nb_ranges, j;
        uint32_t range_min, range_max, len, i, c;
        const uint8_t *buf;

        range_min = *p++;
        if (flags & ASN1_CTYPE_HAS_HIGH)
            range_max = *p++;
        else
            range_max = UINT32_MAX;
        len = str->len;
        if (!(flags & ASN1_CTYPE_HAS_EXT)) {
            if (len < range_min || len > range_max)
                return asn1_check_error(s, "char string length (%u) not in range %u .. %u", len, range_min, range_max);
        }
        if (char_string_type == ASN1_CSTR_BMPString)
            shift = 1;
        else if (char_string_type == ASN1_CSTR_UniversalString)
            shift = 2;
        else
            shift = 0;
        nb_ranges = *p++;
        
        if (nb_ranges > 0) {
            buf = str->buf;
            for(i = 0; i < len; i++) {
                if (shift == 0)
                    c = ((uint8_t *)buf)[i];
                else if (shift == 1)
                    c = ((uint16_t *)buf)[i];
                else
                    c = ((uint32_t *)buf)[i];
                for(j = 0; j < nb_ranges; j++) {
                    if (c >= p[2 * j] && c <= p[2 * j + 1])
                        goto found;
                }
                return asn1_check_error(s, "character at position %u is not allowed", i);
            found: ;
            }
        }
    }
    return 0;
}
    
/* return TRUE if the constraints are satisfied. */
static int asn1_check_value(ASN1CheckState *s,
                            const ASN1CType *p, const uint8_t *data)
{
    int flags, type, ret;

    flags = p[0];
    type = ASN1_GET_CTYPE(flags);
    //    printf("type=%s\n", asn1_get_type_name(p));
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_check_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_check_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_check_choice(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = 0;
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_check_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_check_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_check_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = 0;
        break;
    case ASN1_CTYPE_ANY:
        ret = 0;
        break;
    case ASN1_CTYPE_TAGGED:
        {
            if (flags & ASN1_CTYPE_HAS_POINTER) {
                data = *(void **)data;
            }
            ret = asn1_check_value(s, (ASN1CType *)p[1], data);
        }
        break;
    case ASN1_CTYPE_REAL:
        ret = 0;
        break;
    case ASN1_CTYPE_CHAR_STRING:
        ret = asn1_check_char_string(s, p, data);
        break;
    default:
        abort();
    }
    return ret;
}

BOOL asn1_check_constraints(const ASN1CType *p, const void *data,
                            char *msg_buf, size_t msg_buf_size)
{
    ASN1CheckState s_s, *s = &s_s;
    int ret;

    s->msg[0] = '\0';

    ret = asn1_check_value(s, p, data);
    if (ret < 0) {
        asn1_pstrcpy(msg_buf, msg_buf_size, s->msg);
        return FALSE;
    } else {
        return TRUE;
    }
}
