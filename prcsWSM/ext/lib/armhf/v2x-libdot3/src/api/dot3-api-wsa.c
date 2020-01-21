/**
 * @file dot3-api-wsa.c
 * @date 2019-08-17
 * @author gyun
 * @brief WSA 관련 API들을 구현한 파일
 */


#include <dot3/dot3-types.h>
#include "dot3/dot3-types.h"

#include "dot3-internal.h"


/**
 * Dot3_ConstructWsa() API 의 파라미터 유효성을 검사한다.
 *
 * @param params        @ref Dot3_ConstructWsa
 * @param outbuf        @ref Dot3_ConstructWsa
 * @param outbuf_size   @ref Dot3_ConstructWsa
 * @return              성공 시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_CheckApiParameters_ConstrutWsa(
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  if (!params || !outbuf || !outbuf_size) {
    Err("Fail to construct WSA - null parameters params: %p, outbuf: %p, outbuf_size: %d", params, outbuf, outbuf_size);
    return -kDot3Result_Fail_NullParameters;
  }
  if (false == dot3_IsValidWsaIdValue(params->hdr.wsa_id)) {
    Err("Fail to construct WSA - invalid wsa id %u\n", params->hdr.wsa_id);
    return -kDot3Result_Fail_InvalidWsaIdValue;
  }
  if (false == dot3_IsValidWsaContentCountValue(params->hdr.content_count)) {
    Err("Fail to construct WSA - invalid content count %u\n", params->hdr.content_count);
    return -kDot3Result_Fail_InvalidWsaContentCountValue;
  }
  if (true == params->hdr.extensions.twod_location) {
    if (false == dot3_IsValidLatitudeValue(params->hdr.twod_location.latitude)) {
      Err("Fail to construct WSA - invalid latitude %d\n", params->hdr.twod_location.latitude);
      return -kDot3Result_Fail_InvalidLatitudeValue;
    }
    if (false == dot3_IsValidLongitudeValue(params->hdr.twod_location.longitude)) {
      Err("Fail to construct WSA - invalid longitude %d\n", params->hdr.twod_location.longitude);
      return -kDot3Result_Fail_InvalidLongitudeValue;
    }
  }
  if (true == params->hdr.extensions.threed_location) {
    if (false == dot3_IsValidLatitudeValue(params->hdr.threed_location.latitude)) {
      Err("Fail to construct WSA - invalid latitude %d\n", params->hdr.threed_location.latitude);
      return -kDot3Result_Fail_InvalidLatitudeValue;
    }
    if (false == dot3_IsValidLongitudeValue(params->hdr.threed_location.longitude)) {
      Err("Fail to construct WSA - invalid longitude %d\n", params->hdr.threed_location.longitude);
      return -kDot3Result_Fail_InvalidLongitudeValue;
    }
    if (false == dot3_IsValidElevationValue(params->hdr.threed_location.elevation)) {
      Err("Fail to construct WSA - invalid elevation %d\n", params->hdr.threed_location.elevation);
      return -kDot3Result_Fail_InvalidElevationValue;
    }
  }
  if ((true == params->hdr.extensions.advertiser_id) &&
      (false == dot3_IsValidWsaAdvertiserIdLen(params->hdr.advertiser_id.len))) {
    Err("Fail to construct WSA - invalid advertiser id len %u\n", params->hdr.advertiser_id.len);
    return -kDot3Result_Fail_InvalidAdvertiserIdLen;
  }
  if ((true == params->present.wra) && (false == dot3_IsValidIPv6PrefixLenValue(params->wra.ip_prefix_len))) {
    Err("Fail to construct WSA - invalid ipv6 prefix len %u\n", params->wra.ip_prefix_len);
    return -kDot3Result_Fail_InvalidIPv6PrefixLenValue;
  }
  return kDot3Result_Success;
}


/*
 * WSA 를 생성한다.
 */
int OPEN_API Dot3_ConstructWsa(
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  Log(kDot3LogLevel_event, "Constructing WSA\n");

  /*
   * 파라미터 체크
   */
  int ret = dot3_CheckApiParameters_ConstrutWsa(params, outbuf, outbuf_size);
  if (ret < 0) {
    return ret;
  }

  /*
   * WSA 패킷 생성
   */
  int wsa_size = dot3_ConstructWsa(&(g_dot3_mib.provider_info), params, outbuf, outbuf_size);
  if (wsa_size < 0) {
    return wsa_size;
  }

  Log(kDot3LogLevel_event, "Success to construct %d-bytes WSA\n", wsa_size);
  return wsa_size;
}


/**
 * Dot3_ParseWsa() API 의 파라미터 유효성을 검사한다.
 *
 * @param encoded_wsa           @ref Dot3_ParseWsa
 * @param encoded_wsa_size      @ref Dot3_ParseWsa
 * @param params                @ref Dot3_ParseWsa
 * @return                      성공 시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_CheckApiParameters_ParseWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params)
{
  if (!encoded_wsa || !encoded_wsa_size || !params) {
    Err("Fail to parse WSA - null parameters encoded_wsa: %p, encoded_wsa_size: %d, params: %p",
        encoded_wsa, encoded_wsa_size, params);
    return -kDot3Result_Fail_NullParameters;
  }
  return kDot3Result_Success;
}


/*
 * WSA를 파싱한다.
 */
int OPEN_API Dot3_ParseWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params)
{
  /*
   * 파라미터 체크
   */
  int ret = dot3_CheckApiParameters_ParseWsa(encoded_wsa, encoded_wsa_size, params);
  if (ret < 0) {
    return ret;
  }

  /*
   * WSA 파싱
   */
  return dot3_ParseWsa(encoded_wsa, encoded_wsa_size, params);
}
