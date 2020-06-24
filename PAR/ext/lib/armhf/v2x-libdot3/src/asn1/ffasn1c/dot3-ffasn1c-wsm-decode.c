//
// Created by gyun on 2019-08-09.
//

#include <string.h>
#include <dot3-asn.h>
#include <dot3/dot3-types.h>

#include "asn1defs_int.h"
#include "dot3-asn.h"
#include "dot3-ffasn1c.h"
#include "dot3-internal.h"

/// WSMP-N-Header 에 수납될 수 있는 확장필드의 최대 개수
const int kWsmpNHeaderExtensionMaxCount = 3;

/**
 * @brief 디코딩된 WSM-N-Header 정보를 파싱한다.
 * @param wsm_msg   파싱할 asn.1 정보구조체의 주소를 전달한다.
 * @param params    파싱된 정보가 저장될 수신파라미터정보 구조체의 주소를 전달한다.
 * @return 성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_PasrseWsmpNHeader(struct ShortMsgNpdu *const wsm_msg, struct Dot3WsmMpduRxParams *const params)
{
  Log(kDot3LogLevel_event, "Parsing WSMP-N-header\n");

  /*
   * subtype 필드 체크
   */
  if (wsm_msg->subtype.choice != ShortMsgSubtype_nullNetworking) {
    Err("Fail to parse WSMP-N-Header - invalid subtype %d\n", wsm_msg->subtype.choice);
    return -kDot3Result_Fail_InvalidWsmpNHeaderSubType;
  }

  /*
   * WSMP version 필드 체크
   */
  if (wsm_msg->subtype.u.nullNetworking.version != kShortMsgVersionNo) {
    Err("Fail to parse WSMP-N-Header - invalid WSMP version %u\n", wsm_msg->subtype.u.nullNetworking.version);
    return -kDot3Result_Fail_InvalidWsmpNHeaderWsmpVersion;
  }
  params->version = wsm_msg->subtype.u.nullNetworking.version;

  /*
   * 확장필드 파싱 - ChannelNumber, DataRate, TransmitPowerUsed
   *  - 확장필드가 없는 경우, Unknown 값으로 설정된다.
   */
  params->tx_chan_num = kDot3Channel_Unknown;
  params->tx_datarate = kDot3DataRate_Unknown;
  params->tx_power = kDot3Power_Unknown;
  if (wsm_msg->subtype.u.nullNetworking.nExtensions_option) {
    if (!wsm_msg->subtype.u.nullNetworking.nExtensions.tab) {
      Err("Fail to parse WSMP-N-Header - null pointer for ShortMsgNextension\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }
    struct ShortMsgNextension *ext;
    uint32_t ext_field_cnt = wsm_msg->subtype.u.nullNetworking.nExtensions.count;
    if (ext_field_cnt > kWsmpNHeaderExtensionMaxCount) {
      Err("Fail to parse WSMP-N-Header - too many ext field - %u\n", ext_field_cnt);
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }
    for (uint32_t i = 0; i < wsm_msg->subtype.u.nullNetworking.nExtensions.count; i++) {
      ext = wsm_msg->subtype.u.nullNetworking.nExtensions.tab + i;
      switch (ext->extensionId) {
        case kDot3ExtensionId_ChannelNumber80211: {
          if (!ext->value.u.data) {
            Err("Fail to parse WSMP-N-Header - null pointer for ChannelNumber extension\n");
            return -kDot3Result_Fail_Asn1AbnormalOp;
          }
          params->tx_chan_num = *(int *)(ext->value.u.data);
          break;
        }
        case kDot3ExtensionId_DataRate80211: {
          if (!ext->value.u.data) {
            Err("Fail to parse WSMP-N-Header - null pointer for DataRate extension\n");
            return -kDot3Result_Fail_Asn1AbnormalOp;
          }
          params->tx_datarate = *(int *)(ext->value.u.data);
          break;
        }
        case kDot3ExtensionId_TxPowerUsed80211: {
          if (!ext->value.u.data) {
            Err("Fail to parse WSMP-N-Header - null pointer for TxPowerUsed extension\n");
            return -kDot3Result_Fail_Asn1AbnormalOp;
          }
          params->tx_power = *(int *)(ext->value.u.data);
          break;
        }
        default: {
          Err("Fail to parse WSMP-N-Header - invalid extension id %d\n", ext->extensionId);
          return -kDot3Result_Fail_InvalidWsmpNHeaderExtensionId;
        }
      }
    }
  }

  /*
   * TPID 필드 체크
   */
  if (wsm_msg->transport.choice != ShortMsgTpdus_bcMode) {
    Err("Fail to parse WSMP-N-Header - invalid TPID %d\n", wsm_msg->transport.choice);
    return -kDot3Result_Fail_InvalidWsmpNHeaderTpid;
  }

  Log(kDot3LogLevel_event, "Success to parse WSMP-N-Header - chan: %d, datarate: %d, power: %d\n",
      params->tx_chan_num, params->tx_datarate, params->tx_power);
  return kDot3Result_Success;
}


/**
 * @brief 디코딩된 WSMP-T-Header 정보구조체에서 PSID, 페이로드길이(=WSM body length) 정보를 파싱하여 반환한다.
 * @param wsm_msg   파싱할 WSMP-T-Header 정보구조체의 주소를 전달한다. NULL 을 사용할 수 없다.
 * @param params    파싱된 PSID 정보가 저장될 정보구조체의 주소를 전달한다. NULL 을 사용할 수 없다.
 * @return          성공시 파싱된 페이로드의 길이, 실패시 음수(-Dot3ResultCode)
 */
static int
dot3_FFAsn1c_ParseWsmpTHeader(const struct ShortMsgNpdu *const wsm_msg, struct Dot3WsmMpduRxParams *const params)
{
  Log(kDot3LogLevel_event, "Parsing WSMP-T-header\n");

  /*
   * PSID 파싱
   */
  int ret = dot3_FFAsn1c_ParseVarLengthNumber(&wsm_msg->transport.u.bcMode.destAddress);
  if (ret < 0) {
    Err("Fail to parse WSMP-T-Header - fail to dot3_FFAsn1c_FillVarLengthNumber()\n");
    return ret;
  }
  if (ret > kDot3Psid_Max) {
    Err("Fail to parse WSMP-T-Header - too large psid value %d\n", ret);
    return -kDot3Result_Fail_InvalidPsidValue;
  }
  params->psid = (Dot3Psid)ret;

  /*
   * 페이로드길이 파싱
   *  - 허용길이를 초과하면 에러를 반환한다.
   */
  size_t payload_size = wsm_msg->body.len;
  if (payload_size > kWsmBodyMaxSize) {
    Err("Fail to parse WSMP-T-Header - too long payload %d > %d(kWsmBodyMaxSize)\n", payload_size, kWsmBodyMaxSize);
    return -kDot3Result_Fail_TooLongPayload;
  }

  Log(kDot3LogLevel_event, "Success to parse WSM-T-Header - psid: %u, payload_size: %u\n", params->psid, payload_size);
  return (int)payload_size;
}

/**
 * @brief ffasn1c 라이브러리를 이용하여 WSM을 UPER 인코딩한다.
 * @param msdu          @ref ParseWsm
 * @param msdu_size     @ref ParseWsm
 * @param outbuf        @ref ParseWsm
 * @param outbuf_size   @ref ParseWsm
 * @param params        @ref ParseWsm
 * @return              @ref ParseWsm
 */
int dot3_FFAsn1c_DecodeWsm(
  const uint8_t *const msdu,
  const Dot3PduSize msdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params)
{
  int ret;
  struct ShortMsgNpdu *wsm_msg = NULL;
  Log(kDot3LogLevel_event, "Decoding WSM\n");

  /*
   * WSM 디코딩
   */
  ASN1Error err;
  asn1_ssize_t decoded_size = asn1_uper_decode((void **)&wsm_msg, asn1_type_ShortMsgNpdu, msdu, msdu_size, &err);
  if ((decoded_size < 0) || (decoded_size > msdu_size) || (!wsm_msg)) {
    Err("Fail to decode WSM - fail to asn1_uper_decode() - decoded_size %d\n", decoded_size);
    return -kDot3Result_Fail_Asn1Decode;
  }

  /*
   * WSM-N-Header 필드의 정보를 파싱하여 반환정보에 저장한다.
   */
  ret = dot3_FFAsn1c_PasrseWsmpNHeader(wsm_msg, params);
  if (ret < 0) {
    Err("Fail to decode WSM - fail to FFAsn1c_PasrseWsmpNHeader()\n");
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    return ret;
  }

  /*
   * WSM-T-Header 필드의 정보를 파싱하여 반환정보에 저장한다.
   */
  int payload_size;
  payload_size = dot3_FFAsn1c_ParseWsmpTHeader(wsm_msg, params);
  if (payload_size < 0) {
    Err("Fail to decode WSM - fail to FFAsn1c_ParseWsmpTHeader()\n");
    asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
    return payload_size;
  }

  /*
   * WSM body 를 파싱하여 반환정보에 저장한다.
   */
  if (payload_size) {
    if ((payload_size > outbuf_size)) {
      Err("Fail to decode WSM - insufficient buffer for payload %d > %u\n", payload_size, outbuf_size);
      asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);
      return -kDot3Result_Fail_InsufficientBuf;
    }
    memcpy(outbuf, wsm_msg->body.buf, payload_size);
  }

  /*
   * asn.1 정보구조체 해제
   */
  asn1_free_value(asn1_type_ShortMsgNpdu, wsm_msg);

  Log(kDot3LogLevel_event, "Success to decode WSM - payload has %d bytes size\n", payload_size);
  return payload_size;
}
