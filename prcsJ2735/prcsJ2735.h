#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <J2735_201603_CITS.h>
#include <gps.h>
#include <hexdump.h>
#include <syslog.h>

#define ADDRSIZE 20

/* ENUM */
typedef enum
{
    opType_rx = 0,
    opType_tx_MapData,
    opType_tx_SPaT,
    opType_tx_BSM,
    opType_tx_PVD,
    opType_tx_RSA,
    opType_tx_TIM,
    opType_tx_RTCM,
    opType_count
} opType;

typedef enum
{
    udpServer = 1,
    udpClient = 2
} sock_e;


/* STRUCT */
typedef struct
{
    /* 동작 변수 */
    opType      op;
    sock_e      sockType;

    /* 타이머 변수 */
    uint32_t    interval;
    timer_t     timer;
    pthread_mutex_t txMtx;
    pthread_cond_t txCond; 

    /* gpsd */
    char *gpsdPort;

    /* 디버그 변수 */
    uint32_t    dbg;

    /* udp client 변수 */
    char destIP[ADDRSIZE];
    uint32_t destPort;
} mib_t;

typedef struct
{
    uint32_t type;
    uint32_t len;
    char buf[1024];
    bool flag;//갱신정보
} rtcmData_t;

/*----------------------------------------------------------------------------------*/


/****************************************************************************************
	전역변수

****************************************************************************************/
extern mib_t		g_mib;
extern int ending;
#define kMpduMaxSize 2302

/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/
/* options.c */
void PrintOptions(void);
int32_t ParsingOptions(int32_t argc, char *argv[]);
/* asn1.c */
void *asn1_malloc(size_t size);
void *asn1_realloc(void *ptr, size_t size);
void asn1_free(void *ptr);
void asn1_xer_printf(const ASN1CType *msg_type, void* msg);
/* msgQ.c */
int initMQ(void);
void releaseMQ(void);
int recvMQ(char *pkt);
void sendMQ(uint8_t *pPkt, uint32_t len);
/* txJ2735.c */ 
void setJ2735tx();
void sendJ2735(void);
/* timer.c */
int InitTxTimer(const uint32_t interval);
/* rxJ2735.c */ 
void setJ2735rx();
/* prcsRTCM.c */
int getRTCM(uint8_t *buf);
void setRTCM(uint8_t *buf, int len);
int rtcmPkt(struct gps_data_t * gpsData);
int ConstructRTCM(uint8_t *pkt, uint32_t *len);
void setRTCM_mutex(int op);
//void set_renewFlag();
/* socket.c */
int createSockThread();
void closeSocketThread();
void connection_Check(void);
