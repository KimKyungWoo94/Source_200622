/**
 * @file saf5100.h
 * @date 2019-05-09
 * @author gyun
 * @brief SAF5100 플랫폼 의존 코드 헤더 파일
 */


#ifndef LIBWLANACCESS_SAF5100_H
#define LIBWLANACCESS_SAF5100_H


#include "wlanaccess-internal.h"
#include "llc-api.h"


#define SAF5100_MAX_DEV_IN_PLATFORM 2            ///< 단일 플랫폼에서 지원가능한 SAF5100 디바이스 최대개수
#define SAF5100_IF_NUM_IN_DEV (MKX_RADIO_COUNT)  ///< SAF5100 디바이스 내에 포함된 통신인터페이스 수
#define SAF5100_MAX_IF_IN_PLATFORM 4             ///< SAF5100을 탑재한 단일플랫폼에서 지원가능한 통신인터페이스 최대개수

/// CMakeLists.txt 에 설정된 TARGET_PLATFORM_V2X_IF_NUM 값 유효성 검사.
#if ((_V2X_IF_NUM_ <= 0) || (_V2X_IF_NUM_ > SAF5100_MAX_IF_IN_PLATFORM))
#error "Invalid TARGET_PLATFORM_V2X_IF_NUM value in CMakeLists.txt"
#endif


/// Amount of space to reserve for stack footers at the end of the PktBuf
#define LLC_DEV_HEADROOM (72)
#define LLC_DEV_TAILROOM (16)

/*
 * API 를 통해 요청되는 요청의 유형
 */
enum eSAF5100ReqType {
  kSAF5100Req_None,               ///< 요청된 것이 없음
  kSAF5100Req_AccessChannel,      ///< 채널 접속 요청
  kSAF5100Req_SetIfMacAddress,    ///< MAC주소 설정 요청
};
typedef uint8_t SAF5100ReqType;   ///< @copydoc eSAF5100ReqType

/*
 * SAF5100 디바이스 정보 (각 SAF5100 칩/모듈에 대한 정보)
 */
struct SAF5100Device
{
  uint8_t dev_index;                 ///< 디바이스 식별번호 (0~1)
  volatile uint8_t init_notif_cnt;   ///< 모듈 초기화 설정에 관련된 Notifcation 발생 수. 0->2가 되면 초기화가 완료된 것이다. TODO:: 불필요하면 삭제
  struct MKx *mkx;                   ///< 해당 디바이스에 연관된 MKx 핸들
  int fd;                            ///< 해당 디바이스에 대한 파일 디스크립터 (이벤트 폴링용)
  SAF5100ReqType req[SAF5100_IF_NUM_IN_DEV];   ///< 해당 디바이스의 각 인터페이스에 대한 요청 상태
};

/*
 * SAF5100 플랫폼 정보
 *  - SAF5100 플랫폼 : 1~2 개의 SAF5100 디바이스를 포함하는 하드웨어 플랫폼
 */
struct SAF5100Platform
{
  uint8_t if_num;     ///< 플랫폼이 지원하는 인터페이스 개수
  uint8_t dev_num;    ///< 플랫폼이 지원하는 SAF5100 디바이스 개수 (1~2)
  struct SAF5100Device dev[SAF5100_MAX_DEV_IN_PLATFORM]; ///< 각 SAF5100 디바이스 정보
  struct AlPlatform *parent;                             ///< 상위 (공통) 플랫폼 정보
};

extern int D_LEVEL;
extern struct SAF5100Platform g_al_saf5100_platform;

const struct MKxRadioConfigData INTERNAL
*al_SAF5100_GetCurrentRadioConfigData(const struct MKx *const mkx, const AlIfIndex ifindex_in_dev);
uint8_t INTERNAL al_SAF5100_Convert10MhzMcsToDataRate(const tMKxMCS mcs);
uint8_t INTERNAL al_SAF5100_Convert20MhzMcsToDataRate(const tMKxMCS mcs);
tMKxStatus INTERNAL al_SAF5100_TxCnf(struct MKx *pMKx, tMKxTxPacket *pTxPkt, const tMKxTxEvent *pTxEvent, void *pPriv);
tMKxStatus INTERNAL al_SAF5100_RxAlloc(struct MKx *pMKx, int BufLen, uint8_t **ppBuf, void **ppPriv);
tMKxStatus INTERNAL al_SAF5100_RxInd(struct MKx *pMKx, tMKxRxPacket *pRxPkt, void *pPriv);
tMKxStatus INTERNAL al_SAF5100_NotifInd(struct MKx *pMKx, tMKxNotif Notif);

#endif //LIBWLANACCESS_SAF5100_H
