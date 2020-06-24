/**
 * @file wlanaccess-internal.h
 * @date 2019-05-08
 * @author gyun
 * @brief wlanaccess 라이브러리 내에서 사용되는 정보 정의 헤더파일
 */


#ifndef LIBWLANACCESS_WLANACCESS_INTERNAL_H
#define LIBWLANACCESS_WLANACCESS_INTERNAL_H


#include "wlanaccess.h"


#define OPEN_API __attribute__((visibility("default")))  ///< 공개 API 함수임을 나타내기 위한 매크로
#define INTERNAL __attribute__((visibility("hidden")))   ///< 공개 API 가 아닌 내부함수로 지정 (외부로 노출되지 않음)


/**
 * 플랫폼 의존 정보. 각 플랫폼에 의존적인 정보가 저장된다.
 */
struct AlPlatformSpecificData {

  /// 플랫폼 private 데이터
  void *priv;

  /// @brief MPDU 전송 함수 포인터. 각 플랫폼 별로 구현된 MPDU 전송 함수를 등록한다.
  /// @param priv 플랫폼 private 데이터
  /// @param ifindex 인터페이스 식별번호
  /// @param mpdu 전송할 MPDU
  /// @param mpdu_size MPDU 사이즈 (CRC 불포함)
  /// @param txparams 송신 파라미터
  /// @return 성공시 0, 실패시 음수(-AlResultCode)
  int (*TransmitMpdu)(
    const void *const priv,
    const AlIfIndex ifindex,
    const uint8_t *const mpdu,
    const AlMpduSize mpdu_size,
    const struct AlMpduTxParams *const txparams);

  /// @brief 채널접속 함수 포인터. 각 플랫폼 별로 구현된 채널접속 함수를 등록한다.
  /// @param priv 플랫폼 private 데이터
  /// @param ifindex 인터페이스 식별번호
  /// @param ts0_chan TimeSlot0에 접속할 채널번호
  /// @param ts1_chan TimeSlot1에 접속할 채널번호
  /// @return 성공시 0, 실패시 음수(-AlResultCode)
  int
  (*AccessChannel)(const void *const priv, const AlIfIndex ifindex, const AlChannel ts0_chan, const AlChannel ts1_chan);

  /// @brief 접속채널확인 함수 포인터., 각 플랫폼 별로 구현된 접속채널확인 함수를 등록한다.
  /// @param ifindex 인터페이스 식별번호
  /// @param ts0_chan TimeSlot0에 접속된 채널번호가 저장될 변수의 포인터
  /// @param ts1_chan TimeSlot1에 접속된 채널번호가 저장될 변수의 포인터
  /// @return 성공시 0, 실패시 음수(-AlResultCode)
  int (*GetCurrentChannel)(
    const void *const priv,
    const AlIfIndex ifindex,
    AlChannel *const ts0_chan,
    AlChannel *const ts1_chan);

  /// @brief 채널접속해제 함수 포인터. 각 플랫폼 별로 구현된 채널접속해제 함수를 등록한다.
  /// @param priv 플랫폼 private 데이터
  /// @param ifindex 인터페이스 식별번호
  /// @param timeslot 채널접속을 해제할 TimeSlot
  /// @return 성공시 0, 실패시 음수(-AlResultCode)
  int (*ReleaseChannel)(const void *const priv, const AlIfIndex ifindex, const AlTimeSlot timeslot);

  /// @brief MAC 주소 설정 함수 포인터. 각 플랫폼 별로 구현된 MAC 주소 설정 함수를 등록한다.
  /// @param priv 플랫폼 private 데이터
  /// @param ifindex 인터페이스 식별번호
  /// @param addr 설정할 MAC 주소
  /// @return 성공시 0, 실패시 음수(-AlResultCode)
  int (*SetIfMacAddress)(const void *const priv, const AlIfIndex ifindex, const AlMacAddress addr);

  /// @brief 이벤트폴링함수 포인터. 플랫폼에 대해 구현된 폴링함수를 등록한다.
  /// @param priv 플랫폼 private 데이터
  void (*PollEvent)(const void *const priv);

};

/**
 * @brief 액세스계층 플랫폼 정보
 *
 * 액세스계층 플랫폼에서 공통적으로 사용되는 정보와 플랫폼 의존적인 정보가 포함된다.
 */
struct AlPlatform {

  struct AlTxStatstics txstats[_V2X_IF_NUM_][2]; /// 인터페이스/타임슬롯 별 송신통계정보
  struct AlRxStatstics rxstats[_V2X_IF_NUM_][2]; /// 인터페이스/타임슬롯 별 수신통계정보
  struct AlPlatformSpecificData platform_data; /// 플랫폼 의존적 정보

  /// @brief 채널접속 처리결과 전달 콜백함수 포인터
  /// @param ifindex 인터페이스 식별번호
  void (*ProcessAccessChannelResultCallback)(const AlIfIndex ifindex);

  /// @brief MAC주소설정 처리결과 전달 콜백함수 포인터
  /// @param ifindex 인터페이스 식별번호
  void (*ProcessSetIfMacAddressResultCallback)(const AlIfIndex ifindex);

  /// @brief 송신요청 처리결과 전달 콜백함수 포인터.
  /// @param result 송신요청 처리결과
  /// @param dev_specific_errcode 플랫폼 의존적 에러코드 (result가 음수일 경우에만 사용됨)
  void (*ProcessTransmitResultCallback)(const AlTxResultCode result, const int platform_specific_errcode);

  /// @brief 수신 MPDU 전달 콜백함수 포인터
  /// @param mpdu 수신 MPDU
  /// @param mpdu_size 수신 MPDU 크기
  /// @param rxparams 수신 파라미터 정보
  void (*ProcessRxMpduCallback)(const uint8_t *const mpdu,
                                const AlMpduSize mpdu_size,
                                const struct AlMpduRxParams *const rxparams);
};

/**
 * 로그출력 매크로
 */
#ifdef _DEBUG_
#define Log(l, f, a...) \
  do {  \
    if (g_al_log >= l) { \
      al_PrintLog(_PLATFORM_, __FUNCTION__, f, ## a); \
    } \
  } while(0)
#define Err(f, a ...)  \
  do {  \
    if (g_al_log >= kAlLogLevel_err) { \
      al_PrintLog(_PLATFORM_, __FUNCTION__, f, ## a); \
    } \
  } while(0)
#else
#define Log(l, f, a ...) do {} while(0)
#define Err(f, a ...) do {} while(0)
#endif

extern AlLogLevel g_al_log;
uint8_t INTERNAL al_ConvertRxPowerToRcpi(int16_t rxpower);
AlChannel INTERNAL al_ConvertFreqToChannelNumber(const uint16_t freq);
uint16_t INTERNAL al_ConvertChannelNumberToFreq(const AlChannel chan);
uint8_t INTERNAL al_GetChannelNumberBandwidth(const AlChannel channel);
void INTERNAL al_PrintLog(const char *dev, const char *func, const char *format, ...);
void INTERNAL al_PrintPacketDump(const uint8_t *const pkt, const uint32_t pkt_size);


#endif //LIBWLANACCESS_WLANACCESS_INTERNAL_H
