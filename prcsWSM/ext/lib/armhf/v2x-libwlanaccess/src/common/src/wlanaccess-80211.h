/**
 * @file wlanaccess-80211.h
 * @date 2019-05-09
 * @author gyun
 * @brief wlanaccess 라이브러리 내에서 사용되는 802.11 관련 정의 헤더파일
 */


#ifndef LIBWLANACCESS_WLANACCESS_80211_H
#define LIBWLANACCESS_WLANACCESS_80211_H


#include <stdint.h>


/// aSlotTime(usec) - per 802.11-2012 Table 18-17
enum eDot11aSlotTime {
  kDot11aSlotTime_5M = 21,
  kDot11aSlotTime_10M = 13,
  kDot11aSlotTime_20M = 9,
};

/// aSIFSTime(usec) - per 802.11-2012 Table 18-17
enum eDot11aSIFSTime {
  kDot11aSIFSTime_5M = 64,
  kDot11aSIFSTime_10M = 32,
  kDot11aSIFSTime_20M = 16
};

/// aPreambleLength(usec) - per 802.11-2012 Table 18-17
enum eDot11aPreambleLength {
  kDot11aPreambleLength_5M = 64,
  kDot11aPreambleLength_10M = 32,
  kDot11aPreambleLength_20M = 16
};

/// aPLCPHeaderLength(usec) - per 802.11-2012 Table 18-17
enum eDot11aPLCPHeaderLength {
  kDot11aPLCPHeaderLength_5M = 16,
  kDot11aPLCPHeaderLength_10M = 8,
  kDot11aPLCPHeaderLength_20M = 4
};

/// DIFS(usec) - per 802.11-2012 p.843
///
/// DIFS(usec) = aSIFSTime + (2 x aSlotTime)
enum eDot11DIFS {
  kDot11DIFS_5M = (kDot11aSIFSTime_5M + (2*kDot11aSlotTime_5M)),    // 64 + (2*21) = 106
  kDot11DIFS_10M = (kDot11aSIFSTime_10M + (2*kDot11aSlotTime_10M)), // 32 + (2*13) = 58
  kDot11DIFS_20M = (kDot11aSIFSTime_20M + (2*kDot11aSlotTime_20M))  // 16 + (2*9) = 34
};

/// @brief ACKTxTime(usec) - per 802.11-2012 p.844
///
/// ACKTxTime = (preamble + PLCP header + ACK MPDU) 전송시간
///  - ACK MPDU 전송시간 : lowest PHY mandatory rate 기준으로 계산
///  - ACK MPDU = 112bit -> 5M: 112/(1.5*10^6)=75us, 10M: 112/(3*10^6)=38us, 20M: 112/(6*10^6)=19us
enum eDot11ACKTxTime {
  kDot11ACKTxTime_5M = (kDot11aPreambleLength_5M + kDot11aPLCPHeaderLength_5M + 75),    // 64 + 16 + 75 = 155
  kDot11ACKTxTime_10M = (kDot11aPreambleLength_10M + kDot11aPLCPHeaderLength_10M + 38), // 32 + 8 + 38 = 78
  kDot11ACKTxTime_20M = (kDot11aPreambleLength_20M + kDot11aPLCPHeaderLength_20M + 19), // 16 + 4 + 19 = 39
};

/// EIFS(usec) = aSIFSTime + DIFS + ACKTxTime - per 802.11-2012 p.843
enum eDot11EIFS {
  kDot11EIFS_5M = (kDot11aSIFSTime_5M + kDot11DIFS_5M + kDot11ACKTxTime_5M),      // 64 + 106 + 155 = 325
  kDot11EIFS_10M = (kDot11aSIFSTime_10M + kDot11DIFS_10M + kDot11ACKTxTime_10M),  // 32 + 58 + 78 = 168
  kDot11EIFS_20M = (kDot11aSIFSTime_20M + kDot11DIFS_20M + kDot11ACKTxTime_20M)   // 16 + 34 + 39 = 89
};

/// Contention Window 값
enum eDot11EdcaParam {
  kDot11_aCWmin = 15,   /// aCWmin - per 802.11-2012 Table 18-17
  kDot11_aCWmax = 1023, /// aCWmax - per 802.11-2012 Table 18-17
  kDot11_OCB_CWmin_BK = kDot11_aCWmin,  /// =15. CWmin for OCB/AC_BK - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmax_BK = kDot11_aCWmax,  /// =1023. CWmax for OCB/AC_BK - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmin_BE = kDot11_aCWmin,  /// =15. CWmin for OCB/AC_BE - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmax_BE = kDot11_aCWmax,  /// =1023. CWmax for OCB/AC_BE - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmin_VI = (kDot11_aCWmin + 1) / 2 - 1,  /// =7. CWmin for OCB/AC_VI - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmax_VI = kDot11_aCWmin,                /// =15. CWmax for OCB/AC_VI - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmin_VO = (kDot11_aCWmin + 1) / 4 - 1,  /// =3. CWmin for OCB/AC_VO - per 802.11-2012 Table 8-106
  kDot11_OCB_CWmax_VO = (kDot11_aCWmin + 1) / 2 - 1,  /// =7. CWmax for OCB/AC_VO - per 802.11-2012 Table 8-106
  kDot11_OCB_TXOP = 0,      /// TXOP for OCB - per 802.11-2012 Table 8-106
  kDot11_OCB_AIFSN_BK = 9,  /// AIFSN for OCB/AC_BK - per 802.11-2012 Table 8-106
  kDot11_OCB_AIFSN_BE = 6,  /// AIFSN for OCB/AC_BE - per 802.11-2012 Table 8-106
  kDot11_OCB_AIFSN_VI = 3,  /// AIFSN for OCB/AC_VI - per 802.11-2012 Table 8-106
  kDot11_OCB_AIFSN_VO = 2,  /// AIFSN for OCB/AC_VI - per 802.11-2012 Table 8-106
};


#endif //LIBWLANACCESS_WLANACCESS_80211_H
