//
// Created by gyun on 2019-06-27.
//

#include "rtxsrc/rtxCommon.h"
#include "rtxsrc/rtxDiagBitTrace.h"
#include "rtxsrc/rtxPrint.h"
#include "rtxsrc/rtxStreamMemory.h"

#include "dot3-asn.h"
#include "dot3-internal.h"


/**
 * @brief ASN.1 인코딩을 위해 WSM-N-Header 정보 구조체를 채운다.
 * @param ctxt asn.1 인코더 컨텍스트
 * @param params WSM 송신파라미터
 * @param wsm_msg 정보를 채울 정보구조체의 버퍼 포인터를 전달한다.
 * @return 성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 *
 * 각 확장필드는 어떤 순서로 추가되어도 상관 없지만, 단위테스트 결과와의 비교를 위해
 * 채널번호, 데이터레이터, 전송파워 순서대로 추가한다.
 */
static int
dot3_ObjAsn1c_FillWsmpNHeader(OSCTXT *const ctxt, struct Dot3WsmMpduTxParams *const params, ShortMsgNpdu *const wsm_msg)
{
  Log(kDot3LogLevel_event, "Filling WSMP-N-header\n");

  /*
   * SubType, Version 필드를 채운다.
   */
  wsm_msg->subtype.t = T_ShortMsgSubtype_nullNetworking;
  wsm_msg->subtype.u.nullNetworking = rtxMemAllocTypeZ(ctxt, NullNetworking);
  if (!wsm_msg->subtype.u.nullNetworking) {
    Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(NullNetworking)\n");
    return -kDot3Result_Fail_Asn1Encode;
  }
  wsm_msg->subtype.u.nullNetworking->version = ASN1V_c_shortMsgVersionNo; // WSMP version = 3

  /*
   * 확장필드를 채운다.
   */
  OSRTDList *list; // 확장필드를 추가하기 위한 리스트 구조체
  ShortMsgNextension *data; // 확장필드에 추가되는 데이터
  bool ext_added = false; // 확장필드가 추가되었는지 여부

  // Channel load 확장필드 추가
  if (params->hdr_extensions.chan_load) {

    // 확장필드를 추가하기 위한 정보 초기화
    wsm_msg->subtype.u.nullNetworking->m.nExtensionsPresent = 1;
    list = &wsm_msg->subtype.u.nullNetworking->nExtensions;
    rtxDListInit(list);
    ext_added = true;

    // 확장필드 추가 */
    data = rtxMemAllocTypeZ(ctxt, ShortMsgNextension);
    if (!data) {
      Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(ShortMsgNextension)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    data->extensionId = ASN1V_c_ChannelLoad;
    data->value.numocts = sizeof(params->chan_load);
    data->value.data = (const OSOCTET *)&(params->chan_load);
    rtxDListAppend(ctxt, list, (void *)data);
  }
  // Channel Number 확장필드 추가
  if (params->hdr_extensions.chan_num) {

    // 확장필드를 추가하기 위한 정보 초기화
    if (!ext_added) {
      wsm_msg->subtype.u.nullNetworking->m.nExtensionsPresent = 1;
      list = &wsm_msg->subtype.u.nullNetworking->nExtensions;
      rtxDListInit(list);
      ext_added = true;
    }
    // 확장필드 추가
    data = rtxMemAllocTypeZ(ctxt, ShortMsgNextension);
    if (!data) {
      Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(ShortMsgNextension)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    data->extensionId = ASN1V_c_ChannelNumber80211;
    data->value.numocts = sizeof(uint8_t);
    data->value.data = (const OSOCTET *)&(params->chan_num);
    rtxDListAppend(ctxt, list, (void *)data);
  }
  // DataRate 확장필드 추가
  if (params->hdr_extensions.datarate) {

    // 확장필드를 추가하기 위한 정보 초기화
    if (!ext_added) {
      wsm_msg->subtype.u.nullNetworking->m.nExtensionsPresent = 1;
      list = &wsm_msg->subtype.u.nullNetworking->nExtensions;
      rtxDListInit(list);
      ext_added = true;
    }
    // 확장필드 추가
    data = rtxMemAllocTypeZ(ctxt, ShortMsgNextension);
    if (!data) {
      Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(ShortMsgNextension)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    data->extensionId = ASN1V_c_DataRate80211;
    data->value.numocts = sizeof(uint8_t);
    data->value.data = (const OSOCTET *)&(params->datarate);
    rtxDListAppend(ctxt, list, (void *)data);
  }
  // TransmitPowerUsed 확장필드 추가
  if (params->hdr_extensions.transmit_power) {

    // 확장필드를 추가하기 위한 정보 초기화
    if (!ext_added) {
      wsm_msg->subtype.u.nullNetworking->m.nExtensionsPresent = 1;
      list = &wsm_msg->subtype.u.nullNetworking->nExtensions;
      rtxDListInit(list);
      //ext_added = true;
    }
    // 확장필드 추가
    data = rtxMemAllocTypeZ(ctxt, ShortMsgNextension);
    if (!data) {
      Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(ShortMsgNextension)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    OSOCTET *tmp = rtxMemAllocType(ctxt, OSOCTET);
    if (!tmp) {
      Err("Fail to fill WSMP-N-header - fail to rtxMemAllocTypeZ(OSOCTET)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    *tmp = ConvertPowerForEncoding(params->transmit_power);
    data->extensionId = ASN1V_c_TxPowerUsed80211;
    data->value.numocts = sizeof(OSOCTET);
    data->value.data = tmp;
    rtxDListAppend(ctxt, list, (void *)data);
  }

  Log(kDot3LogLevel_event, "Success to fill WSM-N-header\n");
  return kDot3Result_Success;
}


/**
 * @brief ASN.1 인코딩을 위해 VarLengthNumber(Psid) 정보구조체를 채운다.
 * @param ctxt asn.1 인코더 컨텍스트
 * @param psid 인코딩할 psid 값
 * @param var 정보를 채울 정보구조체의 포인터를 전달한다.
 * @return 성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
static int dot3_ObjAsn1c_FillVarLengthNumber(OSCTXT *const ctxt, Dot3Psid psid, VarLengthNumber *const var_len_num)
{
  Log(kDot3LogLevel_event, "Filling VarLengthNumber - psid: %u\n", psid);

  /*
   * Psid 값의 범위에 따라 p-encoded Psid의 길이를 결정한다.
   */
  uint32_t p_encoded_psid_len;
  if (psid <= 127) {
    p_encoded_psid_len = 1;
  } else if (psid <= 16511) {
    p_encoded_psid_len = 2;
  } else if (psid <= 2113663) {
    p_encoded_psid_len = 3;
  } else if (psid <= 270549119) {
    p_encoded_psid_len = 4;
  } else {
    Err("Fail to fill VarLengthNumber(Psid) - invalid Psid %d\n", psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }

  asn1Init_VarLengthNumber(var_len_num);

  /*
   * p-encoded Psid 값의 길이에 맞게 VarLengthNumber 정보구조체를 채운다.
   */
  if (p_encoded_psid_len == 1) {
    var_len_num->t = T_VarLengthNumber_content;
    var_len_num->u.content = (uint8_t)psid;
  }
  else {
    var_len_num->t = T_VarLengthNumber_extension;
    var_len_num->u.extension = rtxMemAllocTypeZ(ctxt, Ext1);
    if (!var_len_num->u.extension) {
      Err("Fail to fill VarLengthNumber(Psid) - fail to rtxMemAllocTypeZ(Ext1)\n");
      return -kDot3Result_Fail_Asn1Encode;
    }
    if (p_encoded_psid_len == 2) {
      var_len_num->u.extension->t = T_Ext1_content;
      var_len_num->u.extension->u.content = (uint16_t)psid;
    }
    else {
      var_len_num->u.extension->t = T_Ext1_extension;
      var_len_num->u.extension->u.extension = rtxMemAllocTypeZ(ctxt, Ext2);
      if (!var_len_num->u.extension->u.extension) {
        Err("Fail to fill VarLengthNumber(Psid) - fail to rtxMemAllocTypeZ(Ext2)\n");
        return -kDot3Result_Fail_Asn1Encode;
      }
      if (p_encoded_psid_len == 3) {
        var_len_num->u.extension->u.extension->t = T_Ext2_content;
        var_len_num->u.extension->u.extension->u.content = (uint32_t)psid;
      }
      else {
        var_len_num->u.extension->u.extension->t = T_Ext2_extension;
        var_len_num->u.extension->u.extension->u.extension = (uint32_t)psid;
      }
    }
  }

  Log(kDot3LogLevel_event, "Success to fill VarLengthNumber\n");
  return kDot3Result_Success;
}


/**
 * @brief ASN.1 인코딩을 위해 WSM-T-Header 정보 구조체를 채운다.
 * @param ctxt asn.1 인코더 컨텍스트
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
dot3_ObjAsn1c_FillWsmpTHeader(
  OSCTXT *const ctxt,
  struct Dot3WsmMpduTxParams *const params,
  const Dot3PduSize payload_size,
  ShortMsgNpdu *const wsm_msg)
{
  int ret;
  Log(kDot3LogLevel_event, "Filling WSMP-T-header - Psid: %u, WSM length: %u\n", params->psid, payload_size);

  wsm_msg->transport.t = T_ShortMsgTpdus_bcMode;
  wsm_msg->transport.u.bcMode = rtxMemAllocType(ctxt, ShortMsgBcPDU);
  if (!wsm_msg->transport.u.bcMode) {
    Err("Fail to fill WSMP-T-header - fail to rtxMemAllocTypeZ(ShortMsgBcPDU)\n");
    return -kDot3Result_Fail_Asn1Encode;
  }
  asn1Init_ShortMsgBcPDU(wsm_msg->transport.u.bcMode);

  // wsm_msg->transport.u.bcMode->m.tExtensionsPresent = 0; // 표준에 따르면, 아직까지는 T-헤더에 확장필드는 없다.

  /*
   * Psid 필드를 채운다.
   */
  ret = dot3_ObjAsn1c_FillVarLengthNumber(ctxt, params->psid, &wsm_msg->transport.u.bcMode->destAddress);
  if (ret < 0) {
    Err("Fail to fill WSMP-T-header - fail to ObjAsn1c_FillVarLengthNumber()\n");
    return ret;
  }

  /*
   * WSM Length 필드를 채운다.
   */
  wsm_msg->body.numocts = payload_size;

  Log(kDot3LogLevel_event, "Success to fill WSMP-T-header\n");
  return kDot3Result_Success;
}


/**
 * @brief Objective systems 사의 asn.1 라이브러리를 이용하여 WSM을 UPER 인코딩한다.
 * @param params        @ref ConstructWsm
 * @param payload       @ref ConstructWsm
 * @param payload_size  @ref ConstructWsm
 * @param outbuf        @ref ConstructWsm
 * @param outbuf_size   @ref ConstructWsm
 * @return              @ref ConstructWsm
 */
int dot3_ObjAsn1c_EncodeWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  Log(kDot3LogLevel_event, "Encoding WSM\n");

  /*
   * ASN.1 인코더 컨텍스트를 초기화한다.
   */
  OSCTXT ctxt;
  int ret = rtInitContext(&ctxt);
  if (ret) {
    Err("Fail to encode WSM - fail to rtInitContext()\n");
    if (g_dot3_log >= kDot3LogLevel_err) {
      rtxErrPrint(&ctxt);
    }
    return -kDot3Result_Fail_Asn1Encode;
  }

  /*
   * 인코딩 결과가 저장될 버퍼를 지정한다.
   */
  pu_setBuffer(&ctxt, outbuf, outbuf_size, false/*unaligned*/);

  /*
   * 인코딩을 위한 asn.1 정보구조체를 할당하고 초기화한다.
   */
  ShortMsgNpdu *wsm_msg = rtxMemAllocType(&ctxt, ShortMsgNpdu);
  if (!wsm_msg) {
    Err("Fail to encode WSM - fail to rtxMemAllocType(ShortMsgNpdu)\n");
    rtxFreeContext(&ctxt);
    return -kDot3Result_Fail_Asn1Encode;
  }
  asn1Init_ShortMsgNpdu(wsm_msg);

  /*
   * asn.1 정보구조체의 WSM-N-Header 필드를 채운다.
   */
  ret = dot3_ObjAsn1c_WsmpNHeader(&ctxt, params, wsm_msg);
  if (ret < 0) {
    Err("Fail to encode WSM - fail to ObjAsn1c_FillWsmpNHeader()\n");
    rtxFreeContext(&ctxt);
    return ret;
  }

  /*
   * asn.1 정보구조체의 WSM-T-Header 필드를 채운다.
  */
  ret = dot3_ObjAsn1c_WsmpTHeader(&ctxt, params, payload_size, wsm_msg);
  if (ret < 0) {
    Err("Fail to encode WSM - fail to ObjAsn1c_FillWsmpTHeader()\n");
    rtxFreeContext(&ctxt);
    return ret;
  }

  /*
   * asn.1 정보구조체의 body 필드를 채운다.
   *  body 필드의 len 필드는 ObjAsn1c_FillWsmpTHeader()에서 이미 채워졌다.
   */
  if (payload && (payload_size > 0)) {
    wsm_msg->body.data = payload;
  }

  /*
   * 인코딩한다.
   */
  ret = asn1PE_ShortMsgNpdu(&ctxt, wsm_msg);
  if (ret < 0) {
    Err("Fail to encode WSM - fail to asn1PE_ShortMsgNpdu()\n");
    if (g_dot3_log >= kDot3LogLevel_err) {
      rtxErrPrint(&ctxt);
    }
    rtxFreeContext(&ctxt);
    if (ret == RTERR_BUFOVFLW) {
      return -kDot3Result_Fail_InsufficientBuf;
    } else {
      return -kDot3Result_Fail_Asn1Encode;
    }
  }

  /*
   * 인코딩 출력의 길이를 확인하고 유효성을 검증한다.
   */
  int encoded_wsm_size;
  pe_GetMsgPtr(&ctxt, &encoded_wsm_size);
  if (encoded_wsm_size < (kWsmpHdrMinSize + payload_size)) {
    Err("Fail to encode WSM - Too short encoded WSM: %d\n", encoded_wsm_size);
    rtxFreeContext(&ctxt);
    return -kDot3Result_Fail_TooShortWsm;
  }
  if (encoded_wsm_size > kWsmMaxSize) {
    Err("Fail to encode WSM - Too long encoded WSM: %d\n", encoded_wsm_size);
    rtxFreeContext(&ctxt);
    return -kDot3Result_Fail_TooLongWsm;
  }

  Log(kDot3LogLevel_event, "Success to encode %d-bytes WSM\n", encoded_wsm_size);
  return encoded_wsm_size;
}
