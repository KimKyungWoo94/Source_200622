/* 
 * ASN1 common definitions for BER encoding
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

void asn1_ber_put_len(ASN1ByteBuffer *s, unsigned int len)
{
    unsigned int v;
    int n, i;

    if (len <= 127) {
        asn1_put_byte(s, len);
    } else {
        n = 0;
        v = len;
        do {
            v >>= 8;
            n++;
        } while (v != 0);
        asn1_put_byte(s, n | 0x80);
        for(i = n - 1; i >= 0; i--) {
            asn1_put_byte(s, (len >> (8 * i)) & 0xff);
        }
    }
}

void asn1_ber_put_tag(ASN1ByteBuffer *s, unsigned int tag, int constructed)
{
    unsigned int b, v;
    int n, i;

#ifdef DEBUG
    printf("put_tag: 0x%x c=%d\n", tag, constructed);
#endif
    b = (tag >> (ASN1_TAG_CLASS_SHIFT - 6)) & 0xc0;
    b |= constructed << 5;
    tag &= ASN1_TAG_MAX;
    if (tag < 31) {
        asn1_put_byte(s, b | tag);
    } else {
        asn1_put_byte(s, b | 0x1f);
        v = tag;
        n = 0;
        do {
            v >>= 7;
            n++;
        } while (v != 0);
        for(i = n - 1; i >= 1; i--)
            asn1_put_byte(s, ((tag >> (i * 7)) & 0x7f) | 0x80);
        asn1_put_byte(s, tag & 0x7f);
    }
}

int asn1_ber_encode_int32(ASN1ByteBuffer *s, int val)
{
    int shift, n, i;
    n = 1;
    shift = 24;
    while (((val << shift) >> shift) != val) {
        n++;
        shift -= 8;
    }
        
    asn1_ber_put_len(s, n);
    for(i = n - 1; i >= 0; i--) {
        asn1_put_byte(s, ((unsigned int)val >> (8 * i)) & 0xff);
    }
    return 0;
}

#define TAG_LEN_MAX_BYTES 5

uint32_t asn1_ber_put_len_start(ASN1ByteBuffer *s)
{
    uint32_t i, pos;
    pos = s->len;
    /* put dummy data to handle worst case length (5 bytes) */
    for(i = 0; i < TAG_LEN_MAX_BYTES; i++)
        asn1_put_byte(s, 0);
    return pos;
}

void asn1_ber_put_len_end(ASN1ByteBuffer *s, uint32_t pos)
{
    uint32_t saved_len, tag_len, n;

    tag_len = s->len - (pos + TAG_LEN_MAX_BYTES);
    saved_len = s->len;
    s->len = pos;
    asn1_ber_put_len(s, tag_len);
    /* number of bytes to remove */
    n = (pos + TAG_LEN_MAX_BYTES) - s->len;
    if (n != 0) {
        memmove(s->buf + s->len, s->buf + s->len + n, 
                saved_len - (s->len + n));
    }
    s->len = saved_len - n;
}
