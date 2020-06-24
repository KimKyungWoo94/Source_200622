/**
 * @file saf5100.c
 * @date 2019-05-08
 * @author gyun
 * @brief SAF5100 플랫폼 의존 코드 구현 파일
 */


#include <inttypes.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#include "wlanaccess-16094.h"
#include "wlanaccess-80211.h"
#include "wlanaccess-internal.h"

#include "debug-app.h"
#include "llc.h"
#include "llc-api.h"
#include "mk2mac-api-types.h"
#include "pktbuf-app.h"

#include "saf5100.h"


/*
 * 개별/그룹 MAC주소 확인 매크로
 */
#define DOT11_GET_MAC_ADDR_IG(addr) (addr[0]&1)
#define DOT11_MAC_ADDR_IG_INDIVIDUAL 0
#define DOT11_MAC_ADDR_IG_GROUP 1

/// 플랫폼 내 SAF5100 디바이스의 개수 (빌드 시 전달된 _V2X_IF_NUM_ 값에 의해 결정된다)
uint8_t g_saf5100_dev_num = (_V2X_IF_NUM_ / SAF5100_IF_NUM_IN_DEV) + (_V2X_IF_NUM_ % SAF5100_IF_NUM_IN_DEV);

/*
 * libLLC 내 로그메시지 처리를 위한 정의
 *  - libLLC 내에 정의되어 있는 D_LOCAL을 D_ALL로 변경하여, D_LEVEL 변수만을 이용해 libLLC 로그메시지 출력레벨울 제어하도록 한다.
 *  - D_LEVEL : libLLC 에서 로그메시지 출력레벨 설정을 위해 사용되는 전역 변수이며,
 *    라이브러리 내부에 선언되어 있지 않으므로 여기서 선언을 해 주어야 한다.
 */
#undef D_LOCAL
#define D_LOCAL D_ALL;
int D_LEVEL = D_ALL; //3;

struct SAF5100Platform g_al_saf5100_platform; ///< SAF5100 플랫폼 정보
struct MKxRadioConfigData g_al_default_radio_cfg_data = {  /// SAF5100 디바이스의 라디오 기본 설정 값
  .Mode = MKX_MODE_OFF,

  // TimeSlot0 설정
  .ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq = 0,
  .ChanConfig[MKX_CHANNEL_0].PHY.Bandwidth = MK2BW_10MHz, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_0].PHY.TxAntenna = MKX_ANT_1,
  .ChanConfig[MKX_CHANNEL_0].PHY.RxAntenna = MKX_ANT_1,
  .ChanConfig[MKX_CHANNEL_0].PHY.DefaultMCS = MK2MCS_R12QPSK,
  .ChanConfig[MKX_CHANNEL_0].PHY.DefaultTxPower = 40, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_0].MAC.DualTxControl = MKX_TXC_TXRX, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_0].MAC.CSThreshold = -65, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_0].MAC.SlotTime = kDot11aSlotTime_10M,
  .ChanConfig[MKX_CHANNEL_0].MAC.DIFSTime = kDot11DIFS_10M,
  .ChanConfig[MKX_CHANNEL_0].MAC.SIFSTime = kDot11aSIFSTime_10M,
  .ChanConfig[MKX_CHANNEL_0].MAC.EIFSTime = kDot11EIFS_10M - kDot11DIFS_10M, // EIFS - DIFS
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_NON_QOS].AIFS = kDot11_OCB_AIFSN_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_NON_QOS].CWMIN = kDot11_OCB_CWmin_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_NON_QOS].CWMAX = kDot11_OCB_CWmax_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_NON_QOS].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VO].AIFS = kDot11_OCB_AIFSN_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VO].CWMIN = kDot11_OCB_CWmin_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VO].CWMAX = kDot11_OCB_CWmax_VO,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VO].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VI].AIFS = kDot11_OCB_AIFSN_VI,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VI].CWMIN = kDot11_OCB_CWmin_VI,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VI].CWMAX = kDot11_OCB_CWmax_VI,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_VI].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BE].AIFS = kDot11_OCB_AIFSN_BE,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BE].CWMIN = kDot11_OCB_CWmin_BE,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BE].CWMAX = kDot11_OCB_CWmax_BE,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BE].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BK].AIFS = kDot11_OCB_AIFSN_BK,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BK].CWMIN = kDot11_OCB_CWmin_BK,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BK].CWMAX = kDot11_OCB_CWmax_BK,
  .ChanConfig[MKX_CHANNEL_0].MAC.TxQueue[MKX_TXQ_AC_BK].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[0].Addr = 0XFFFFFFFFFFFFULL, // 브로드캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[0].Mask = 0XFFFFFFFFFFFFULL,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[0].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[1].Addr = 0x000000003333ULL, // IPv6 멀티캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[1].Mask = 0x00000000FFFFULL,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[1].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[2].Addr = 0x0000005E0001ULL, // IPv4 멀티캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[2].Mask = 0x000080FFFFFFULL,
  .ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[2].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_0].MAC.ShortRetryLimit = 7, // per 802.11-2012 p.2134
  .ChanConfig[MKX_CHANNEL_0].MAC.LongRetryLimit = 4,  // per 802.11-2012 p.2134
  .ChanConfig[MKX_CHANNEL_0].MAC.RTSCTSThreshold = 32767, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_0].LLC.GuardDuration = (kDot4Interval_DefaultGuardInterval*1000),
  .ChanConfig[MKX_CHANNEL_0].LLC.IntervalDuration = (kDot4Interval_Continuous*1000),

  // TimeSlot1 설정
  .ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq = 0,
  .ChanConfig[MKX_CHANNEL_1].PHY.Bandwidth = MK2BW_10MHz, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_1].PHY.TxAntenna = MKX_ANT_1,
  .ChanConfig[MKX_CHANNEL_1].PHY.RxAntenna = MKX_ANT_1,
  .ChanConfig[MKX_CHANNEL_1].PHY.DefaultMCS = MK2MCS_R12QPSK,
  .ChanConfig[MKX_CHANNEL_1].PHY.DefaultTxPower = 40, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_1].MAC.DualTxControl = MKX_TXC_TXRX, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_1].MAC.CSThreshold = -65, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_1].MAC.SlotTime = kDot11aSlotTime_10M,
  .ChanConfig[MKX_CHANNEL_1].MAC.DIFSTime = kDot11DIFS_10M,
  .ChanConfig[MKX_CHANNEL_1].MAC.SIFSTime = kDot11aSIFSTime_10M,
  .ChanConfig[MKX_CHANNEL_1].MAC.EIFSTime = kDot11EIFS_10M - kDot11DIFS_10M, // EIFS - DIFS
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_NON_QOS].AIFS = kDot11_OCB_AIFSN_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_NON_QOS].CWMIN = kDot11_OCB_CWmin_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_NON_QOS].CWMAX = kDot11_OCB_CWmax_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_NON_QOS].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VO].AIFS = kDot11_OCB_AIFSN_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VO].CWMIN = kDot11_OCB_CWmin_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VO].CWMAX = kDot11_OCB_CWmax_VO,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VO].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VI].AIFS = kDot11_OCB_AIFSN_VI,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VI].CWMIN = kDot11_OCB_CWmin_VI,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VI].CWMAX = kDot11_OCB_CWmax_VI,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_VI].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BE].AIFS = kDot11_OCB_AIFSN_BE,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BE].CWMIN = kDot11_OCB_CWmin_BE,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BE].CWMAX = kDot11_OCB_CWmax_BE,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BE].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BK].AIFS = kDot11_OCB_AIFSN_BK,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BK].CWMIN = kDot11_OCB_CWmin_BK,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BK].CWMAX = kDot11_OCB_CWmax_BK,
  .ChanConfig[MKX_CHANNEL_1].MAC.TxQueue[MKX_TXQ_AC_BK].TXOP = kDot11_OCB_TXOP,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[0].Addr = 0XFFFFFFFFFFFFULL, // 브로드캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[0].Mask = 0XFFFFFFFFFFFFULL,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[0].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[1].Addr = 0x000000003333ULL, // IPv6 멀티캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[1].Mask = 0x00000000FFFFULL,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[1].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[2].Addr = 0x0000005E0001ULL, // IPv4 멀티캐스트 수신 & 미응답 설정
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[2].Mask = 0x000080FFFFFFULL,
  .ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[2].MatchCtrl = 0,
  .ChanConfig[MKX_CHANNEL_1].MAC.ShortRetryLimit = 7, // per 802.11-2012 p.2134
  .ChanConfig[MKX_CHANNEL_1].MAC.LongRetryLimit = 4,  // per 802.11-2012 p.2134
  .ChanConfig[MKX_CHANNEL_1].MAC.RTSCTSThreshold = 32767, // from mk5CfgDual.cfg
  .ChanConfig[MKX_CHANNEL_1].LLC.GuardDuration = (kDot4Interval_DefaultGuardInterval*1000),
  .ChanConfig[MKX_CHANNEL_1].LLC.IntervalDuration = (kDot4Interval_Continuous*1000),
};


/**
 * "10MHz 대역폭 Datarate" 를 MCS 값으로 변환한다.
 *
 * @param datarate 데이터레이트
 * @return MCS 값
 */
static inline tMKxMCS al_SAF5100_ConvertDataRateTo10MHzMcs(const uint8_t datarate)
{
  tMKxMCS ret;
  switch(datarate) {
    case 6: { ret = MKXMCS_R12BPSK; break; }
    case 9: { ret = MKXMCS_R34BPSK; break; }
    case 12: { ret = MKXMCS_R12QPSK; break; }
    case 18: { ret = MKXMCS_R34QPSK; break; }
    case 24: { ret = MKXMCS_R12QAM16; break; }
    case 36: { ret = MKXMCS_R34QAM16; break; }
    case 48: { ret = MKXMCS_R23QAM64; break; }
    case 54: { ret = MKXMCS_R34QAM64; break; }
    default: { ret = MKXMCS_DEFAULT; break; }
  }
  return ret;
}


/**
 * "20MHz 대역폭 Datarate" 를 MCS 값으로 변환한다.
 *
 * @param datarate 데이터레이트
 * @return MCS 값
 */
static inline tMKxMCS al_SAF5100_ConvertDataRateTo20MHzMcs(const uint8_t datarate)
{
  tMKxMCS ret;
  switch(datarate) {
    case 12: { ret = MKXMCS_R12BPSK; break; }
    case 18: { ret = MKXMCS_R34BPSK; break; }
    case 24: { ret = MKXMCS_R12QPSK; break; }
    case 36: { ret = MKXMCS_R34QPSK; break; }
    case 48: { ret = MKXMCS_R12QAM16; break; }
    case 72: { ret = MKXMCS_R34QAM16; break; }
    case 96: { ret = MKXMCS_R23QAM64; break; }
    case 108: { ret = MKXMCS_R34QAM64; break; }
    default: { ret = MKXMCS_DEFAULT; break; }
  }
  return ret;
}


/**
 * MCS 값을 "10MHz 대역폭 Datarate" 로 변환한다.
 *
 * @param MCS 값
 * @return datarate 데이터레이트 (0: Unknown)
 */
uint8_t INTERNAL al_SAF5100_Convert10MhzMcsToDataRate(const tMKxMCS mcs)
{
  uint8_t ret;
  switch(mcs) {
    case MKXMCS_R12BPSK: { ret = 6; break; }
    case MKXMCS_R34BPSK: { ret = 9; break; }
    case MKXMCS_R12QPSK: { ret = 12; break; }
    case MKXMCS_R34QPSK: { ret = 18; break; }
    case MKXMCS_R12QAM16: { ret = 24; break; }
    case MKXMCS_R34QAM16: { ret = 36; break; }
    case MKXMCS_R23QAM64: { ret = 48; break; }
    case MKXMCS_R34QAM64: { ret = 54; break; }
    default: { ret = 0; break; }
  }
  return ret;
}


/**
 * MCS 값을 "20MHz 대역폭 Datarate" 로 변환한다.
 *
 * @param MCS 값
 * @return datarate 데이터레이트 (0: Unknown)
 */
uint8_t INTERNAL al_SAF5100_Convert20MhzMcsToDataRate(const tMKxMCS mcs)
{
  uint8_t ret;
  switch(mcs) {
    case MKXMCS_R12BPSK: { ret = 12; break; }
    case MKXMCS_R34BPSK: { ret = 18; break; }
    case MKXMCS_R12QPSK: { ret = 24; break; }
    case MKXMCS_R34QPSK: { ret = 36; break; }
    case MKXMCS_R12QAM16: { ret = 48; break; }
    case MKXMCS_R34QAM16: { ret = 72; break; }
    case MKXMCS_R23QAM64: { ret = 96; break; }
    case MKXMCS_R34QAM64: { ret = 108; break; }
    default: { ret = 0; break; }
  }
  return ret;
}


/**
 * SAF5100 디바이스의 각 라디오별(0,1) 기본설정을 로딩한다.
 * 전역변수로 정의된 기본설정값들을 인자로 전달된 "라디오설정정보"에 복사한다.
 *
 * @param data 기본값으로 설정될 "라디오설정정보" 포인터
 */
static inline void al_SAF5100_LoadDefaultRadioConfigData(struct MKxRadioConfigData *const data)
{
  memcpy(data, &g_al_default_radio_cfg_data, sizeof(g_al_default_radio_cfg_data));
}


/**
 * LLC 내부의 라디오설정정보에 대한 참조를 반환한다. (읽기 위해)
 *
 * @param mkx SAF5100 디바이스의 MKx 핸들
 * @param ifindex_in_dev SAF5100 디바이스 내 인터페이스 식별번호
 * @return 라디오설정데이터 포인터
 */
const struct MKxRadioConfigData INTERNAL
* al_SAF5100_GetCurrentRadioConfigData(const struct MKx *const mkx, const AlIfIndex ifindex_in_dev)
{
  return &(mkx->Config.Radio[ifindex_in_dev]);
}


/**
 * LLC 라이브러리 내부에 저장되어 있는 라디오설정정보를 복사한다. (수정 후 재설정하기 위해)
 * @param mkx SAF5100 디바이스의 MKx 핸들
 * @param ifindex_in_dev SAF5100 디바이스 내 인터페이스 식별번호
 * @param radio_cfg_data 라디오설정데이터가 복사될 구조체 포인터
 */
static inline void al_SAF5100_CopyCurrentRadioConfigData(
  const struct MKx *const mkx,
  const AlIfIndex ifindex_in_dev,
  struct MKxRadioConfigData *const radio_cfg_data)
{
  memcpy(radio_cfg_data, &(mkx->Config.Radio[ifindex_in_dev]), sizeof(struct MKxRadioConfigData));
}


/**
 * Datarate 값을 MCS 값으로 변환한다.
 *
 * @param mkx SAF5100 디바이스의 MKx 핸들
 * @param ifindex_in_dev SAF5100 디바이스 내 인터페이스 식별번호
 * @param datarate 데이터레이트
 * @return MCS 값
 */
static tMKxMCS
al_SAF5100_ConvertDataRateToMcs(
  const struct MKx *const mkx,
  const AlIfIndex ifindex_in_dev,
  const uint8_t datarate)
{
  const struct MKxRadioConfigData *radio_cfg_data = al_SAF5100_GetCurrentRadioConfigData(mkx, ifindex_in_dev);

  // 접속 중인 채널 대역폭에 따라 Datarate를 MCS로 변환한다. (구현 상, TimeSlot0/1은 항상 같은 대역폭을 사용한다)
  if (radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.Bandwidth == MKXBW_10MHz) {
    return al_SAF5100_ConvertDataRateTo10MHzMcs(datarate);
  } else {  // MKXBW_20MHz
    return al_SAF5100_ConvertDataRateTo20MHzMcs(datarate);
  }
}


/**
 * SAF5100 플랫폼의 MPDU 전송 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 TransmitMpdu() 함수포인터에 연결되며, Al_TransmitMpdu() 에서 호출된다.
 *
 * @param priv          @ref TransmitMpdu
 * @param ifindex       @ref TransmitMpdu
 * @param mpdu          @ref TransmitMpdu
 * @param mpdu_size     @ref TransmitMpdu
 * @param txparams      @ref TransmitMpdu
 * @return              @ref TransmitMpdu
 */
static int al_SAF5100_TransmitMpdu(
  const void *const priv,
  const AlIfIndex ifindex,
  const uint8_t *const mpdu,
  const AlMpduSize mpdu_size,
  const struct AlMpduTxParams *const txparams)
{
  uint8_t dev_index = (ifindex / SAF5100_IF_NUM_IN_DEV);      // 디바이스 식별번호
  AlIfIndex ifindex_in_dev = ifindex % SAF5100_IF_NUM_IN_DEV; // 디바이스 내에서의 인터페이스 식별번호
  struct SAF5100Platform *saf5100_platform = (struct SAF5100Platform *)priv;
  struct MKx *mkx = saf5100_platform->dev[dev_index].mkx;

  Log(kAlLogLevel_event, "Transmitting MPDU - ifindex:%u -> dev_index: %u, ifindex_in_dev: %u\n",
      ifindex, dev_index, ifindex_in_dev);

  /*
   * 파라미터 체크
   *  - 널 파라미터
   *  - 디바이스에서 지원하는 인터페이스 범위를 확인한다.
   *  - TimeSlot: 값의 유효성을 확인한다.
   *  - 채널번호: 명시된 ifindex/TimeSlot에 명시된 채널이 실제 접속 중인지 확인한다.
   *  - 송신파워: 범위를 벗어날 경우 디바이스에서 Saturation 되므로 체크하지 않는다.
   *  - 데이터레이트: 유효하지 않은 값일 경우, Default 값으로 설정되므로 체크하지 않는다.
   */
  if (!mpdu || !txparams) {
    Err("Fail to transmit MPDU. null parameters - mpdu: %p, txparams: %p\n", mpdu, txparams);
    return -kAlResult_NullParameters;
  }
  if (ifindex >= g_al_saf5100_platform.if_num) {
    Err("Fail to transmit MPDU. Invalid ifindex: %u\n", ifindex);
    return -kAlResult_InvalidIfIndex;
  }
  if ((mpdu_size < kAlMpduMinSize) || (mpdu_size > kAlMpduMaxSize)) {
    Err("Fail to transmit MPDU. Invalid mpdu_size: %u\n", mpdu_size);
    return -kAlResult_InvalidMpduSize;
  }
  if (txparams->timeslot > kAlTimeSlot_max) {
    Err("Fail to transmit MPDU. Invalid timeslot: %u\n", txparams->timeslot);
    return -kAlResult_InvalidTimeSlot;
  }
  uint16_t freq = al_ConvertChannelNumberToFreq(txparams->channel);
  tMKxChannel timeslot = (txparams->timeslot == kAlTimeSlot_1) ? MKX_CHANNEL_1 : MKX_CHANNEL_0;
  const struct MKxRadioConfigData *radio_cfg_data = al_SAF5100_GetCurrentRadioConfigData(mkx, ifindex_in_dev);
  if (freq != radio_cfg_data->ChanConfig[timeslot].PHY.ChannelFreq) {
    Err("Fail to transmit MPDU. Invalid channel: %u(%u). Current freq - ts0:%u, ts1:%u\n",
      txparams->channel, freq, radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq,
        radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq);
    return -kAlResult_InvalidChannel;
  }

  /*
   * 송신패킷을 위한 패킷버퍼 메모리를 할당한다.
   */
  struct PktBuf *pbuf = PktBuf_Alloc(
    LLC_DEV_HEADROOM + LLC_DEV_TAILROOM + sizeof(struct MKxTxPacket) + mpdu_size);
  if (pbuf == NULL) {
    Err("Fail to transmit MPDU. PktBuf_Alloc() failed.\n");
    return -kAlResult_NoMemory;
  }
  PktBuf_Reserve(pbuf, LLC_DEV_HEADROOM);
  struct MKxTxPacket *txpkt = (struct MKxTxPacket *)PktBuf_Put(pbuf, sizeof(struct MKxTxPacket) + mpdu_size);
  struct MKxTxPacketData *txpktdata = &(txpkt->TxPacketData);

  /*
   * 패킷송신정보를 설정한다.
   */
  txpktdata->RadioID = (tMKxRadio)ifindex_in_dev;
  txpktdata->ChannelID = (tMKxChannel)(txparams->timeslot);
  txpktdata->TxAntenna = MKX_ANT_DEFAULT;  // Default: Config() 에 의해 설정되어 있는 안테나 사용
  txpktdata->MCS = al_SAF5100_ConvertDataRateToMcs(mkx, ifindex_in_dev, txparams->datarate);
  txpktdata->TxPower = (tMKxPower)txparams->txpower;
  txpktdata->TxCtrlFlags = 0; // 0: 일반동작
  txpktdata->Expiry = 0; // TODO:: 현재 TSF 읽어와서 txparams->expiry를 더한 TSF 값을 설정하는 것으로 변경
  txpktdata->TxFrameLength = mpdu_size; // CRC 제외
  memcpy(txpktdata->TxFrame, mpdu, mpdu_size);

  Log(kAlLogLevel_event, "Transmit MPDU - RadioID: %u, ChannelID: %u, TxAntenna: %u, MCS: 0x%1X\n"
                         "                TxPower: %d, TxCtrlFlags: %u: Expiry: %"PRIu64", TxFrameLength: %u\n",
    txpktdata->RadioID, txpktdata->ChannelID, txpktdata->TxAntenna, txpktdata->MCS,
    txpktdata->TxPower, txpktdata->TxCtrlFlags, txpktdata->Expiry, txpktdata->TxFrameLength);
  if (g_al_log >= kAlLogLevel_dump) {
    al_PrintPacketDump(txpktdata->TxFrame, txpktdata->TxFrameLength);
  }

  /*
   * 패킷을 송신한다 -> LLC 로 전달한다.
   */
  int ret = mkx->API.Functions.TxReq(mkx, txpkt, pbuf);
  if (ret != MKXSTATUS_SUCCESS) {
    Err("Fail to access channel. TxReq() failed - eMKxStatus: %d\n", ret);
    PktBuf_Free(pbuf);
    return -kAlResult_DevSpecificError;
  }

  Log(kAlLogLevel_event, "Success to transmit MPDU\n");
  return kAlResult_Success;
}


/**
 * SAF5100 플랫폼의 채널접속 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 AccessChannel() 함수포인터에 연결되며, Al_AccessChannel() 에서 호출된다.
 *
 * @param priv          @ref AccessChannel
 * @param ifindex       @ref AccessChannel
 * @param ts0_chan      @ref AccessChannel
 * @param ts0_chan      @ref AccessChannel
 * @return              @ref AccessChannel
 */
static int al_SAF5100_AccessChannel(
  const void *const priv,
  const AlIfIndex ifindex,
  const AlChannel ts0_chan,
  const AlChannel ts1_chan)
{
  uint8_t dev_index = (ifindex / SAF5100_IF_NUM_IN_DEV);      // 디바이스 식별번호
  AlIfIndex ifindex_in_dev = ifindex % SAF5100_IF_NUM_IN_DEV; // 디바이스 내에서의 인터페이스 식별번호
  struct SAF5100Platform *saf5100_platform = (struct SAF5100Platform *)priv;
  struct MKx *mkx = saf5100_platform->dev[dev_index].mkx;

  Log(kAlLogLevel_config,
      "Accessing channel - ifindex:%u, dev_index: %u, ifindex_in_dev: %u, ts0:%u, ts1:%u\n",
      ifindex, dev_index, ifindex_in_dev, ts0_chan, ts1_chan);

  /*
   * 파라미터 체크
   *  - 디바이스에서 지원하는 인터페이스 범위를 확인한다.
   *  - 구현상, TS0와 TS1의 채널대역폭은 항상 동일해야 한다.
   */
  if (ifindex >= g_al_saf5100_platform.if_num) {
    Err("Fail to access channel. Invalid ifindex: %u\n", ifindex);
    return -kAlResult_InvalidIfIndex;
  }
  tMKxBandwidth ts0_bw = al_GetChannelNumberBandwidth(ts0_chan);
  tMKxBandwidth ts1_bw = al_GetChannelNumberBandwidth(ts1_chan);
  if (ts0_bw != ts1_bw) {
    Err("Fail to access channel. Different channel/bandwidth between timeslot - %d(%u), %d(%u)\n",
        ts0_bw, ts0_chan, ts1_bw, ts1_chan);
    return -kAlResult_InvalidChannel;
  }


  /*
   * 라이브러리 내부에 저장되어 있는 디바이스 세팅정보를 복사해 온다. (필요한 부분만 새로 업데이트 해서 세팅하기 위해)
   */
  struct MKxRadioConfig radio_cfg;
  struct MKxRadioConfigData *radio_cfg_data = &(radio_cfg.RadioConfigData);
  al_SAF5100_CopyCurrentRadioConfigData(mkx, ifindex_in_dev, radio_cfg_data);

  /*
   * TS0/TS1 채널번호에 따라 채널접속형태를 결정하고, 채널주파수 값을 설정한다.
   */
  if (ts0_chan == ts1_chan) {
    // Continuous
    if (ts0_chan != 0) {
      Log(kAlLogLevel_config, "Continuous access in channel %u (bw: %u)\n", ts0_chan, ts0_bw);
      radio_cfg_data->Mode = MKX_MODE_CHANNEL_0;
      radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq = al_ConvertChannelNumberToFreq(ts0_chan);
      radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq = al_ConvertChannelNumberToFreq(ts1_chan);
      radio_cfg_data->ChanConfig[MKX_CHANNEL_0].LLC.IntervalDuration = (kDot4Interval_Continuous*1000);
      radio_cfg_data->ChanConfig[MKX_CHANNEL_1].LLC.IntervalDuration = (kDot4Interval_Continuous*1000);
    }
    // Off (ts0_chan = ts1_chan = 0)
    else {
      Log(kAlLogLevel_config, "Channel access off\n");
      radio_cfg_data->Mode = MKX_MODE_OFF;  // Off
      radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq = 0;
      radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq = 0;
      radio_cfg_data->ChanConfig[MKX_CHANNEL_0].LLC.IntervalDuration = (kDot4Interval_Continuous*1000);
      radio_cfg_data->ChanConfig[MKX_CHANNEL_1].LLC.IntervalDuration = (kDot4Interval_Continuous*1000);
    }
  }
  // alternating
  else {
    Log(kAlLogLevel_config, "Alternating access in channel %u- %u (bw: %u)\n", ts0_chan, ts1_chan, ts0_bw);
    radio_cfg_data->Mode = MKX_MODE_SWITCHED;
    radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq = al_ConvertChannelNumberToFreq(ts0_chan);
    radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq = al_ConvertChannelNumberToFreq(ts1_chan);
    radio_cfg_data->ChanConfig[MKX_CHANNEL_0].LLC.IntervalDuration = (kDot4Interval_DefaultTsDuration*1000);
    radio_cfg_data->ChanConfig[MKX_CHANNEL_1].LLC.IntervalDuration = (kDot4Interval_DefaultTsDuration*1000);
  }

  /*
   * 채널 대역폭을 설정한다. (TS0과 TS1의 채널대역폭은 동일해야 한다)
   */
  radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.Bandwidth = ts0_bw;
  radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.Bandwidth = ts0_bw;

  /*
   * 업데이트된 설정정보를 디바이스에 세팅한다.
   */
  int ret = mkx->API.Functions.Config(mkx, ifindex_in_dev, &radio_cfg);
  if (ret != MKXSTATUS_SUCCESS){
    Err("Fail to access channel. Config() failed - eMKxStatus: %d\n", ret);
    return -kAlResult_DevSpecificError;
  }

  saf5100_platform->dev[dev_index].req[ifindex_in_dev] = kSAF5100Req_AccessChannel;

  Log(kAlLogLevel_config, "Success to access channel\n");
  return kAlResult_Success;
}


/**
 * SAF5100 플랫폼의 접속채널확인 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 GetCurrentChannel() 함수포인터에 연결되며,
 * Al_GetCurrentChannel() 에서 호출된다.
 * LLC 라이브러리 내에 저장되어 있는 채널접속정보를 반환한다.
 *
 * @param priv          @ref GetCurrentChannel
 * @param ifindex       @ref GetCurrentChannel
 * @param ts0_chan      @ref GetCurrentChannel
 * @param ts0_chan      @ref GetCurrentChannel
 * @return              @ref GetCurrentChannel
 */
static int al_SAF5100_GetCurrentChannel(
  const void *const priv,
  const AlIfIndex ifindex,
  AlChannel *const ts0_chan,
  AlChannel *const ts1_chan)
{
  uint8_t dev_index = (ifindex / SAF5100_IF_NUM_IN_DEV);      // 디바이스 식별번호
  AlIfIndex ifindex_in_dev = ifindex % SAF5100_IF_NUM_IN_DEV; // 디바이스 내에서의 인터페이스 식별번호
  struct SAF5100Platform *saf5100_platform = (struct SAF5100Platform *)priv;
  struct MKx *mkx = saf5100_platform->dev[dev_index].mkx;

  Log(kAlLogLevel_config,
      "Get current channel - ifindex:%u, dev_index: %u, ifindex_in_dev: %u\n", ifindex, dev_index, ifindex_in_dev);

  /*
   * 파라미터 체크
   *  - 널 포인터 체크한다.
   *  - 디바이스에서 지원하는 인터페이스 범위를 확인한다.
   */
  if (!ts0_chan || !ts1_chan) {
    Err("Fail to get current channel. null parameters - ts0_chan: %p, ts1_chan: %p\n", ts0_chan, ts1_chan);
    return -kAlResult_NullParameters;
  }
  if (ifindex >= g_al_saf5100_platform.if_num) {
    Err("Fail to get current channel. Invalid ifindex: %u\n", ifindex);
    return -kAlResult_InvalidIfIndex;
  }

  /*
   * 라이브러리 내부에 저장되어 있는 디바이스 세팅정보를 복사해 온다.
   */
  struct MKxRadioConfig radio_cfg;
  struct MKxRadioConfigData *radio_cfg_data = &(radio_cfg.RadioConfigData);
  al_SAF5100_CopyCurrentRadioConfigData(mkx, ifindex_in_dev, radio_cfg_data);

  /*
   * 반환한다.
   */
  *ts0_chan = al_ConvertFreqToChannelNumber(radio_cfg_data->ChanConfig[MKX_CHANNEL_0].PHY.ChannelFreq);
  *ts1_chan = al_ConvertFreqToChannelNumber(radio_cfg_data->ChanConfig[MKX_CHANNEL_1].PHY.ChannelFreq);

  Log(kAlLogLevel_config, "Success to get current channel - ts0: %u, ts1: %u\n", *ts0_chan, *ts1_chan);
  return kAlResult_Success;
}


/**
 * SAF5100 플랫폼의 채널접속해제 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 ReleaseChannel() 함수포인터에 연결되며, Al_ReleaseChannel() 에서 호출된다.
 *
 * @param priv          @ref ReleaseChannel
 * @param ifindex       @ref ReleaseChannel
 * @param timeslot      @ref ReleaseChannel
 * @return              @ref ReleaseChannel
 */
static int al_SAF5100_ReleaseChannel(const void *const priv, const AlIfIndex ifindex, const AlTimeSlot timeslot)
{
  Log(kAlLogLevel_config, "Release channel - ts: %u\n", timeslot);
  return -kAlResult_NotSupportedYet;
}


/**
 * SAF5100 플랫폼의 MAC 주소 설정 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 SetIfMacAddress() 함수포인터에 연결되며, Al_SetIfMacAddress() 에서 호출된다.
 *
 * @param priv          @ref SetIfMacAddress
 * @param ifindex       @ref SetIfMacAddress
 * @param addr           @ref SetIfMacAddress
 * @return              @ref SetIfMacAddress
 */
static int al_SAF5100_SetIfMacAddress(const void *const priv, const AlIfIndex ifindex, const AlMacAddress addr)
{
  uint8_t dev_index = (ifindex / SAF5100_IF_NUM_IN_DEV);      // 디바이스 식별번호
  AlIfIndex ifindex_in_dev = ifindex % SAF5100_IF_NUM_IN_DEV; // 단일 모듈 내에서의 인터페이스 식별번호
  struct SAF5100Platform *saf5100_platform = (struct SAF5100Platform *)priv;
  struct MKx *mkx = saf5100_platform->dev[dev_index].mkx;

  Log(kAlLogLevel_config,
      "Set interface MAC address - ifindex: %u, dev_index: %u, ifindex_in_dev: %u, "
      "addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
      ifindex, dev_index, ifindex_in_dev, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  /*
   * 파라미터 체크
   *  - 디바이스에서 지원하는 인터페이스 범위를 확인한다.
   *  - MAC주소 유효성 확인
   */
  if (ifindex >= g_al_saf5100_platform.if_num) {
    Err("Fail to set interface MAC address. Invalid ifindex: %u\n", ifindex);
    return -kAlResult_InvalidIfIndex;
  }
  if (DOT11_GET_MAC_ADDR_IG(addr) == DOT11_MAC_ADDR_IG_GROUP) {
    Err("Fail to set interface MAC address. It's group address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    return -kAlResult_InvalidMacAddress;
  }

  /*
   * 현재 디바이스에 세팅되어 있는 설정정보를 복사해 온다. (필요한 부분만 새로 업데이트 해서 세팅하기 위해)
   */
  struct MKxRadioConfig radio_cfg;
  struct MKxRadioConfigData *radio_cfg_data = &(radio_cfg.RadioConfigData);
  al_SAF5100_CopyCurrentRadioConfigData(mkx, ifindex_in_dev, radio_cfg_data);

  /*
   * MAC 주소를 AMSTable[3] 에 세팅한다. (ACK 응답 활성화 & 마지막 엔트리임을 표시)
   *  - 참고: AMSTable[0]은 브로드캐스트, [1]은 IPv6 멀티캐스트, [2]는 IPv4 멀티캐스트용으로 사용된다.
   *  - TimeSlot0과 TimeSlot1에 대해 동일하게 설정된다.
   */
  radio_cfg_data->ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[3].Mask = 0XFFFFFFFFFFFFULL;
  radio_cfg_data->ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[3].Mask = 0XFFFFFFFFFFFFULL;
  radio_cfg_data->ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[3].Addr =
    ((uint64_t)addr[5] << 40) | ((uint64_t)addr[4] << 32) |
    ((uint64_t)addr[3] << 24) | ((uint64_t)addr[2] << 16) |
    ((uint64_t)addr[1] << 8) | ((uint64_t)addr[0] << 0);
  radio_cfg_data->ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[3].Addr =
    ((uint64_t)addr[5] << 40) | ((uint64_t)addr[4] << 32) |
    ((uint64_t)addr[3] << 24) | ((uint64_t)addr[2] << 16) |
    ((uint64_t)addr[1] << 8) | ((uint64_t)addr[0] << 0);
  radio_cfg_data->ChanConfig[MKX_CHANNEL_0].MAC.AMSTable[3].MatchCtrl =
    MKX_ADDRMATCH_RESPONSE_ENABLE | MKX_ADDRMATCH_LAST_ENTRY;
  radio_cfg_data->ChanConfig[MKX_CHANNEL_1].MAC.AMSTable[3].MatchCtrl =
    MKX_ADDRMATCH_RESPONSE_ENABLE | MKX_ADDRMATCH_LAST_ENTRY;

  /*
   * 업데이트된 설정정보를 디바이스에 세팅한다.
   */
  int ret = mkx->API.Functions.Config(mkx, ifindex_in_dev, &radio_cfg);
  if (ret != MKXSTATUS_SUCCESS){
    Err("Fail to set interface MAC address. Config() failed - eMKxStatus: %d\n", ret);
    return -kAlResult_DevSpecificError;
  }

  saf5100_platform->dev[dev_index].req[ifindex_in_dev] = kSAF5100Req_SetIfMacAddress;

  Log(kAlLogLevel_config, "Success to set interface MAC address\n");
  return kAlResult_Success;
}


/**
 * SAF5100 플랫폼의 이벤트 폴링 함수 구현부.
 * 초기화 루틴에서 struct AlDeviceSpecificData 구조체의 Poll() 함수포인터에 연결되며, Al_Poll() 에서 호출된다.
 *
 * @param priv      @ref Polll
 * @return          @ref Polll
 */
static void al_SAF5100_PollEvent(const void *const priv)
{
  struct SAF5100Platform *saf5100_platform = (struct SAF5100Platform *)priv;
  struct pollfd fds[2];

  for (int i = 0; i < saf5100_platform->dev_num; i++) {
    fds[i].fd = saf5100_platform->dev[i].fd;
    fds[i].events = (POLLIN|POLLPRI);
  }

  int ret, i;
  while(1) {
    ret = poll(fds, saf5100_platform->dev_num, -1);
    if (ret <= 0) {  // 에러 또는 타임아웃
      Err("Fail to poll event\n");
      continue;
    }
    for (i = 0; i < saf5100_platform->dev_num; i++) {
      if (fds[i].revents & (POLLIN|POLLPRI)) {
        ret = MKx_Recv(saf5100_platform->dev[i].mkx);  // LLC 라이브러리는 본 함수 내에서 콜백함수를 호출한다.
        if (ret < 0) {
          Err("Fail to MKx_Recv() - ret: %d\n", ret);
        }
      }
      if (fds[i].revents & (POLLERR|POLLHUP|POLLNVAL)) {
        Err("Poll error on module %d - revents 0x%02X\n", i, fds[i].revents);
      }
    }
  }
}


/**
 * @copydoc al_PlatformInit
 */
int INTERNAL al_PlatformInit(struct AlPlatform *const platform, const bool reset)
{
  Log(kAlLogLevel_init, "Initializing SAF5100 platform\n");

  /*
   * 플랫폼 의존 정보를 초기화한다.
   *  - 공통 함수 포인터 등록
   *  - private 데이터 등록
   */
  platform->platform_data.TransmitMpdu = al_SAF5100_TransmitMpdu;
  platform->platform_data.AccessChannel = al_SAF5100_AccessChannel;
  platform->platform_data.GetCurrentChannel = al_SAF5100_GetCurrentChannel;
  platform->platform_data.ReleaseChannel = al_SAF5100_ReleaseChannel;
  platform->platform_data.SetIfMacAddress = al_SAF5100_SetIfMacAddress;
  platform->platform_data.PollEvent = al_SAF5100_PollEvent;
  platform->platform_data.priv = (void *)&g_al_saf5100_platform;

  /*
   * SAF5100 플랫폼 정보를 초기화한다.
   */
  memset(&g_al_saf5100_platform, 0, sizeof(g_al_saf5100_platform));
  g_al_saf5100_platform.parent = platform;
  g_al_saf5100_platform.if_num = _V2X_IF_NUM_;
  uint8_t dev_num = g_al_saf5100_platform.dev_num = g_saf5100_dev_num;

  /*
   * SAF5100 플랫폼 내 각 디바이스를 초기화한다.
   *  - 디바이스 초기화 - MKx_Init()
   *  - 디바이스 파일디스크립터 획득 (이벤트 폴링을 위해) - MKx_Fd()
   *  - 콜백함수 등록
   *  - private 데이터 등록
   */
  struct MKx *mkx;
  int ret, fd;
  struct SAF5100Device *saf5100_dev;
  for (int i = 0; i < dev_num; i++) {

    Log(kAlLogLevel_init, "Initializing SAF5100 device %d\n", i);

    ret = MKx_Init(i, &mkx);
    if (ret != MKXSTATUS_SUCCESS){
      Err("Fail to initialize. MKx_Init(%d) failed - eMKxStatus: %d\n", i, ret);
      return -kAlResult_DevSpecificError;
    }

    fd = MKx_Fd(mkx);
    if (fd < 0) {
      Err("Fail to initialize. MKx_Fd(%d) failed - eMKxStatus: %d\n", i, fd);
      return -kAlResult_DevSpecificError;
    }

    saf5100_dev = &g_al_saf5100_platform.dev[i];
    saf5100_dev->dev_index = i;
    saf5100_dev->mkx = mkx;
    saf5100_dev->fd = fd;
    saf5100_dev->mkx->API.Callbacks.TxCnf = al_SAF5100_TxCnf;
    saf5100_dev->mkx->API.Callbacks.RxAlloc = al_SAF5100_RxAlloc;
    saf5100_dev->mkx->API.Callbacks.RxInd = al_SAF5100_RxInd;
    saf5100_dev->mkx->API.Callbacks.NotifInd = al_SAF5100_NotifInd;
//  saf5100_dev->mkx->API.Callbacks.DebugInd = SAF5100_DebugInd;
//  saf5100_dev->mkx->API.Callbacks.GetTSFInd = SAF5100_GetTSFInd;
    saf5100_dev->mkx->API.Callbacks.C2XSecRsp = NULL;
    saf5100_dev->mkx->pPriv = (void *)&g_al_saf5100_platform.dev[i];
  }

  /*
   * 각 디바이스의 각 인터페이스를 기본설정으로 초기화한다 -> cw-llc* 드라이버 및 하드웨어에도 적용된다.
   */
  if (reset) {

    struct MKxRadioConfig radio_cfg;
    memset(&radio_cfg, 0, sizeof(radio_cfg));
    al_SAF5100_LoadDefaultRadioConfigData(&(radio_cfg.RadioConfigData));  /// 기본값 로딩

    /// 각 디바이스를 기본값으로 초기화 - 하드웨어 및 드라이버에도 적용된다.
    for (int i = 0; i < dev_num; i++) {
      Log(kAlLogLevel_init, "Resetting SAF5100 device %d\n", i);
      saf5100_dev = &g_al_saf5100_platform.dev[i];
      mkx = saf5100_dev->mkx;
      for (int j = 0; j < MKX_RADIO_COUNT; j++) {
        radio_cfg.RadioConfigData.ChanConfig[0].PHY.TxAntenna = (tMKxAntenna)(j + 1);
        radio_cfg.RadioConfigData.ChanConfig[0].PHY.RxAntenna = (tMKxAntenna)(j + 1);
        radio_cfg.RadioConfigData.ChanConfig[1].PHY.TxAntenna = (tMKxAntenna)(j + 1);
        radio_cfg.RadioConfigData.ChanConfig[1].PHY.RxAntenna = (tMKxAntenna)(j + 1);
        ret = mkx->API.Functions.Config(mkx, j, &radio_cfg);
        if (ret != MKXSTATUS_SUCCESS) {
          Err("Fail to reset. Config(%d, %d) failed - eMKxStatus: %d\n", i, j, ret);
          return -kAlResult_DevSpecificError;
        }
      }
    }
  }

  usleep(5*1000);

  Log(kAlLogLevel_init, "Success to initialize SAF5100 platform - %u interface is supported\n",
      g_al_saf5100_platform.if_num);
  return g_al_saf5100_platform.if_num;
}
