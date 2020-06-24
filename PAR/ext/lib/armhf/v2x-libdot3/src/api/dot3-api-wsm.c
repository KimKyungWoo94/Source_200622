//
// Created by gyun on 2019-06-06.
//

#include <stddef.h>

#include "dot3/dot3.h"
#include "dot3-internal.h"

/**
 * @brief Dot3_ConstructWsmMpdu() API에 전달된 인자들의 유효성을 체크한다.
 * @param params WSM 송신 파라미터
 * @param payload 상위계층 페이로드 버퍼 포인터
 * @param payload_size payload 의 길이
 * @param outbuf 생성된 MPDU가 반환될 버퍼 포인터
 * @param outbuf_size 생성된 MPDU의 길이가 반환될 변수 포인터
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 *
 * 송신파라미터 인자의 datarate와 transmit_power 값은 유효하지 않을 경우 기본 값으로 조정된다.
 */
static int dot3_CheckAndAdjustApiParameters_ConstructWsmMpdu(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  Log(kDot3LogLevel_event, "Checking Dot3_ConstructWsmMpdu() parameters\n");

  // 널 포인터 체크
  if (!params || !outbuf) {
    Err("Invalid Dot3_ConstructWsmMpdu() parameters - null parameters\n");
    return -kDot3Result_Fail_NullParameters;
  }
  // 페이로드 최대길이 체크
  if (payload_size > (kMsduMaxSize - (kLLCHdrSize + kWsmpHdrMinSize))) {
    Err("Invalid Dot3_ConstructWsmMpdu() parameter - too long payload %u > %u\n",
        payload_size, (kMsduMaxSize - (kLLCHdrSize + kWsmpHdrMinSize)));
    return -kDot3Result_Fail_TooLongPayload;
  }
  // outbuf 최소길이 체크
  if (outbuf_size < (kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize + payload_size)) {
    Err("Invalid Dot3_ConstructWsmMpdu() parameter - insufficient outbuf %u < %u\n",
        outbuf_size, (kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize + payload_size));
    return -kDot3Result_Fail_InsufficientBuf;
  }
  // PSID 값 유효성 확인
  if (dot3_IsValidPsidValue(params->psid) == false) {
    Err("Invalid Dot3_ConstructWsmMpdu() parameter - PSID %u\n", params->psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }
  // Priority 값 유효성 확인
  if (dot3_IsValidPriorityValue(params->priority) == false) {
    Err("Invalid Dot3_ConstructWsmMpdu() parameter - priority %d\n", params->priority);
    return -kDot3Result_Fail_InvalidPriorityValue;

  }
  // WSN-N 헤더 확장필드 포함 요청이 있을 경우, 채널번호 값 유효성 확인
  if (params->hdr_extensions.chan_num) {
    if (dot3_IsValidChannelNumberValue(params->chan_num) == false) {
      Err("Invalid Dot3_ConstructWsmMpdu() parameter - channel number %d\n", params->chan_num);
      return -kDot3Result_Fail_InvalidChannelNumberValue;
    }
  }
  // WSN-N 헤더 확장필드 포함 요청이 있을 경우, datarate 값 유효성 확인
  if (params->hdr_extensions.datarate) {
    if (dot3_IsValidDataRateValue(params->datarate) == false) {
      Err("Invalid Dot3_ConstructWsmMpdu() parameter - datarate %d\n", params->datarate);
      return -kDot3Result_Fail_InvalidDataRate;
    }
  }
  // WSN-N 헤더 확장필드 포함 요청이 있을 경우, transmit power 값 유효성 확인
  if (params->hdr_extensions.transmit_power) {
    if (dot3_IsValidPowerValue(params->transmit_power) == false) {
      Err("Invalid Dot3_ConstructWsmMpdu() parameter - power %d\n", params->transmit_power);
      return -kDot3Result_Fail_InvalidPowerValue;
    }
  }
  Log(kDot3LogLevel_event, "Success to check Dot3_ConstructWsmMpdu() parameters\n");
  return kDot3Result_Success;
}

/*
 * 페이로드(=WSM body)와 송신파라미터를 전달 받아 WSM MPDU를 생성하여 반환한다.
 *
 * 각 인자와 반환값에 대한 설명은 API 선언부 참조.
 */
int OPEN_API Dot3_ConstructWsmMpdu(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  int ret, mpdu_size;
  Log(kDot3LogLevel_event, "Constructing WSM MPDU - payload size is %u\n", payload_size);

  /*
   * 파라미터 체크
   */
  ret = dot3_CheckAndAdjustApiParameters_ConstructWsmMpdu(params, payload, payload_size, outbuf, outbuf_size);
  if (ret < 0) {
    Err("Fail to construct WSM MPDU - invalid parameter\n");
    return ret;
  }

  /*
   * 파라미터 불일치 조정
   */
  Dot3PduSize adj_payload_size = payload_size;
  const uint8_t *adj_payload = payload;
  if (!payload || !payload_size) {
    adj_payload = NULL;
    adj_payload_size = 0;
  }

  /*
   * WSM 패킷 생성
   */
  uint16_t reserved_room_size = kQoSMacHdrSize + kLLCHdrSize;
  int wsm_size = dot3_ConstructWsm(params,
                              adj_payload,
                              adj_payload_size,
                              outbuf + reserved_room_size,
                              outbuf_size - reserved_room_size);
  if (wsm_size < 0) {
    Err("Fail to construct WSM MPDU - fail to construct WSM\n");
    return wsm_size;
  }

  /*
   * MPDU 생성
   */
  dot3_ConstructMpdu(params, outbuf);
  mpdu_size = kQoSMacHdrSize + kLLCHdrSize + wsm_size;

  Log(kDot3LogLevel_event, "Success to construct %d-bytes WSM MPDU\n", mpdu_size);
  return mpdu_size;
}

/**
 * @brief Dot3_ParseWsmMpdu()와 Dot3_ParseInterestedWsmMpdu() API에 전달된 인자들의 유효성을 체크한다.
 * @param mpdu MPDU 버퍼 포인터
 * @param mpdu_size MPDU 의 길이
 * @param outbuf 파싱된 페이로드가 반환될 버퍼 포인터
 * @param params WSM 수신파라미터정보가 업데이트될 구조체 포인터
 * @param wsr_registered WSR 등록 여부가 저장될 변수 포인터
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_CheckAndAdjustApiParameters_ParseWsmMpdu(
  const uint8_t *const mpdu,
  const Dot3PduSize mpdu_size,
  uint8_t *const outbuf,
  struct Dot3WsmMpduRxParams *const params,
  bool *const wsr_registered)
{
  Log(kDot3LogLevel_event, "Checking Dot3_ParseWsmMpdu() parameters\n");

  /*
   * 널 포인터 체크
   */
  if (!mpdu || !outbuf || !params || !wsr_registered) {
    Err("Invalid Dot3_ParseWsmMpdu() parameters - null parameters\n");
    return -kDot3Result_Fail_NullParameters;
  }
  /*
   *  MPDU 최대길이 체크
   */
  if (mpdu_size > kMpduMaxSize) {
    Err("Invalid Dot3_ParseWsmMpdu() parameter - too long mpdu %u > %u\n",
        mpdu_size, kMpduMaxSize);
    return -kDot3Result_Fail_TooLongMpdu;
  }
  /*
   * MPDU 최소길이 체크
   */
  if (mpdu_size < kWsmMpduMinSize) {
    Err("Invalid Dot3_ParseWsmMpdu() parameter - too short mpdu %u < %u\n",
        mpdu_size, kWsmMpduMinSize);
    return -kDot3Result_Fail_TooShortMpdu;
  }

  Log(kDot3LogLevel_event, "Success to check Dot3_ParseWsmMpdu() parameters\n");
  return kDot3Result_Success;
}

/*
 * WSM MPDU 를 파싱하여 페이로드(=WSM body)와 수신파라미터들을 반환한다.
 *
 * 각 인자와 반환값에 대한 설명은 API 선언부 참조.
 */
int OPEN_API Dot3_ParseWsmMpdu(
  const uint8_t *const mpdu,
  const Dot3PduSize mpdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params,
  bool *const wsr_registered)
{
  int ret, payload_size;
  Log(kDot3LogLevel_event, "Parsing %u-bytes WSM MPDU\n", mpdu_size);

  /*
   * 파라미터 체크
   *  - outbuf_size는 여기서 체크하지 않고, WSM 디코딩 절차에서 실제 디코딩된 WSM body의 길이와 직접 비교된다.
   *    (실제 디코딩 전에는 필요한 버퍼 크기를 알 수 없다)
   */
  ret = dot3_CheckAndAdjustApiParameters_ParseWsmMpdu(mpdu, mpdu_size, outbuf, params, wsr_registered);
  if (ret < 0) {
    Err("Fail to parse WSM MPDU - invalid parameter\n");
    return ret;
  }

  /*
   * MPDU 파싱 - 하위계층(MAC, LLC) 헤더들의 크기가 반환된다.
   */
  ret = dot3_ParseMpdu(mpdu, mpdu_size, params);
  if (ret < 0) {
    Err("Fail to parse WSM MPDU - fail to parse MPDU\n");
    return ret;
  }
  Dot3PduSize lower_layer_hdr_size = (Dot3PduSize)ret;

  /*
   * WSM 파싱 - 페이로드(WSM body) 및 수신파라미터정보가 반환된다.
   */
  payload_size = dot3_ParseWsm(mpdu + lower_layer_hdr_size, mpdu_size - lower_layer_hdr_size,
                          outbuf, outbuf_size, params);
  if (payload_size < 0) {
    Err("Fail to parse WSM MPDU - fail to parse WSM\n");
    return payload_size;
  }

  /*
   * 해당 PSID가 WSR 테이블에 등록되어 있는지 확인
   * TODO::
   */
  *wsr_registered = true;


  Log(kDot3LogLevel_event, "Success to parse WSM MPDU - payload size is %u\n", payload_size);
  return payload_size;
}
