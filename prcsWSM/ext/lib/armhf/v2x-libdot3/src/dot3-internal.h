//
// Created by gyun on 2019-06-04.
//

#ifndef LIBDOT3_DOT3_INTERNAL_H
#define LIBDOT3_DOT3_INTERNAL_H

#ifdef __cplusplus  // gtest를 이용한 내부함수 단위테스트를 위해 정의됨.
extern "C" {
#endif

#include "dot3/dot3-types.h"
#include "dot3-mib.h"


#define OPEN_API __attribute__((visibility("default")))  ///< 공개 API 함수임을 나타내기 위한 매크로
#ifdef _EXPORT_INTERNAL_FUNC_
#define INTERNAL __attribute__((visibility("default")))  ///< (내부함수 단위테스트를 위해) 내부함수 미지정
#else
#define INTERNAL __attribute__((visibility("hidden")))   ///< 공개 API 가 아닌 내부함수로 지정 (외부로 노출되지 않음)
#endif


/**
 * @brief 로그메시지 출력 레벨
 *
 * 높은 레벨은 낮은 레벨의 범위를 포함한다.
 * 즉, 로그변수가 높은 레벨로 설정되어 있으면, 그 하위레벨에 해당되는 로그는 함께 출력된다.
 */
enum eDot3LogLevel
{
  kDot3LogLevel_none = 0, /// 아무 로그도 출력하지 않는다.
  kDot3LogLevel_err,      /// 에러 로그
  kDot3LogLevel_init,     /// 초기화 절차에 관련된 로그
  kDot3LogLevel_config,   /// 설정에 관련된 로그 (API 엔트리 포함)
  kDot3LogLevel_event,    /// 각종 이벤트(패킷 송수신 포함)에 관련된 로그
  kDot3LogLevel_dump,     /// 상세내용 로그(송수신 패킷 덤프 데이터 등)
  kDot3LogLevel_all,      /// 모든 로그
  kDot3LogLevel_min = kDot3LogLevel_none,
  kDot3LogLevel_max = kDot3LogLevel_all
};
typedef int Dot3LogLevel;  ///< @copydoc eDot3LogLevel


/*
 * 함수 원형(들)
 */
// dot3.c
int INTERNAL dot3_InitDot3(void);

// dot3-chaninfo.c
int INTERNAL dot3_InitPciTable(struct Dot3ProviderInfo *const pinfo);
void INTERNAL dot3_FlushPciTable(struct Dot3ProviderInfo *const pinfo);
void INTERNAL dot3_PrintPciTableEntry(
  const Dot3LogLevel log_level,
  const struct Dot3PciTableEntry *const entry);
void INTERNAL dot3_PrintPciContents(const Dot3LogLevel log_level, const struct Dot3Pci *const info);
void INTERNAL dot3_PrintPciTable(const Dot3LogLevel log_level, const struct Dot3ProviderInfo *const pinfo);

// dot3-check-validity.c
bool INTERNAL dot3_IsValidPsidValue(const Dot3Psid psid);
bool INTERNAL dot3_IsValidPriorityValue(const Dot3Priority priority);
bool INTERNAL dot3_IsValidChannelNumberValue(const Dot3ChannelNumber chan_num);
bool INTERNAL dot3_IsValidDataRateValue(const Dot3DataRate datarate);
bool INTERNAL dot3_IsValidPowerValue(const Dot3Power power);
bool INTERNAL dot3_IsValidProviderChannelAccess(const Dot3ProviderChannelAccess chan_access);
bool INTERNAL dot3_IsValidPscLen(const uint8_t psc_len);
bool INTERNAL dot3_IsValidWsaCountThresholdIntervalValue(const uint8_t interval);
bool INTERNAL dot3_IsValidWsaIdValue(const Dot3WsaIdentifier wsa_id);
bool INTERNAL dot3_IsValidWsaContentCountValue(const Dot3WsaContentCount content_cnt);
bool INTERNAL dot3_IsValidWsaAdvertiserIdLen(const uint8_t len);
bool INTERNAL dot3_IsValidLatitudeValue(const Dot3Latitude latitude);
bool INTERNAL dot3_IsValidLongitudeValue(const Dot3Longitude longitude);
bool INTERNAL dot3_IsValidElevationValue(const Dot3Elevation elevation);
bool INTERNAL dot3_IsValidIPv6PrefixLenValue(const Dot3IPv6PrefixLen len);

// dot3-convert.c
uint8_t dot3_ConvertPowerForEncoding(Dot3Power signed_power);

// dot3-log.c
void dot3_PrintLog(const char *func, const char *format, ...);
void dot3_PrintPacketDump(const uint8_t *const pkt, const uint32_t pkt_size);

// dot3-mpdu.c
void dot3_ConstructMpdu(struct Dot3WsmMpduTxParams *const params, uint8_t *const outbuf);
int dot3_ParseMpdu(const uint8_t *const mpdu, const Dot3PduSize mpdu_size, struct Dot3WsmMpduRxParams *const params);

// dot3-psr.c
void INTERNAL dot3_InitPsrTable(struct Dot3ProviderInfo *const pinfo);
int INTERNAL dot3_AddPsr(struct Dot3ProviderInfo *const pinfo, const struct Dot3Psr *const psr);
int INTERNAL dot3_DeletePsr(struct Dot3ProviderInfo *const pinfo, const Dot3Psid psid);
void INTERNAL dot3_DeleteAllPsrs(struct Dot3ProviderInfo *const pinfo);
int INTERNAL dot3_GetPsrWithPsid(
  const struct Dot3ProviderInfo *const pinfo,
  const Dot3Psid psid,
  struct Dot3Psr *const psr);
int INTERNAL dot3_GetPsrNum(const struct Dot3ProviderInfo *const pinfo);
int INTERNAL dot3_GetAllPsrs(
  const struct Dot3ProviderInfo *const pinfo,
  struct Dot3Psr *psrs_array,
  const Dot3PsrNum psrs_array_size);
void INTERNAL dot3_PrintPsrContents(const Dot3LogLevel log_level, const struct Dot3Psr *const psr);

// dot3-wsa.c
int INTERNAL dot3_ConstructWsa(
  struct Dot3ProviderInfo *const pinfo,
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);
int INTERNAL dot3_ParseWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params);

// dot3-wsm.c
int dot3_ConstructWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);
int dot3_ParseWsm(
  const uint8_t *const msdu,
  const Dot3PduSize msdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params);

/*
 * 로그출력 매크로
 */
#ifdef _DEBUG_
#define Log(l, f, a...) \
  do {  \
    if (g_dot3_log >= l) { \
      dot3_PrintLog(__FUNCTION__, f, ## a); \
    } \
  } while(0)
#define Err(f, a ...)  \
  do {  \
    if (g_dot3_log >= kDot3LogLevel_err) { \
      dot3_PrintLog(__FUNCTION__, f, ## a); \
    } \
  } while(0)
#else
#define Log(l, f, a ...) do {} while(0)
#define Err(f, a ...) do {} while(0)
#endif

extern Dot3LogLevel g_dot3_log;
extern struct Dot3Mib INTERNAL g_dot3_mib;

#ifdef __cplusplus  // gtest를 이용한 내부함수 단위테스트를 위해 정의됨.
}
#endif

#endif //LIBDOT3_DOT3_INTERNAL_H
