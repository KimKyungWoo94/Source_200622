/**
 * @file wlanaccess.c
 * @date 2019-05-08
 * @author gyun
 * @brief wlanaccess 라이브러리 공통 기능 구현 파일
 */


#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "wlanaccess.h"
#include "wlanaccess-internal.h"


struct AlPlatform g_al_platform;               ///< 액세스계층 플랫폼 정보
AlLogLevel g_al_log = kAlLogLevel_config;      ///< 로그레벨 변수 (Al_Init() 함수에서 업데이트 된다)


/**
 * 플랫폼을 초기화한다.
 *
 * @param platform  액세스계층 플랫폼 정보
 * @param reset     액세스계층 하드웨어/드라이버 초기화 여부
 * @return          성공시 액세스계층 라이브러리가 지원하는 인터페이스의 수, 실패시 음수(-AlResultCode)
 *
 * 본 함수의 구현부는 각 플랫폼별 구현 코드에서 정의되어야 한다.
 */
extern int INTERNAL al_PlatformInit(struct AlPlatform *const platform, const bool reset);


/**
 * @copydoc Al_Init
 */
int OPEN_API Al_Init(const AlLogLevel log_level)
{
  memset(&g_al_platform, 0, sizeof(g_al_platform));
  g_al_log = log_level;

  /*
   * 플랫폼별 초기화 함수 호출
   */
  return al_PlatformInit(&g_al_platform, true);
}


/**
 * @copydoc Al_Open
 */
int OPEN_API Al_Open(const AlLogLevel log_level)
{
  memset(&g_al_platform, 0, sizeof(g_al_platform));
  g_al_log = log_level;

  /*
   * 플랫폼별 오픈 함수 호출
   */
  return al_PlatformInit(&g_al_platform, false);
}


/**
 * @copydoc Al_TransmitMpdu
 */
int OPEN_API Al_TransmitMpdu(
  const AlIfIndex ifindex,
  const uint8_t *const mpdu,
  const AlMpduSize mpdu_size,
  const struct AlMpduTxParams *const txparams)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->TransmitMpdu == NULL) {
    return -kAlResult_NotSupported;
  }
  return platform_data->TransmitMpdu(platform_data->priv, ifindex, mpdu, mpdu_size, txparams);
}


/**
 * @copydoc Al_AccessChannel
 */
int OPEN_API Al_AccessChannel(const AlIfIndex ifindex, const AlChannel ts0_chan, const AlChannel ts1_chan)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->AccessChannel == NULL) {
    return -kAlResult_NotSupported;
  }
  return platform_data->AccessChannel(platform_data->priv, ifindex, ts0_chan, ts1_chan);
}


/**
 * @copydoc Al_ReleaseChannel
 */
int OPEN_API Al_ReleaseChannel(const AlIfIndex ifindex, const AlTimeSlot timeslot)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->ReleaseChannel == NULL) {
    return -kAlResult_NotSupported;
  }
  return platform_data->ReleaseChannel(platform_data->priv, ifindex, timeslot);
}


/**
 * @copydoc Al_SetIfMacAddress
 */
int OPEN_API Al_SetIfMacAddress(const AlIfIndex ifindex, const AlMacAddress addr)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->SetIfMacAddress == NULL) {
    return -kAlResult_NotSupported;
  }
  return platform_data->SetIfMacAddress(platform_data->priv, ifindex, addr);
}


/**
 * @copydoc Al_GetCurrentChannel
 */
int OPEN_API Al_GetCurrentChannel(const AlIfIndex ifindex, AlChannel *const ts0_chan, AlChannel *const ts1_chan)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->GetCurrentChannel == NULL) {
    return -kAlResult_NotSupported;
  }
  return platform_data->GetCurrentChannel(platform_data->priv, ifindex, ts0_chan, ts1_chan);
}


/**
 * @copydoc Al_PollEvent
 */
int OPEN_API Al_PollEvent(void)
{
  struct AlPlatformSpecificData *platform_data = &(g_al_platform.platform_data);
  if (platform_data->PollEvent == NULL) {
    return -kAlResult_NotSupported;
  }
  platform_data->PollEvent(platform_data->priv);
  return kAlResult_Success;
}


/**
 * @copydoc Al_RegisterCallbackAccessChannelResult
 */
void OPEN_API Al_RegisterCallbackAccessChannelResult(
  void (*ProcessAccessChannelResultCallback)(const AlIfIndex ifindex))
{
  g_al_platform.ProcessAccessChannelResultCallback = ProcessAccessChannelResultCallback;
}


/**
 * @copydoc Al_RegisterCallbackSetIfMacAddressResult
 */
void OPEN_API Al_RegisterCallbackSetIfMacAddressResult(
  void (*ProcessSetIfMacAddressResultCallback)(const AlIfIndex ifindex))
{
  g_al_platform.ProcessSetIfMacAddressResultCallback = ProcessSetIfMacAddressResultCallback;
}


/**
 * @copydoc Al_RegisterCallbackTransmitResult
 */
void OPEN_API Al_RegisterCallbackTransmitResult(
  void (*ProcessTransmitResultCallback)(const AlTxResultCode result, const int dev_specific_errcode))
{
  g_al_platform.ProcessTransmitResultCallback = ProcessTransmitResultCallback;
}


/**
 * @copydoc Al_RegisterCallbackRxMpdu
 */
void OPEN_API Al_RegisterCallbackRxMpdu(
  void (*ProcessRxMpduCallback)(
    const uint8_t *const mpdu,
    const AlMpduSize mpdu_size,
    const struct AlMpduRxParams *const rxparams))
{
  g_al_platform.ProcessRxMpduCallback = ProcessRxMpduCallback;
}


/**
 * @brief 0.5dBm 단위의 수신파워를 RCPI 값으로 변환한다.
 * @param rxpower 수신파워(0.5dBm 단위)
 * @return 변환된 RCPI 값
 *
 * RCPI = Int{(Power in dBm + 110) x 2} for 0dBm > Power > -110dBm
 * 수신파워가 -110dBm보다 작을 경우, 0,
 */
uint8_t INTERNAL al_ConvertRxPowerToRcpi(int16_t rxpower)
{
  rxpower /= 2;
  if (rxpower < -110) {
    rxpower = -110;
  } else if (rxpower > 0) {
    rxpower = 0;
  }
  return ((rxpower + 110) * 2);
}


/**
 * 채널번호 별 중심주파수 값을 계산한다.
 *
 * @param chan  채널번호
 * @return      중심주파수 값
 */
AlChannel INTERNAL al_ConvertFreqToChannelNumber(const uint16_t freq)
{
  return (freq - 5000) / 5;
}


/**
 * 각 채널번호에 대한 중심주파수 값을 계산한다.
 *
 * @param chan 채널번호
 * @return 중심주파수 값
 */
uint16_t INTERNAL al_ConvertChannelNumberToFreq(const AlChannel chan)
{
  return (5000 + (chan * 5));
}


/**
 * 채널번호 별 채널대역폭을 반환한다.
 *
 * @param channel 채널번호
 * @return 채널대역폭 (10,20)
 *
 * ITS 주파수 할당 규칙에 따라, 짝수채널번호는 10MHz 대역폭을 갖고, 홀수채널번호는 20MHz 대역폭을 가진다.
 */
uint8_t INTERNAL al_GetChannelNumberBandwidth(const AlChannel channel)
{
  if ((channel % 2) == 0) {
    return 10;
  } else {
    return 20;
  }
}


/**
 * @brief 로그메시지 출력함수 구현부. 로그 메시지를 출력한다.
 * @param dev 플랫폼 이름 (각 플랫폼별 코드에서 전역변수로 정의된다).
 * @param func 로그 출력을 수행하는 함수 이름
 * @param format 출력 라인
 * @param ... 출력 라인
 *
 * 본 함수는 직접 호출되지 않으며, 항상 Log() 및 Err() 매크로를 통해 간접 호출된다.
 * 전달된 출력문 앞에 플랫폼명과 함수명이 추가되어 표준에러(stderr)로 출력된다.
 */
void INTERNAL al_PrintLog(const char *platform, const char *func, const char *format, ...)
{
  va_list arg;
  struct timespec ts;
  struct tm tm_now;

  clock_gettime(CLOCK_REALTIME, &ts);
  localtime_r((time_t *)&ts.tv_sec, &tm_now);
  fprintf(stderr, "[%04u%02u%02u.%02u%02u%02u.%06ld]", tm_now.tm_year+1900, tm_now.tm_mon+1, tm_now.tm_mday,
    tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ts.tv_nsec / 1000);

  fprintf(stderr, "[%s][%s] ", platform, func);
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
}


/**
 * @brief 패킷 덤프를 출력한다.
 * @param pkt 출력할 패킷 데이터
 * @param pkt_size 패킷 데이터 크기
 */
void INTERNAL al_PrintPacketDump(const uint8_t *const pkt, const uint32_t pkt_size)
{
  for (int i = 0; i < pkt_size; i++) {
    if ((i!=0) && (i%16==0)) {
      printf("\n");
    }
    printf("%02X ", pkt[i]);
  }
  printf("\n");
}
