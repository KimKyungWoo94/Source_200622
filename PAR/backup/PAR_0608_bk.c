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

/****************************************************************************************
  전역변수
*****************************************************************************************/
#define BUFFSIZE 1024
struct rsuInfo_t g_rsu; //rsu ID, 위도, 경도 구조체
struct obuInfo_t g_obu; //OBU 위도 ,경도, 스피드, 헤딩 구조체
struct parMib g_mib; //< 어플리케이션 관리정보
struct gps_data_t gpsData; //gpsd 구조체
struct parPacket_t g_Packet;//prcsWSM으로부터 받은 정보 담을 구조체
//struct parInfo_t stPARInfo[RSU_SLOT];
struct parInfo_t *stPARInfo; //통신성능측정프로그램에 필요한 정보를 가지고 있는 구조체
int ending = 0;
bool shmCheck = false; //gpsd shm Check;
pthread_t rx_thread;
pthread_t gpsd_thread;

/****************************************************************************************
    함수원형
****************************************************************************************/
void sigint_handler(int signo){
	ending =1;
	//exit(0);
}

/**
 * main()
 * 어플리케이션 메인 함수
 * @param argc  어플리케이션 실행 시 입력되는 명령줄 내 파라미터들의 개수 (어플리케이션 실행파일명 포함)
 * @param argv  어플리케이션 실행 시 입력되는 명령줄 내 파라미터들의 문자열 집합 (어플리케이션 실행파일명 포함)
 * @return      성공 시 0, 실패 시 -1
 */
int main(int argc, char *argv[]){
	int32_t ret;

	printf("Running PAR application..\n");

	/* 메모리 초기화 */
	memset(&g_mib, 0, sizeof(struct parMib));
	memset(&g_rsu, 0, sizeof(struct rsuInfo_t));
	memset(&g_obu, 0, sizeof(struct obuInfo_t));
	memset(&g_Packet, 0, sizeof(struct parPacket_t));
	

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

	/* 송신 동작 */
	if(g_mib.op ==opTX){

		printf("Running PAR TX Operation..\n");
		par_InitTXoperation();
		par_TXoperation();
	}

	/*수신 동작 */
	else if(g_mib.op == opRX){

		printf("Running PAR RX Operation..\n");
		par_InitRXoperation();
		par_RXoperation();
	}
	/* 동적 할당 해제 */
	free(stPARInfo);
	/* MQ 해제 */
	releaseMQ();
	return 0;
}

