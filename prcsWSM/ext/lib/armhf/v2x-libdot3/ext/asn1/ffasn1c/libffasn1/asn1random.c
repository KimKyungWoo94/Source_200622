/* generate random values in ASN1 fields, for testing */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "asn1defs_int.h"

//#define DEBUG

typedef struct {
    uint32_t m_w;
    uint32_t m_z;
} ASN1RNDState;

static uint32_t asn1_rnd32(ASN1RNDState *s)
{
    uint32_t m_w, m_z, ret;

    /* George Marsaglia simple random generator */
    m_w = s->m_w;
    m_z = s->m_z;
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    s->m_w = m_w;
    s->m_z = m_z;
    ret = (m_z << 16) + m_w;
    return ret;
}

static ASN1RNDState *asn1_rnd_init(uint32_t seed)
{
    ASN1RNDState *s;
    s = asn1_malloc(sizeof(ASN1RNDState));
    if (!s)
        return NULL;
    s->m_w = seed;
    s->m_z = seed * 314159 + 1;
    if (!s->m_w)
        s->m_w = 1;
    if (!s->m_z)
        s->m_z = 1;
    return s;
}

static void asn1_rnd_end(ASN1RNDState *s)
{
    asn1_free(s);
}

typedef struct ASN1RandomState {
    ASN1RNDState *rnd;
    uint32_t sequence_of_len_max;
    uint32_t octet_string_len_max;
    uint32_t bit_string_len_max; /* in bits */
    uint32_t large_integer_len_max; /* in bits */
    BOOL sequence_extensions;
} ASN1RandomState;

static int asn1_random_type(ASN1RandomState *s, const ASN1CType *p, 
                            uint8_t *data);

static int asn1_random_sequence(ASN1RandomState *s, const ASN1CType *p, 
                                uint8_t *data)
{
    int flag, nb_fields, i, nb_exts, nb_exts_present;
    int present, ret;
    const ASN1SequenceField *f;
    BOOL in_ext_group;

    p++;
    nb_fields = *p++;
    p++;

    /* compute the number of extensions so that we can decide up to
       which extension we go */
    nb_exts = 0;
    in_ext_group = FALSE;
    for(i = 0; i < nb_fields; i++) {
        f = (const ASN1SequenceField *)p + i;
        if (ASN1_IS_SEQ_EXT(f)) {
            if (in_ext_group) {
                if (ASN1_IS_SEQ_EXT_GROUP_END(f)) 
                    in_ext_group = FALSE;
            } else {
                nb_exts++;
                if (ASN1_IS_SEQ_EXT_GROUP_START(f))
                    in_ext_group = TRUE;
            }
        }
    }
    if (nb_exts > 0 && s->sequence_extensions) {
        nb_exts_present = asn1_rnd32(s->rnd) % (nb_exts + 1);
    } else {
        nb_exts_present = 0;
    }
#ifdef DEBUG
    printf("seq ext: %d/%d\n", nb_exts_present, nb_exts);
#endif

    in_ext_group = FALSE;
    nb_exts = 0;
    for(i = 0; i < nb_fields; i++) {
        f = (const ASN1SequenceField *)p + i;

        /* only the first nb_exts_present extensions are present */
        if (ASN1_IS_SEQ_EXT(f)) {
            present = (nb_exts < nb_exts_present);
            if (!in_ext_group) {
                if (ASN1_IS_SEQ_EXT_GROUP_START(f))
                    in_ext_group = TRUE;
                else
                    nb_exts++;
            }
            if (in_ext_group) {
                if (ASN1_IS_SEQ_EXT_GROUP_END(f)) {
                    in_ext_group = FALSE;
                    nb_exts++;
                }
            }
        } else {
            present = 1;
        }

        flag = ASN1_GET_SEQ_FLAG(f);
        if (flag == ASN1_SEQ_FLAG_OPTIONAL) {
            if (present)
                present = asn1_rnd32(s->rnd) & 1;
            *(BOOL *)(data + f->u.option_offset) = present;
        } else if (flag == ASN1_SEQ_FLAG_DEFAULT) {
            if (present)
                present = asn1_rnd32(s->rnd) & 1;
            if (!present) {
                /* set default value if not present */
                *(uint32_t *)(data + ASN1_GET_SEQ_OFFSET(f)) = 
                    f->u.default_value;
            }
        } else {
            if (ASN1_IS_SEQ_EXT(f))
                *(BOOL *)(data + f->u.option_offset) = present;
        }

        if (present) {
#ifdef DEBUG
            printf("seq field=%s flag=%d offset=%d\n", 
                   f->name, 
                   ASN1_GET_SEQ_FLAG(f),
                   ASN1_GET_SEQ_OFFSET(f));
#endif
            ret = asn1_random_type(s, f->type, 
                                   data + ASN1_GET_SEQ_OFFSET(f));
            if (ret)
                return ret;
        }
    }
    return 0;
}

static int asn1_random_sequence_of(ASN1RandomState *s, const ASN1CType *p, 
                                   uint8_t *data)
{
    const ASN1SequenceOfCType *f;
    ASN1String *str;
    uint32_t range_min, range_max, count, diff, i;
    int ret, flags;

    range_min = 0;
    range_max = UINT32_MAX;
    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1) {
            range_min = 0;
            range_max = UINT32_MAX;
        }
    }
    if ((int)range_min < 0)
        return -1;
    f = (const ASN1SequenceOfCType *)p;

    diff = range_max - range_min;
    if (diff > s->sequence_of_len_max)
        diff = s->sequence_of_len_max;
    count = asn1_rnd32(s->rnd) % (diff + 1) + range_min;
#ifdef DEBUG
    printf("seq of len=%u elem_size=%d\n", count, f->elem_size);
#endif
    str = (ASN1String *)data;
    str->len = count;
    str->buf = asn1_mallocz2(f->elem_size, count);
    if (!str->buf)
        return -1;
    for(i = 0; i < count; i++) {
        ret = asn1_random_type(s, f->type, str->buf + i * f->elem_size);
        if (ret)
            return ret;
    }
    return 0;
}

static no_inline int asn1_random_choice(ASN1RandomState *s, const ASN1CType *p, 
                                        uint8_t *data)
{
    int flags, nb_fields, val;
    const ASN1ChoiceField *f;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    p++;
    val = asn1_rnd32(s->rnd) % nb_fields;
    *(uint32_t *)(data + p[0]) = val;
    data += p[1];
    p += 2;
    f = (const ASN1ChoiceField *)p + val;
#ifdef DEBUG
    printf("choice field=%s\n", f->name);
#endif
    return asn1_random_type(s, f->type, data);
}

static int asn1_random_enumerated(ASN1RandomState *s, const ASN1CType *p, 
                                  uint8_t *data)
{
    int flags, nb_fields, val;

    flags = *p++;
    nb_fields = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT)
        nb_fields += *p++;
    val = asn1_rnd32(s->rnd) % nb_fields;
    *(uint32_t *)data = val;
    return 0;
}

static int asn1_random_boolean(ASN1RandomState *s, const ASN1CType *p, 
                               uint8_t *data)
{
    *(BOOL *)data = asn1_rnd32(s->rnd) & 1;
    return 0;
}

static uint32_t random_uint(ASN1RandomState *s, uint32_t max_val)
{
    uint32_t val;
    int k, max_bits;
    
    if (max_val == 0)
        return 0;
    max_bits = 32 - clz32(max_val);
    k = asn1_rnd32(s->rnd) % (max_bits + 1);
    if (k == 0) {
        val = 0;
    } else {
        val = 1 << (k - 1);
        val |= asn1_rnd32(s->rnd) & (val - 1);
        /* XXX: could do better */
        if (val > max_val)
            val = max_val;
    }
    return val;
}

#ifdef ASN1_USE_LARGE_INTEGER
static asn1_exception int random_large_uint(ASN1RandomState *s, ASN1Integer *r,
                                            const ASN1Integer *d)
{
    ASN1Limb val;
    int k, max_bits, len, i, l;
    
    if (!d) {
        max_bits = s->large_integer_len_max;
    } else {
        max_bits = asn1_integer_get_bit_size(d);
    }
    k = asn1_rnd32(s->rnd) % (max_bits + 1);
    if (k == 0) {
        if (asn1_integer_set_ui(r, 0))
            return -1;
    } else {
        len = (k + ASN1_LIMB_BITS - 1) >> ASN1_LIMB_BITS_LOG2;
        if (asn1_integer_resize(r, len))
            return -1;
        for(i = 0; i < len - 1; i++)
            r->data[i] = asn1_rnd32(s->rnd);
        l = k & (ASN1_LIMB_BITS - 1);
        if (l == 0)
            l = ASN1_LIMB_BITS;
        val = 1 << (l - 1);
        val |= asn1_rnd32(s->rnd) & (val - 1);
        r->data[len - 1] = val;

        /* XXX: not random at all */
        if (d && asn1_integer_cmp(r, d) > 0) {
            if (asn1_integer_set(r, d))
                return -1;
        }
    }
    return 0;
}

static no_inline int asn1_random_large_integer(ASN1RandomState *s, const ASN1CType *p, 
                                               uint8_t *data)
{
    uint32_t flags;
    const ASN1Integer *range_min, *range_max;
    ASN1Integer diff, *val;

    val = (ASN1Integer *)data;

    flags = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1)
            goto unconstrainted;
    }
    asn1_integer_init(val);
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        range_min = (const ASN1Integer *)*p++;
        range_max = (const ASN1Integer *)*p++;
        asn1_integer_init(&diff);
        if (asn1_integer_sub(&diff, range_max, range_min)) {
            asn1_integer_clear(&diff);
            return -1;
        }
        if (random_large_uint(s, val, &diff)) {
            asn1_integer_clear(&diff);
            return -1;
        }
        asn1_integer_clear(&diff);
        if (asn1_integer_add(val, val, range_min))
            return -1;
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        range_min = (const ASN1Integer *)*p++;
        if (random_large_uint(s, val, NULL))
            return -1;
        if (asn1_integer_add(val, val, range_min))
            return -1;
    } else if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = (const ASN1Integer *)*p++;
        if (random_large_uint(s, val, NULL))
            return -1;
        if (asn1_integer_sub(val, range_max, val))
            return -1;
    } else {
    unconstrainted:
        if (random_large_uint(s, val, NULL))
            return -1;
        if (asn1_rnd32(s->rnd) & 1)
            val->negative = 1;
    }
    return 0;
}
#endif

static no_inline int asn1_random_integer(ASN1RandomState *s, const ASN1CType *p, 
                                         uint8_t *data)
{
    int range_min, range_max, flags;
    uint32_t val;

    flags = *p;
    if (flags & ASN1_CTYPE_HAS_LARGE) { 
#ifdef ASN1_USE_LARGE_INTEGER
        return asn1_random_large_integer(s, p, data);
#else
        return -1;
#endif
    }
    p++;

    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1)
            goto unconstrainted;
    }
    if ((flags & ASN1_CTYPE_HAS_LOW) && (flags & ASN1_CTYPE_HAS_HIGH)) {
        range_min = *p++;
        range_max = *p++;
        val = random_uint(s, range_max - range_min) + range_min;
    } else if (flags & ASN1_CTYPE_HAS_LOW) {
        range_min = *p++;
        if (range_min < 0)
            range_max = INT32_MAX;
        else
            range_max = UINT32_MAX;
        val = random_uint(s, range_max - range_min) + range_min;
    } else if (flags & ASN1_CTYPE_HAS_HIGH) {
        range_max = *p++;
        val = range_max - random_uint(s, range_max - INT32_MIN);
    } else {
    unconstrainted:
        val = random_uint(s, (uint32_t)INT32_MAX + 1);
        if (asn1_rnd32(s->rnd) & 1)
            val = -val;
    }
    *(uint32_t *)data = val;
    return 0;
}

static no_inline int asn1_random_octet_string1(ASN1RandomState *s, const ASN1CType *p,
                                               uint8_t *data, uint32_t range_min, uint32_t range_max)
{
    uint32_t diff;
    ASN1String *str;
    int len, i;

    diff = range_max - range_min;
    if (diff > s->octet_string_len_max)
        diff = s->octet_string_len_max;
    len = asn1_rnd32(s->rnd) % (diff + 1) + range_min;
#ifdef DEBUG
    printf("octet string: len=%d\n", len);
#endif
    str = (ASN1String *)data;
    str->buf = asn1_malloc(len);
    if (!str->buf)
        return -1;
    str->len = len;
    for(i = 0; i < len; i++)
        str->buf[i] = asn1_rnd32(s->rnd);
    return 0;
}

static no_inline int asn1_random_octet_string(ASN1RandomState *s, const ASN1CType *p,
                                              uint8_t *data)
{
    uint32_t range_min, range_max;
    int flags;

    flags = p[0];
    flags = *p++;
    range_min = 0;
    range_max = UINT32_MAX;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1) {
            range_min = 0;
            range_max = UINT32_MAX;
        }
    }
    return asn1_random_octet_string1(s, p, data, range_min, range_max);
}

static no_inline int asn1_random_bit_string(ASN1RandomState *s, const ASN1CType *p,
                                            uint8_t *data)
{
    uint32_t range_min, range_max, diff;
    ASN1BitString *str;
    int flags, len, i, byte_len, k;

    flags = p[0];
    flags = *p++;
    range_min = 0;
    range_max = UINT32_MAX;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1) {
            range_min = 0;
            range_max = UINT32_MAX;
        }
    }
    diff = range_max - range_min;
    if (diff > s->bit_string_len_max)
        diff = s->bit_string_len_max;
    len = asn1_rnd32(s->rnd) % (diff + 1) + range_min;
    byte_len = (len + 7) / 8;
#ifdef DEBUG
    printf("bit string: len=%d\n", len);
#endif
    str = (ASN1BitString *)data;
    str->buf = asn1_malloc(byte_len);
    if (!str->buf)
        return -1;
    str->len = len;
    for(i = 0; i < byte_len; i++)
        str->buf[i] = asn1_rnd32(s->rnd);
    /* suppress last dummy bits */
    k = len & 7;
    if (k != 0) {
        str->buf[byte_len - 1] &= 0xff00 >> k;
    }
    return 0;
}

static int asn1_random_object_identifier(ASN1RandomState *s, const ASN1CType *p, 
                           uint8_t *data)
{
    ASN1String *str = (ASN1String *)data;
    BOOL is_first, is_relative;
    int val, n, i, v, len, j, range;
    ASN1ByteBuffer bb;

    is_relative = ASN1_GET_CTYPE(p[0]) == ASN1_CTYPE_RELATIVE_OID;
    asn1_byte_buffer_init(&bb);
    len = (asn1_rnd32(s->rnd) % 10) + 1;
    is_first = !is_relative;
    for(j = 0; j < len; j++) {
        if (is_first)
            range = 3 * 40;
        else
            range = 10000;
        val = asn1_rnd32(s->rnd) % range;
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
    if (bb.has_error) {
        asn1_free(bb.buf);
        return -1;
    }
    str->len = bb.len;
    str->buf = bb.buf;
    return 0;
}

static no_inline int asn1_random_real(ASN1RandomState *s, const ASN1CType *p, 
                                      uint8_t *data)
{
    uint64_t mant;
    int e, sg, type;

    type = asn1_rnd32(s->rnd) % 20;
    if (type == 0) {
        /* NaN */
        mant = (uint64_t)1 << 51;
        e = 0x7ff;
        sg = 0;
    } else {
        sg = asn1_rnd32(s->rnd) & 1;
        if (type == 1) {
            /* +0/-0 */
            mant = 0;
            e = 0;
        } else if (type == 2) {
            /* +infinity/-infinity */
            mant = 0;
            e = 0x7ff;
        } else if (type == 3) {
            /* denormal */
            mant = asn1_rnd32(s->rnd) | 
                ((uint64_t)asn1_rnd32(s->rnd) << 32);
            mant &= ((uint64_t)1 << 52) - 1;
            if (mant == 0)
                mant = 1;
            e = 0;
        } else {
            /* normal */
            mant = asn1_rnd32(s->rnd) | 
                ((uint64_t)asn1_rnd32(s->rnd) << 32);
            mant &= ((uint64_t)1 << 52) - 1;
            e = (asn1_rnd32(s->rnd) % 0x7fe) + 1; 
        }
    }
    *(uint64_t *)data = ((uint64_t)sg << 63) | ((uint64_t)e << 52) | mant;
    return 0;
}

static no_inline int asn1_random_restricted_mult_string(ASN1RandomState *s, const ASN1CType *p, 
                                              uint8_t *data)
{
    ASN1String *str;
    int char_string_type, shift;
    uint32_t nb_ranges, char_count, len, char_index, l, c, range_min, range_max;
    uint32_t i, j, diff, flags;
    const ASN1CType *ranges;

    flags = *p++;
    char_string_type = *p++;
    range_min = 0;
    range_max = UINT32_MAX;
    if (flags & ASN1_CTYPE_HAS_LOW)
        range_min = *p++;
    if (flags & ASN1_CTYPE_HAS_HIGH)
        range_max = *p++;
    if (flags & ASN1_CTYPE_HAS_EXT) {
        if (asn1_rnd32(s->rnd) & 1) {
            range_min = 0;
            range_max = UINT32_MAX;
        }
    }
    nb_ranges = *p++;
    ranges = p;
    char_count = 0;
    for(i = 0; i < nb_ranges; i++)
        char_count += ranges[2 * i + 1] - ranges[2 * i] + 1;

    diff = range_max - range_min;
    if (diff > s->octet_string_len_max)
        diff = s->octet_string_len_max;
    len = asn1_rnd32(s->rnd) % (diff + 1) + range_min;

    if (char_string_type == ASN1_CSTR_BMPString)
        shift = 1;
    else if (char_string_type == ASN1_CSTR_UniversalString)
        shift = 2;
    else
        shift = 0;

    str = (ASN1String *)data;
    str->buf = asn1_malloc2(len, 1 << shift);
    if (!str->buf)
        return -1;
    str->len = len;
    for(i = 0; i < len; i++) {

        if (char_count == 0)
            char_index = asn1_rnd32(s->rnd) & INT32_MAX; /* XXX: hack to allow 31 UTF-8 output in text format */
        else
            char_index = asn1_rnd32(s->rnd) % char_count;
        for(j = 0; j < nb_ranges; j++) {
            l = ranges[2 * j + 1] - ranges[2 * j];
            if (char_index <= l) {
                c = ranges[2 * j] + char_index;
                goto found;
            } else {
                char_index -= l + 1;
            }
        }
        abort();
    found:
        if (shift == 0)
            ((uint8_t *)str->buf)[i] = c;
        else if (shift == 1)
            ((uint16_t *)str->buf)[i] = c;
        else
            ((uint32_t *)str->buf)[i] = c;
    }
    return 0;
}

static no_inline int asn1_random_utf8_string(ASN1RandomState *s, const ASN1CType *p,
                                             uint8_t *data)
{
    ASN1String *str;
    int len, i, j, l;
    ASN1ByteBuffer bb;
    uint32_t c;
    uint8_t buf[ASN1_UTF8_MAX_LEN];

    len = asn1_rnd32(s->rnd) % (s->octet_string_len_max + 1);
#ifdef DEBUG
    printf("octet string: len=%d\n", len);
#endif
    asn1_byte_buffer_init(&bb);
    for(i = 0; i < len; i++) {
        c = random_uint(s, 0x7fffffff);
        l = asn1_to_utf8(buf, c);
        for(j = 0; j < l; j++)
            asn1_put_byte(&bb, buf[j]);
    }
    if (bb.has_error) {
        asn1_free(bb.buf);
        return -1;
    }
    str = (ASN1String *)data;
    str->buf = bb.buf;
    str->len = bb.len;
    return 0;
}

static no_inline int asn1_random_any(ASN1RandomState *s, const ASN1CType *p,
                                     uint8_t *data)
{
    ASN1OpenType *str;
    int len, i;
    ASN1ByteBuffer bb;

    /* Note: we put a single octet string tag inside a constructed tag
       build more complicated sequence */
    asn1_byte_buffer_init(&bb);
    
    len = asn1_rnd32(s->rnd) % 100;
    asn1_put_byte(&bb, 4); /* octet string */
    asn1_put_byte(&bb, len);
    for(i = 0; i < len; i++)
        asn1_put_byte(&bb, asn1_rnd32(s->rnd) & 0xff);
    if (bb.has_error) {
        asn1_free(bb.buf);
        return -1;
    }
    str = (ASN1OpenType *)data;
    str->type = NULL; /* currently no type */
    str->u.octet_string.len = bb.len;
    str->u.octet_string.buf = bb.buf;
    return 0;
}

static int asn1_random_tagged(ASN1RandomState *s, const ASN1CType *p, 
                              uint8_t *data)
{
    uint8_t *data1;
    int flags;
    flags = p[0];
    if (flags & ASN1_CTYPE_HAS_POINTER) {
        data1 = asn1_mallocz_value((ASN1CType *)p[1]);
        if (!data1)
            return -1;
        *(void **)data = data1;
    } else {
        data1 = data;
    }
    return asn1_random_type(s, (ASN1CType *)p[1], data1);
}

static int asn1_random_type(ASN1RandomState *s, const ASN1CType *p, 
                            uint8_t *data)
{
    int type, ret;

    type = ASN1_GET_CTYPE(p[0]);
#ifdef DEBUG
    printf("type=%s tag=0x%x\n", 
           type < ASN1_CTYPE_COUNT ? asn1_ctype_names[type] : "?",
           ASN1_GET_TAG(p[0]));
#endif
    switch(type) {
    case ASN1_CTYPE_SEQUENCE:
    case ASN1_CTYPE_SET:
        ret = asn1_random_sequence(s, p, data);
        break;
    case ASN1_CTYPE_SEQUENCE_OF:
    case ASN1_CTYPE_SET_OF:
        ret = asn1_random_sequence_of(s, p, data);
        break;
    case ASN1_CTYPE_CHOICE:
        ret = asn1_random_choice(s, p, data);
        break;
    case ASN1_CTYPE_ENUMERATED:
        ret = asn1_random_enumerated(s, p, data);
        break;
    case ASN1_CTYPE_BOOLEAN:
        ret = asn1_random_boolean(s, p, data);
        break;
    case ASN1_CTYPE_INTEGER:
        ret = asn1_random_integer(s, p, data);
        break;
    case ASN1_CTYPE_NULL:
        ret = 0;
        break;
    case ASN1_CTYPE_OCTET_STRING:
        ret = asn1_random_octet_string(s, p, data);
        break;
    case ASN1_CTYPE_BIT_STRING:
        ret = asn1_random_bit_string(s, p, data);
        break;
    case ASN1_CTYPE_TAGGED:
        ret = asn1_random_tagged(s, p, data);
        break;
    case ASN1_CTYPE_OBJECT_IDENTIFIER:
    case ASN1_CTYPE_RELATIVE_OID:
        ret = asn1_random_object_identifier(s, p, data);
        break;
    case ASN1_CTYPE_REAL:
        ret = asn1_random_real(s, p, data);
        break;
    case ASN1_CTYPE_CHAR_STRING:
        /* XXX: we don't respect the various character constraints */
        if (p[1] <= ASN1_CSTR_UniversalString) {
            ret = asn1_random_restricted_mult_string(s, p, data);
        } else if (p[1] == ASN1_CSTR_UTF8String) {
            ret = asn1_random_utf8_string(s, p, data);
        } else {
            /* XXX: need to ensure constraints */
            ret = asn1_random_octet_string1(s, p, data, 0, UINT32_MAX);
        }
        break;
    case ASN1_CTYPE_ANY:
        ret = asn1_random_any(s, p, data);
        break;
    default:
        ret = -1;
    }
    return ret;
}

void *asn1_random(const ASN1CType *p, int seed)
{
    ASN1RandomState s_s, *s = &s_s;
    void *data;

    data = asn1_mallocz_value(p);
    if (!data)
        return NULL;

    s->rnd = asn1_rnd_init(seed);
    if (1) {
        s->sequence_of_len_max = 10;
        s->bit_string_len_max = 100;
        s->octet_string_len_max = 200;
        s->sequence_extensions = TRUE;
        s->large_integer_len_max = 512;
    } else {
        s->sequence_of_len_max = 10;
        s->bit_string_len_max = 20000;
        s->octet_string_len_max = 20000;
        s->sequence_extensions = TRUE;
        s->large_integer_len_max = 512;
    }

    asn1_random_type(s, p, data);
    asn1_rnd_end(s->rnd);
    return data;
}
