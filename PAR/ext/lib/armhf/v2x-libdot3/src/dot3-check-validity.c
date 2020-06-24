/**
 * @file dot3-check-validity.c
 * @date 2019-06-06
 * @author gyun
 * @brief 각종 값에 대한 유효성을 체크하는 기능을 구현한 파일
 */


#include "dot3-internal.h"


/**
 * PSID가 유효한 값을 가지는지 확인한다.
 *
 * @param psid 유효성을 체크할 psid 값
 * @return 유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidPsidValue(const Dot3Psid psid)
{
  if (psid <= kDot3Psid_Max) {
    return true;
  }
  return false;
}


/**
 * Priority가 유효한 값을 가지는지 확인한다.
 *
 * @param priority 유효성을 체크할 priority 값
 * @return 유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidPriorityValue(const Dot3Priority priority)
{
  if ((priority >= kDot3Priority_Min) && (priority <= kDot3Priority_Max)) {
    return true;
  }
  return false;
}


/**
 * 채널번호가 유효한 값을 가지는지 확인한다.
 *
 * @param chan_num 유효성을 체크할 채널번호 값
 * @return 유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidChannelNumberValue(const Dot3ChannelNumber chan_num)
{
  if (((chan_num >= kDot3Channel_Min) && (chan_num <= kDot3Channel_Max)) ||
      (chan_num == kDot3Channel_Any)) {
    return true;
  }
  return false;
}


/**
 * DataRate가 유효한 값을 가지는지 확인한다.
 *
 * @param datarate 유효성을 체크할 DataRate 값
 * @return 유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidDataRateValue(const Dot3DataRate datarate)
{
  if ((datarate < kDot3DataRate_Min) || (datarate > kDot3DataRate_27Mbps)) {
    return false;
  }
  bool ret;
  switch(datarate) {
    case kDot3DataRate_3Mbps:
    case kDot3DataRate_4p5Mbps:
    case kDot3DataRate_6Mbps:
    case kDot3DataRate_9Mbps:
    case kDot3DataRate_12Mbps:
    case kDot3DataRate_18Mbps:
    case kDot3DataRate_24Mbps:
    case kDot3DataRate_27Mbps: {
      ret = true;
      break;
    }
    default: {
      ret = false;
      break;
    }
  }
  return ret;
}


/**
 * 파워가 유효한 값을 가지는지 확인한다.
 *
 * @param power 유효성을 체크할 파워 값
 * @return 유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidPowerValue(const Dot3Power power)
{
  if ((power >= kDot3Power_Min) && (power <= kDot3Power_Max)) {
    return true;
  }
  return false;
}


/**
 * ProviderChannelAccess 값이 유효한 값을 가지는지 확인한다.
 *
 * @param chan_access   유효성을 체크할 channel access 값
 * @return              유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidProviderChannelAccess(const Dot3ProviderChannelAccess chan_access)
{
  if (chan_access > kDot3ProviderChannelAccess_Max) {
    return false;
  }
  return true;
}


/**
 * ProviderServiceContext의 길이가 유효한지 확인한다.
 *
 * @param psc_len   유효성을 체크할 길이 값
 * @return          유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidPscLen(const uint8_t psc_len)
{
  if (psc_len > kDot3PscLen_Max) {
    return false;
  }
  return true;
}


/**
 * WsaCountThresholdInterval의 값이 유효한지 확인한다.
 *
 * @param interval  유효성을 체크할 값
 * @return          유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidWsaCountThresholdIntervalValue(const uint8_t interval)
{
  if (interval == 0) {
    return false;
  }
  return true;
}


/**
 * WsaIdentifier 값이 유효한지 확인한다.
 *
 * @param wsa_id    유효성을 체크할 값
 * @return          유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidWsaIdValue(const Dot3WsaIdentifier wsa_id)
{
  if (wsa_id > kDot3WsaMaxId) {
    return false;
  }
  return true;
}


/**
 * WsaContentCount 값이 유효한지 확인한다.
 *
 * @param content_cnt    유효성을 체크할 값
 * @return               유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidWsaContentCountValue(const Dot3WsaContentCount content_cnt)
{
  if (content_cnt > kDot3WsaMaxContentCount) {
    return false;
  }
  return true;
}


/**
 * WsaAdvertiserId 의 길이가 유효한지 확인한다.
 *
 * @param len    유효성을 체크할 값
 * @return       유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidWsaAdvertiserIdLen(const uint8_t len)
{
  if ((len == 0) || (len > kDot3WsaAdvertiserIdLen_Max)) {
    return false;
  }
  return true;
}


/**
 * Latitude 값이 유효한지 확인한다.
 *
 * @param latitude    유효성을 체크할 값
 * @return            유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidLatitudeValue(const Dot3Latitude latitude)
{
  if ((latitude < kDot3Latitude_Min) || (latitude > kDot3Latitude_Max)) {
    return false;
  }
  return true;
}


/**
 * Longitude 값이 유효한지 확인한다.
 *
 * @param longitude     유효성을 체크할 값
 * @return              유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidLongitudeValue(const Dot3Longitude longitude)
{
  if ((longitude < kDot3Longitude_Min) || (longitude > kDot3Longitude_Max)) {
    return false;
  }
  return true;
}


/**
 * Elevation 값이 유효한지 확인한다.
 *
 * @param elevation     유효성을 체크할 값
 * @return              유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidElevationValue(const Dot3Elevation elevation)
{
  if ((elevation < kDot3Elevation_Min) || (elevation > kDot3Elevation_Max)) {
    return false;
  }
  return true;
}


/**
 * IPv6 prefix length 값이 유효한지 확인한다.
 *
 * @param len    유효성을 체크할 값
 * @return       유효할 경우 true, 유효하지 않을 경우 false
 */
bool INTERNAL dot3_IsValidIPv6PrefixLenValue(const Dot3IPv6PrefixLen len)
{
  if ((len < kDot3IPv6PrefixLen_Min) || (len > kDot3IPv6PrefixLen_Max)) {
    return false;
  }
  return true;
}



