//
// Created by gyun on 2019-08-02.
//

#include <string.h>
#include <dot3-asn.h>
#include <dot3/dot3-types.h>

#include "asn1defs_int.h"
#include "dot3-asn.h"
#include "dot3-ffasn1c.h"
#include "dot3-internal.h"

const int kShortMsgVersionNo = 3; ///< WSMP version = 3


/**
 * @brief ASN.1 인코딩을 위해 WSM-N-Header 정보 구조체를 채운다.
 * @param params WSM 송신파라미터
 * @param wsm_msg 정보를 채울 정보구조체의 버퍼 포인터를 전달한다.
 * @return 성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 *
 * 표준 상 각 확장필드는 어떤 순서로 추가되어도 상관 없지만, 단위테스트 결과와의 비교를 위해
 * 채널번호, 데이터레이터, 전송파워 순서대로 추가한다.
 */
static int
dot3_FFAsn1c_FillWsmpNHeader(struct Dot3WsmMpduTxParams *const params, ShortMsgNpdu *const wsm_msg)
{
  Log(kDot3LogLevel_event, "Filling WSMP-N-header\n");

  /*
   * SubType, Version 필드를 채운다.
   */
  wsm_msg->subtype.choice = ShortMsgSubtype_nullNetworking;
  wsm_msg->subtype.u.nullNetworking.version = kShortMsgVersionNo;

  /*
   * 있을 경우, 확장필드를 채운다.
   */
  wsm_msg->subtype.u.nullNetworking.nExtensions.count = 0;
  if (params->hdr_extensions.chan_num) {
    wsm_msg->subtype.u.nullNetworking.nExtensions.count++;
  }
  if (params->hdr_extensions.datarate) {
    wsm_msg->subtype.u.nullNetworking.nExtensions.count++;
  }
  if (params->hdr_extensions.transmit_power) {
    wsm_msg->subtype.u.nullNetworking.nExtensions.count++;
  }
  if (wsm_msg->subtype.u.nullNetworking.nExtensions.count) {

    wsm_msg->subtype.u.nullNetworking.nExtensions_option = true;
    wsm_msg->subtype.u.nullNetworking.nExtensions.tab = (ShortMsgNextension *)asn1_malloc(
      asn1_get_size(asn1_type_ShortMsgNextension) * wsm_msg->subtype.u.nullNetworking.nExtensions.count);
    if (!wsm_msg->subtype.u.nullNetworking.nExtensions.tab) {
      Err("Fail to fill WSMP-N-header - fail to asn1_malloc(ShortMsgNextensions.tab)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    uint8_t *ptr = (uint8_t *)wsm_msg->subtype.u.nullNetworking.nExtensions.tab;

    // Channel Number 확장필드
    if (params->hdr_extensions.chan_num) {
      struct ShortMsgNextension *ext_chan_num = (struct ShortMsgNextension *)ptr;
      ptr += asn1_get_size(asn1_type_ShortMsgNextension);
      ext_chan_num->extensionId = kDot3ExtensionId_ChannelNumber80211;
      ext_chan_num->value.type = (ASN1CType *)asn1_type_ChannelNumber80211;
      ext_chan_num->value.u.data = asn1_mallocz_value(asn1_type_ChannelNumber80211);
      if (!ext_chan_num->value.u.data) {
        Err("Fail to fill WSMP-N-header - fail to asn1_mallocz_value(asn1_type_ChannelNumber80211)\n");
        return -kDot3Result_Fail_Asn1Encode;
      }
      *(int *)(ext_chan_num->value.u.data) = params->chan_num;
    }

    // DataRate 확장필드
    if (params->hdr_extensions.datarate) {
      struct ShortMsgNextension *ext_datarate = (struct ShortMsgNextension *)ptr;
      ptr += asn1_get_size(asn1_type_ShortMsgNextension);
      ext_datarate->extensionId = kDot3ExtensionId_DataRate80211;
      ext_datarate->value.type = (ASN1CType *)asn1_type_DataRate80211;
      ext_datarate->value.u.data = asn1_mallocz_value(asn1_type_DataRate80211);
      if (!ext_datarate->value.u.data) {
        Err("Fail to fill WSMP-N-header - fail to asn1_mallocz_value(asn1_type_DataRate80211)\n");
        return -kDot3Result_Fail_Asn1Encode;
      }
      *(int *)(ext_datarate->value.u.data) = params->datarate;
    }

    // TransmitPowerUsed 확장필드
    if (params->hdr_extensions.transmit_power) {
      struct ShortMsgNextension *ext_tx_power = (struct ShortMsgNextension *)ptr;
      ext_tx_power->extensionId = kDot3ExtensionId_TxPowerUsed80211;
      ext_tx_power->value.type = (ASN1CType *)asn1_type_TXpower80211;
      ext_tx_power->value.u.data = asn1_mallocz_value(asn1_type_TXpower80211);
      if (!ext_tx_power->value.u.data) {
        Err("Fail to fill WSMP-N-header - fail to asn1_mallocz_value(asn1_type_TXpower80211)\n");
        return -kDot3Result_Fail_Asn1Encode;
      }
      *(int *)(ext_tx_power->value.u.data) = params->transmit_power;
    }
  }

  Log(kDot3LogLevel_event, "Success to fill WSM-N-header\n");
  return kDot3Result_Success;
}


/**
 * @brief ASN.1 인코딩을 위해 WSM-T-Header 정보 구조체를 채운다.
 * @param params WSM 송신파라미터
 * @param payload_size 페이로드(WSM body)의 길이  = WSM Length
 * @param wsm_msg 정보를 채울 정보구조체의 버퍼 포인터를 전달한다.
 * @return 성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 *
 * 현재 버전의 1609.3 표준(2016)에서
 *  - TPID=0일 때 WSM-T-Header는 PSID와 WSM Length 필드를 가진다.
 *  - TPID=1일 때 WSM-T-Header는 확장필드를 가진다고 되어 있으나, 현재 정의되어 있는 확장필드는 없다.
 */
static int
dot3_FFAsn1c_FillWsmpTHeader(
  struct Dot3WsmMpduTxParams *const params,
  const Dot3PduSize payload_size,
  ShortMsgNpdu *const wsm_msg)
{
  int ret;
  Log(kDot3LogLevel_event, "Filling WSMP-T-header - Psid: %u, WSM length: %u\n", params->psid, payload_size);

  // TODO:: TPID는 N 헤더이므로, N헤더 파싱 쪽으로 옮긴다.
  wsm_msg->transport.choice = ShortMsgTpdus_bcMode;
  wsm_msg->transport.u.bcMode.tExtensions_option = false; // 표준에 따르면, 아직까지는 T-헤더에 확장필드는 없다.

  /*
   * Psid 필드를 채운다.
   */
  ret = dot3_FFAsn1c_FillVarLengthNumber(params->psid, &wsm_msg->transport.u.bcMode.destAddress);
  if (ret < 0) {
    Err("Fail to fill WSMP-T-header - fail to dot3FFAsn1c_FillVarLengthNumber()\n");
    return ret;
  }

  /*
   * WSM Length 필드를 채운다.
   */
  wsm_msg->body.len = payload_size;

  Log(kDot3LogLevel_event, "Success to fill WSMP-T-header\n");
  return kDot3Result_Success;
}


/**
 * @brief ffasn1c 라이브러리를 이용하여 WSM을 UPER 인코딩한다.
 * @param params        @ref ConstructWsm
 * @param payload       @ref ConstructWsm
 * @param payload_size  @ref ConstructWsm
 * @param outbuf        @ref ConstructWsm
 * @param outbuf_size   @ref ConstructWsm
 * @return              @ref ConstructWsm
 */
int dot3_FFAsn1c_EncodeWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  int ret;
  Log(kDot3LogLevel_event, "Encoding WSM\n");

  /*
   * 인코딩을 위한 asn.1 정보구조체를 할당하고 초기화한다.
   */
  struct ShortMsgNpdu *wsm_msg = (struct ShortMsgNpdu *)asn1_mallocz_value(asn1_type_ShortMsgNpdu);
  if (!wsm_msg) {
    Err("Fail to encode WSM - fail to asn1_mallocz_value(ShortMsgNpdu)\n");
    return -kDot3Result_Fail_Asn1Encode;
  }

  /*
   * asn.1 정보구조체의 WSM-N-Header 필드를 채운다.
   */
  ret = dot3_FFAsn1c_FillWsmpNHeader(params, wsm_msg);
  if (ret < 0) {
    Err("Fail to encode WSM - fail to FFAsn1c_FillWsmpNHeader()\n");
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    return ret;
  }

  /*
   * asn.1 정보구조체의 WSM-T-Header 필드를 채운다.
  */
  ret = dot3_FFAsn1c_FillWsmpTHeader(params, payload_size, wsm_msg);
  if (ret < 0) {
    Err("Fail to encode WSM - fail to FFAsn1c_FillWsmpTHeader()\n");
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    return ret;
  }

  /*
   * asn.1 정보구조체의 body 필드를 채운다.
   *  body 필드의 len 필드는 FFAsn1c_FillWsmpTHeader()에서 이미 채워졌다.
   */
  if (payload && (payload_size > 0)) {
    wsm_msg->body.buf = (uint8_t *)asn1_malloc(payload_size);
    if (!wsm_msg->body.buf) {
      Err("Fail to encode WSM - fail to asn1_malloc(payload)\n");
      asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    }
    memcpy(wsm_msg->body.buf, payload, payload_size);
  }

  /*
   * 인코딩하고 결과 유효성을 검증한다.
   */
  uint8_t *buf;
  asn1_ssize_t encoded_wsm_size = asn1_uper_encode(&buf, asn1_type_ShortMsgNpdu, wsm_msg);
  // 인코딩 실패
  if ((encoded_wsm_size < 0) || (buf == NULL)) {
    Err("Fail to encode WSM - fail to asn1_uper_encode()\n");
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    return -kDot3Result_Fail_Asn1Encode;
  }
  // 인코딩 길이가 허용되는 최대길이보다 크면 실패
  if (encoded_wsm_size > kWsmMaxSize) {
    Err("Fail to encode WSM - Too long encoded WSM: %d\n", encoded_wsm_size);
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    asn1_free(buf);
    return -kDot3Result_Fail_TooLongWsm;
  }
  // 인코딩 길이가 outbuf의 크기보다 크면 실패
  if (encoded_wsm_size > outbuf_size) {
    Err("Fail to encode WSM - Insufficient buffer size than encoded: %d < %d\n", outbuf_size, encoded_wsm_size);
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    asn1_free(buf);
    return -kDot3Result_Fail_InsufficientBuf;
  }
  // 인코딩 길이가 이론 상 최소길이보다 짧으면 실패
  if (encoded_wsm_size < (kWsmpHdrMinSize + payload_size)) {
    Err("Fail to encode WSM - Too short encoded WSM: %d\n", encoded_wsm_size);
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    asn1_free(buf);
    return -kDot3Result_Fail_TooShortWsm;
  }

  /*
   * 인코딩 데이터 반환
   */
  memcpy(outbuf, buf, encoded_wsm_size);

  /*
   * asn.1 정보구조체 및 인코딩 버퍼 메모리 해제
   */
  asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
  asn1_free(buf);

  Log(kDot3LogLevel_event, "Success to encode %d-bytes WSM\n", encoded_wsm_size);
  return encoded_wsm_size;
}
