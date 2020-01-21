//
// Created by gyun on 2019-06-27.
//

#ifndef LIBDOT3_DOT3_ASN1_OBJASN1C_H
#define LIBDOT3_DOT3_ASN1_OBJASN1C_H

#include "dot3-internal.h"

/*
 * 함수 원형(들)
 */
// dot3-objasn1c-wsm-encode.c
int dot3_ObjAsn1c_EncodeWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);

#endif //LIBDOT3_DOT3_ASN1_OBJASN1C_H
