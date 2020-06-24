#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "msgQ.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <PAR.h>

/* 전역변수 */
#define BUFFSIZE 1024
struct rsuInfo_t rsu; //rsu ID, 위도, 경도 구조체
struct obuInfo_t obu; //OBU 위도 ,경도, 스피드, 헤딩 구조체
struct PAR_MIB g_mib; //< 어플리케이션 관리정보
struct gps_data_t gpsData; //gpsd 구조체
struct PAR_Packet_t Packet;//prcsWSM으로부터 받은 정보 담을 구조체
//struct PAR_Info_t stPARInfo[RSU_SLOT];
struct PAR_Info_t *stPARInfo;
int ending = 0;
bool shmCheck = false; //gpsd shm Check;
pthread_t rx_thread;
pthread_t gpsd_thread;

/* 함수원형 */
void sigint_handler(int signo){
	ending =1;
	//exit(0);
}

int main(int argc, char *argv[]){
	int32_t ret;

	printf("Running PAR application..\n");

	/* 메모리 초기화 */
	memset(&g_mib, 0, sizeof(struct PAR_MIB));
	memset(&rsu, 0, sizeof(struct rsuInfo_t));
	memset(&obu, 0, sizeof(struct obuInfo_t));
	memset(&Packet, 0, sizeof(struct PAR_Packet_t));
	//memset(&stPARInfo,0,sizeof(struct PAR_Info_t));
	
	//g_mib.interval = 10000; /* 송신주기 usec -> 0.1초  10000usec : 10msec : 1초에 100번 10msec은 0.01초 */

	/* 사용자가 입력한 파라미터들을 MIB에 저장한다. */
	ret = ParsingOptions(argc, argv);

	if(ret<0){
		return -1;
	}

	/* 파라미터 출력 */
	PrintOptions();

	/* 프로그램 종료 위한 시그널 등록 Ctrl+C */
	signal(SIGINT, sigint_handler);

	/* MsgQ Open */
	if(initMQ() == -1)	
		return -1;

	if(g_mib.op ==opTX){

		printf("Running PAR TX Operation..\n");
		par_InitTXoperation();
		par_TXoperation();
	}

	else if(g_mib.op == opRX){

		printf("Running PAR RX Operation..\n");
		par_InitRXoperation();
		par_RXoperation();
	}
	free(stPARInfo);
	releaseMQ();
	return 0;
}

