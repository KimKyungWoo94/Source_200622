/**
 * @file wlanaccess-16094.h
 * @date 2019-05-14
 * @author gyun
 * @brief wlanaccess 라이브러리 내에서 사용되는 1609.4 관련 정의 헤더파일
 */


#ifndef LIBWLANACCESS_WLANACCESS_16094_H
#define LIBWLANACCESS_WLANACCESS_16094_H

/// 채널스위칭 관련 인터벌
enum eDot4Interval {
  kDot4Interval_Continuous = 0,
  kDot4Interval_DefaultTsDuration = 50,
  kDot4Interval_DefaultSyncTolerance = 2,
  kDot4Interval_DefaultMaxSwitchingTime = 2,
  kDot4Interval_DefaultGuardInterval = (kDot4Interval_DefaultSyncTolerance+kDot4Interval_DefaultMaxSwitchingTime)
};

#endif //LIBWLANACCESS_WLANACCESS_16094_H
