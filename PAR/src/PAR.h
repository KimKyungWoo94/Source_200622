//
// Created by gyun on 2019-08-19.
//

#ifndef PAR_PAR_H
#define PAR_PAR_H

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
#include <malloc.h>
#include <errno.h>
#include <gps.h>
#include <stdint.h>
#include "dot3/dot3.h"

#define RSU_SLOT 101
#define BUFSIZE 1024
#define MAX_ZERO_COUNT 5
//#define MSIZE(ptr) malloc_usable_size((void*)ptr)


/****************************************************************************************
  ENUM
 ****************************************************************************************/
typedef enum
{
	opRX,
	opTX,
} op_e;

/****************************************************************************************
  구조체
 ****************************************************************************************/

/* Linkedlist 정보 */
typedef struct list_t {
	struct parInfo_t *cur;
	struct parInfo_t *head;
	struct parInfo_t *tail;
	int numOfList;
}linkedList;


/* RSU 정보 */
struct rsuInfo_t{
	int32_t rsuID;
	int32_t rsuLatitude;
	int32_t rsuLongitude;
};

/* OBU 정보 */
struct obuInfo_t{

	int32_t  obuLatitude;
	int32_t  obuLongitude;
	double   obuSpeed;
	double   obuHeading;
};

/* prcsWSM으로부터 받은 패킷 정보 */
struct parPacket_t{

	int rsuID;//prcsWSM으로부터 받은 RSU_ID
	int32_t rsuLatitude; //prcsWSM으로부터 받은 위도 int32_t int; 4Byte
	int32_t rsuLongitude;//prcsWSM으로부터 받은 경도
	int16_t rxPower; //prcsWSM으로부터 받은 RXPOWER int16_t short int 2Byte
	uint8_t rcpi; // prcsWSM으로부터 받은 rcpi uint8_t unsigned char 1Byte

};

/* 통신성능 측정 프로그램에 사용될 정보 */
struct parInfo_t{
	uint32_t check;// 이벤트 번호
	int rsuID;//prcsWSM으로부터 받은 RSU_ID
	int32_t rsuLatitude; //prcsWSM으로부터 받은 위도 int32_t int; 4Byte
	int32_t rsuLongitude;//prcsWSM으로부터 받은 경도
	int16_t rxpower[10]; //prcsWSM으로부터 받은 RXPOWER int16_t short int 2Byte
	//int16_t rxpower2;
	//uint8_t rcpi2;
	uint8_t rcpi[10]; // prcsWSM으로부터 받은 rcpi uint8_t unsigned char 1Byte
	int arrayIdx;
	int32_t obuLatitude; //OBU 위도
	int32_t obuLongitude; //OBU 경도
	double obuHeading; //방면
	double obuSpeed; //속도
	uint32_t interval; //수신주기
	double distance; //거리
	uint32_t cnt; //COUNT
	uint32_t maxPAR; //최대PAR
	uint32_t curPAR; //현재 PAR
	struct parInfo_t *next;
	int pastArrayidx;
};

/* 통신성능측정 프로그램에 사용될 인자 값 및 변수들 */
struct parMib
{

	/* 동작변수 */
	op_e op;

	/* 송신 인자값 */
	int rsuID;
	
	/* 수신 인자값 */
	uint32_t cycle;
	uint32_t Information; //Information 쓰레드 사용 여부
	//uint32_t rsuNum; //RSU 개수
	
	/* 송수신 인자값 */
	int32_t Latitude; //위도
	int32_t Longitude; //경도


	/* 타이머 변수 */
	uint32_t    interval;
	timer_t     timer;
	pthread_mutex_t txMtx;
	pthread_cond_t txCond;

	/*디버그 변수 */
	uint32_t    dbg;

};


/****************************************************************************************
  전역변수
 ****************************************************************************************/
extern struct rsuInfo_t g_rsu;
extern struct obuInfo_t g_obu;
extern struct parMib g_mib;
extern struct parPacket_t g_Packet;
//extern struct parInfo_t stPARInfo[RSU_SLOT];
extern struct parInfo_t *stPARInfo;
extern linkedList *ListPtr;
extern int ending;
extern bool shmCheck;
extern pthread_t rx_thread;
extern pthread_t userSelect_thread;
extern pthread_t gpsd_thread;
extern struct gps_data_t gpsData;

/****************************************************************************************
  함수원형(지역/전역)
 ****************************************************************************************/

/* options.c */
int32_t ParsingOptions(int32_t argc, char *argv[]);
void PrintOptions(void);

/* PAR.c */
void sigint_handler(int signo);

/* PAR-tx.c */
int par_InitTXoperation();
void par_TXoperation();
static void* gpsdThread(void *notused);

/* PAR-RX.c */
int par_InitRXoperation();
void par_RXoperation();
void createNode(/*linkedList *L*/);
void freeAllNode();
void par_Report(void);
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola);
static void* rxThread(void *notused);
static void* userSelectThread(void *notused);
struct parInfo_t* getNode(int index);
void getCalDataRcpi(uint8_t* array, int arrayIdx, int mode);
void getCalDataRxpower(int16_t* array, int arrayIdx, int mode);
bool isThereRSUID(int rsuID);
/* msgQ.c */
int initMQ(void);
void releaseMQ(void);
int recvMQ(char *pkt);
void sendMQ(uint8_t *pPkt, uint32_t len);

/* shm.c */
int32_t InitShm(int* shmid, char **shmPtr);
int32_t ReleaseShm(char *shmPtr);

/* timer.c */
static void TxTimerExpired(union sigval arg);
int InitTxTimer(const uint32_t interval);
#endif //PAR_PAR_H
