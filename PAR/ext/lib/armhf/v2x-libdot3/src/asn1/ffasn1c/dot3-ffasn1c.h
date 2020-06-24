//
// Created by gyun on 2019-08-02.
//

#ifndef V2X_LIBDOT3_DOT3_ASN1_FFASN1C_H
#define V2X_LIBDOT3_DOT3_ASN1_FFASN1C_H


#include "dot3-asn.h"
#include "dot3-internal.h"


extern const int kShortMsgVersionNo;

/**
 * @brief 확장필드 식별자
 */
enum eDot3ExtensionId
{
  // for WSMP-N-Header
  kDot3ExtensionId_TxPowerUsed80211 = 4,
  kDot3ExtensionId_ChannelNumber80211 = 15,
  kDot3ExtensionId_DataRate80211 = 16,

  // for WSA header
  kDot3ExtensionId_RepeatRate = 17,
  kDot3ExtensionId_2DLocation = 5,
  kDot3ExtensionId_3DLocation = 6,
  kDot3ExtensionId_AdvertiserId = 7,

  // for WSA service info
  kDot3ExtensionId_Psc = 8,
  kDot3ExtensionId_IPv6Address = 9,
  kDot3ExtensionId_ServicePort = 10,
  kDot3ExtensionId_ProviderMacAddress = 11,
  kDot3ExtensionId_RcpiThreshold = 19,
  kDot3ExtensionId_WsaCountThreshold = 20,
  kDot3ExtensionId_WsaCountThresholdInterval = 22,

  // for WSA channel info
  kDot3ExtensionId_EdcaParameterSet = 12,
  kDot3ExtensionId_ChannelAccess = 21,

  // for WRA
  kDot3ExtensionId_SecondaryDns = 13,
  kDot3ExtensionId_GatewayMacAddress = 14
};
typedef int Dot3ExtensionId;  /// @copydoc eDot3ExtensionId

/*
 * 함수 원형(들)
 */
// dot3-ffasn1c.c
int INTERNAL dot3_FFAsn1c_FillVarLengthNumber(Dot3Psid psid, VarLengthNumber *const var_len_num);
int INTERNAL dot3_FFAsn1c_ParseVarLengthNumber(const VarLengthNumber *const var_len_num);

// dot3-ffasn1c-wsa_decode.c
int INTERNAL dot3_FFAsn1c_DecodeWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params);
// dot3-ffasn1c-wsa-encode.c
int INTERNAL dot3_FFAsn1c_EncodeWsa(
  struct Dot3ProviderInfo *const pinfo,
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);
// dot3-ffasn1c-wsm-decode.c
int dot3_FFAsn1c_DecodeWsm(
  const uint8_t *const msdu,
  const Dot3PduSize msdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params);
// dot3-ffasn1c-wsm-encode.c
int dot3_FFAsn1c_EncodeWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);

#endif //V2X_LIBDOT3_DOT3_ASN1_FFASN1C_H
