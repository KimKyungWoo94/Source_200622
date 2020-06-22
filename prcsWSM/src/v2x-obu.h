//
// Created by gyun on 2019-08-19.
//

#ifndef V2X_OBU_V2X_OBU_H
#define V2X_OBU_V2X_OBU_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <msgQ.h>
#include <syslog.h>
#include "dot3/dot3.h"


// 서비스 PSID
//  - 시나리오 : WSMP, IP 각각 하나의 서비스가 있다.
#define WSMP_SERVICE_PSID (10)
#define IP_SERVICE_PSID (0x1020407E)

// 각 인터페이스 별 채널
#define IF0_CHAN_NUM (178)
#define IF1_CHAN_NUM (184)

// IP 서비스 정보
//#define IP_SERVICE_SERVER_ADDRESS "1080::8:800:200c:419c"
//#define IP_SERVICE_SERVER_PORT (20000)

// WRA 정보
//#define WRA_IP_PREFIX "1080::"
//#define WRA_IP_PREFIX_LEN 64
//#define WRA_DEFAULT_GATEWAY "1080::8:800:200c:4100"
//#define WRA_PRIMARY_DNS "1080::8:800:200c:4101"

// 각 인터페이스 별 전송 정보
#define IF0_DATARATE  (12)   // 12 = 6Mbps
#define IF1_DATARATE  (12)   // 12 = 6Mbps
#define IF0_POWER  (20)      // dBm
#define IF1_POWER  (20)      // dBm

// 서비스가 속한 WSA ID
//  - 서비스 별로 동일해야 단일 WSA에 수납된다.
//#define WSMP_SERVICE_WSA_ID 0
//#define IP_SERVICE_WSA_ID 0

// 전송 주기
#define WSM_TX_INTERVAL (2000000) // 1usec 단위

// 로그메시지 출력 레벨
enum eDbgMsgLevel {
  kDbgMsgLevel_nothing, ///< 미출력
  kDbgMsgLevel_event, ///< 이벤트 출력
  kDbgMsgLevel_msgdump,  ///< 메시지 hexdump 출력
  kDbgMsgLevel_max = kDbgMsgLevel_msgdump
};
typedef uint32_t DbgMsgLevel; ///< @copydoc eDbgMsgLevel

typedef enum
{
    opRX,
    opTX,
    opTRX
} op_e;

struct V2X_OBU_MIB
{
    uint8_t if_num; ///< v2x 송수신 인터페이스 총 개수 (플랫폼 하드웨어에 의존적이다)

  /* 동작변수 */
  op_e op;

  /* 송신환경 변수 */
  uint32_t          netIfIndex;
  uint8_t           destMac[kDot3MacAddrSize];
  Dot3ChannelNumber channel;
  Dot3Priority      priority;
  Dot3TimeSlot      timeSlot;
  Dot3DataRate      dataRate;
  Dot3Power         power;
  Dot3Psid          psid;
  int16_t rxpower;
  uint8_t rcpi;

};


/*
 * 전역변수
 */
extern struct V2X_OBU_MIB g_mib;
extern DbgMsgLevel g_dbg;
extern const uint8_t g_if0_mac_address[];
extern const uint8_t g_if1_mac_address[];

/*
 * v2x-obu-libdot3.c
 */
int V2X_OBU_InitDot3Library(int log_level);

/*
 * v2x-obu-libwlanaccess.c
 */
int V2X_OBU_OpenAccessLibrary(const int log_level);
int V2X_OBU_AccessChannel(const uint8_t if_idx, const uint8_t ts0_chan, const uint8_t ts1_chan);
int V2X_OBU_GetCurrentChannel(uint8_t *const ts0_chan, uint8_t *const ts1_chan);
int V2X_OBU_SetIfMacAddress(const uint8_t if_idx, const uint8_t *addr);
void V2X_OBU_WaitEventPolling(void);

/*
 * v2s-obu-rx.c
 */
void V2X_OBU_ProcessRxMpdu(const uint8_t *const mpdu, const uint16_t mpdu_size);
//int rtcmCheckTimer(const uint32_t interval);

/*
 * v2x-obu-wsa.c
 */
int V2X_OBU_InitWsaTx(const uint32_t timer_interval);

/*
 * v2x-obu-wsm.c
 */
int V2X_OBU_InitWsmTx(const uint32_t timer_interval);

/* options.c */
int32_t ParsingOptions(int32_t argc, char *argv[]);

#endif //V2X_OBU_V2X_OBU_H
