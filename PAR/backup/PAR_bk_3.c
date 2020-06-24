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
#include <v2x-obu.h>

/* 전역변수 */
#define BUFFSIZE 1024
struct rsuInfo_t rsu; //rsu ID, 위도, 경도 구조체
struct V2X_OBU_MIB g_mib; //< 어플리케이션 관리정보
struct gps_data_t gpsData; //gpsd 구조체
struct PAR_Packet_t Packet;//prcsWSM으로부터 받은 정보 담을 구조체
struct PAR_Info_t stPARInfo[RSU_SLOT];
//struct PAR_Packet_t Packet[RSU_SLOT]; //prcsWSM으로부터 받은 정보 담을 구조체
int ending =0;
pthread_t gpsd_thread;
//DbgMsgLevel g_dbg = kDbgMsgLevel_nothing;  ///< 디버그메시지 출력레벨

/* 함수원형 */
#if 1
void sigint_handler(int signo){
	ending =1;
	exit(0);
}
#endif
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola);
static void* gpsdThread(void *notused);
static void* rxgpsdThread(void *notused);

int main(int argc, char *argv[]){
	int32_t ret;
	uint8_t outbuf[BUFFSIZE];
	uint32_t len; 
	int status;
	int idx =0;
	uint32_t curPAR =0;
	uint32_t maxPAR =0;
	uint32_t cnt[RSU_SLOT];

	printf("Running PAR application..\n");

	memset(&g_mib, 0, sizeof(struct V2X_OBU_MIB));
	memset(&rsu, 0, sizeof(struct rsuInfo_t));
	memset(&Packet, 0, sizeof(struct PAR_Packet_t));
	memset(outbuf, 0, BUFFSIZE);
	g_mib.interval = 10000; /* 송신주기 usec -> 0.1초  10000usec : 10msec : 1초에 100번 10msec은 0.01초 */

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

		/* 송신 타이머 관련 뮤텍스, 컨디션시그널 초기화*/
		pthread_mutex_init(&g_mib.txMtx, NULL);
		pthread_cond_init(&g_mib.txCond, NULL);


		/* 타이머 생성 */
		InitTxTimer(g_mib.interval);

		/* GPSD 쓰레드 생성 */
		ret = pthread_create(&gpsd_thread, NULL, gpsdThread, NULL);
		if(ret <0){
			perror("[PAR] Fail to create gpsd thread() ");
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR] Fail to create gpsd thread() : %s\n ",status);
			return -1;
		}
		else{
			printf("Success gpsd thread() \n");
		}


		while(!ending){

			/* 송신타이머로부터 시그널이 올때 때까지 대기한다. */
			pthread_mutex_lock(&g_mib.txMtx);
			pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
			pthread_mutex_unlock(&g_mib.txMtx);

			memcpy(outbuf,&rsu,sizeof(struct rsuInfo_t));
			len = sizeof(struct rsuInfo_t);
			for(int i=0;i<len;i++){
				printf("[0x%2x] ",outbuf[i]);
			}
			printf("\n");
			sendMQ(&outbuf,len);
			memset(outbuf, 0, sizeof(outbuf));
		}		

		/* GPSD 쓰레드 종료 */
		ret = pthread_join(gpsd_thread, (void **)status);
		if( ret == 0 )
		{
			if( g_mib.dbg )
			{
				printf("[PAR] Completed join with gpsdThread status = %d\n", status);
				//syslog(LOG_INFO | LOG_LOCAL0, "[PAR] Completed join with gpsdThread status = %d\n", status);
			}
		}
		else
		{
			printf("[PAR] ERROR: return code from pthread_join() is %d\n", ret);
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR] ERROR: return code from pthread_join() is %d\n", ret);
		}

		/* gpsd close */
		gps_close(&gpsData);

		/* 뮤텍스 및 컨디션시그널 해제 */
		pthread_mutex_destroy(&g_mib.txMtx);
		pthread_cond_destroy(&g_mib.txCond);

	}

	else if(g_mib.op == opRX){
		int j=0;
		printf("Running PAR RX Operation..\n");

		/* RX_GPSD 쓰레드 생성 */
		ret = pthread_create(&gpsd_thread, NULL, rxgpsdThread, NULL);
		if(ret <0){
			perror("[PAR] Fail to create gpsd thread() ");
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR] Fail to create gpsd thread() : %s\n ",status);
			return -1;
		}
		else{
			printf("Success RXgpsd thread() \n");
		}

		while(!ending){
			len = recvMQ(outbuf);
			printf("len : %d \n",len);

			if(len <0)
				continue;
			else{
				for(int i=0; i<len;i++){
					printf("[0X%2x] ", outbuf[i]);
				}
				printf("\n");
				memcpy(&Packet,outbuf,len);
				printf("[PAR : Received Packet to prcsWSM]\n");
				printf("RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, RXPOWER : %d, rcpi : %d \n",Packet.rsuID, Packet.RLatitude, Packet.RLongitude, Packet.rxpower, Packet.rcpi);
				//printf("RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, RXPOWER : %d, rcpi : %d \n",Packet[j].rsuID, Packet[j].RLatitude, Packet[j].RLongitude, Packet[j].rxpower, Packet[j].rcpi);

				if(Packet.rsuID >0 && Packet.rsuID <RSU_SLOT){
					printf("SUCCESS RSUID < RSUSLOT\n");
					stPARInfo[Packet.rsuID].check =1;
					stPARInfo[Packet.rsuID].rsuID = Packet.rsuID;
					stPARInfo[Packet.rsuID].RLongitude = Packet.RLongitude;
					stPARInfo[Packet.rsuID].RLatitude = Packet.RLatitude;
					stPARInfo[Packet.rsuID].obuSpeed = g_mib.OBUSpeed;
					stPARInfo[Packet.rsuID].obuHeading = g_mib.OBUHeading;
					stPARInfo[Packet.rsuID].interval = g_mib.interval;
					stPARInfo[Packet.rsuID].rxpower = Packet.rxpower;
					stPARInfo[Packet.rsuID].rcpi = Packet.rcpi;
					stPARInfo[Packet.rsuID].obuLongitude = g_mib.OBULongitude;
					stPARInfo[Packet.rsuID].obuLatitude = g_mib.OBULatitude;
					stPARInfo[Packet.rsuID].cnt++;

					printf("stPARInfo[CNT] : %d \n",stPARInfo[Packet.rsuID].cnt);
				}
				//printf("1\n");
				for(idx=0;idx<RSU_SLOT;idx++)
				{
					printf("idx : %d\n",idx);
					//패킷 카운트버퍼 초기화
					cnt[idx]=0;
					//printf("2\n");
					printf("stPARInfo[idx].rsuID : %d \n", stPARInfo[idx].rsuID);

					if(stPARInfo[idx].rsuID >0 && stPARInfo[idx].rsuID < RSU_SLOT)
					{
						//기존 들어오던 기지국 정보가 수신되지 않기 시작함
						if(stPARInfo[idx].check >0 && stPARInfo[idx].cnt==0)
						{
							stPARInfo[idx].check++;
						}
						// 10초 동안 연속적으로 수신되지 않음
						if(stPARInfo[idx].check > MAX_ZERO_COUNT && stPARInfo[idx].cnt==0)
						{
							// 다음 수신 정보가 있을때까지 통신성능측정프로그램으로 정보전달하지 않음.
							stPARInfo[idx].check=0;
						}
						// 체크되어 있지 않은 기지국의 정보는 통신성능측정프로그램으로 정보전달하지 않음.
						if(stPARInfo[idx].check==0)
						{
							continue;
						}
						
						stPARInfo[idx].distance = ldCaldistance(stPARInfo[idx].RLongitude, stPARInfo[idx].RLatitude, stPARInfo[idx].obuLongitude, stPARInfo[idx].obuLatitude);

						cnt[idx]=stPARInfo[idx].cnt;
						//stPARInfo[idx].cnt =0;
						
						// 현재 측정지점에서 최대 PAR 갱신
						//printf("3\n");
						//curPAR = (cnt[idx]*100)/(1000/g_mib.interval);
						curPAR = (cnt[idx]*100)/(1000/10);
						//printf("4\n");
						
						if( curPAR > maxPAR ) 
							maxPAR = curPAR;

						if(stPARInfo[idx].cnt>RSU_SLOT)
							stPARInfo[idx].cnt =0;
						
						printf("%d\n",idx);

						printf("CHECK : %u, RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, OBU Latitude : %d, OBU Longitude : %d,  RXPOWER : %d, rcpi : %d, distance : %.0f, OBUSpeed : %3.2f, OBUHeading : %3.2f, CNT : %u, PAR : %d \n",
								stPARInfo[idx].check,
								stPARInfo[idx].rsuID,
								stPARInfo[idx].RLatitude,
								stPARInfo[idx].RLongitude,
								stPARInfo[idx].obuLatitude,
								stPARInfo[idx].obuLongitude,
								stPARInfo[idx].rxpower,
								stPARInfo[idx].rcpi,
								stPARInfo[idx].distance,
								(double)stPARInfo[idx].obuSpeed,
								(double)stPARInfo[idx].obuHeading,
								cnt[idx],
								maxPAR);
					}

				}
				printf("MAXPAR : %u\n",maxPAR);


			}
		}
		/* RX_GPSD 쓰레드 종료 */
		ret = pthread_join(gpsd_thread, (void **)status);
		if( ret == 0 )
		{
			if( g_mib.dbg )
			{
				printf("[PAR_RX] Completed join with gpsdThread status = %d\n", status);
				//syslog(LOG_INFO | LOG_LOCAL0, "[PAR_RX] Completed join with gpsdThread status = %d\n", status);
			}
		}
		else
		{
			printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
		}

		/* gpsd close */
		gps_close(&gpsData);
	}

	releaseMQ();
	return 0;
}
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola)
{
	long double a, b, c, d, x, y, z;
	a = (long double)((rlo * 1e-7) * M_PI / 180);
	b = (long double)((rla * 1e-7) * M_PI / 180);
	c = (long double)((olo * 1e-7) * M_PI / 180);
	d = (long double)((ola * 1e-7) * M_PI / 180);
	x = sin(b) * sin(d);
	y = cos(b) * cos(d);
	z = fabs(c - a);
	y = y*cos(z);
	x = acos(x + y);
	y = x * 6371009;

	return y;
}
static  void* gpsdThread(void *notused){
	int result;

	result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(result <0){
		printf("[PAR] gps_open() fail(%s)\n", gps_errstr(result));
		//syslog(LOG_ERR | LOG_LOCAL1, "[PAR] gps_open() fail(%s)\n", gps_errstr(result));
	}

	while(!ending){

		result = gps_read(&gpsData);
		if(result < 0){
			printf("[PAR] gps_read() fail( %s)\n", gps_errstr(result));
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR] gps_read() fail( %s)\n", gps_errstr(result));
			gps_close(&gpsData);
		}
		printf("g_mib_Lati : %u  g_mib_ Longi : %u \n",g_mib.Latitude, g_mib.Longitude);

		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			printf("INPUT LATI and LONGI\n");
			rsu.rsuID = g_mib.rsuID;
			rsu.rsuLatitude = g_mib.Latitude;
			rsu.rsuLongitude = g_mib.Longitude;
			printf("rsu ID : %d   rsuLati : %d   rsuLongi : %d \n",rsu.rsuID, rsu.rsuLatitude, rsu.rsuLongitude);
		}

		else
		{
			if(gpsData.set)
			{
				printf("GPSData.set Success\n");
				rsu.rsuID = g_mib.rsuID;
				rsu.rsuLatitude = (int) pow(10,7)*gpsData.fix.latitude;
				rsu.rsuLongitude = (int) pow(10,7)*gpsData.fix.longitude;
				printf("rsu ID : %d  rsuLatitude : %d  rsuLongitude : %d\n",rsu.rsuID, rsu.rsuLatitude, rsu.rsuLongitude);
			}

			else {
				printf("GPS Invalid\n");
				rsu.rsuID = g_mib.rsuID;
				rsu.rsuLatitude = 900000001;
				rsu.rsuLongitude = 1800000001;
				printf("rsu ID : %d  rsuLatitude : %d  rsuLongitude : %d\n",rsu.rsuID, rsu.rsuLatitude, rsu.rsuLongitude);
			}
		}
		sleep(1);
	}
	gps_close(&gpsData);
	pthread_exit((void *)0);
}

static  void* rxgpsdThread(void *notused){
	int result;

	result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(result <0){
		printf("[PAR_RX] gps_open() fail(%s)\n", gps_errstr(result));
		//syslog(LOG_ERR | LOG_LOCAL1, "[PAR_RX] gps_open() fail(%s)\n", gps_errstr(result));
	}

	while(!ending){

		result = gps_read(&gpsData);
		if(result < 0){
			printf("[PAR_RX] gps_read() fail( %s)\n", gps_errstr(result));
			//syslog(LOG_ERR | LOG_LOCAL1, "[PAR_RX] gps_read() fail( %s)\n", gps_errstr(result));
			gps_close(&gpsData);
		}
		printf("g_mib_Lati : %u  g_mib_ Longi : %u \n",g_mib.Latitude, g_mib.Longitude);

		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			printf("INPUT LATI and LONGI\n");
			g_mib.OBULatitude = g_mib.Latitude;
			g_mib.OBULongitude = g_mib.Longitude;
			printf("OBULati : %d   OBULongi : %d \n",g_mib.OBULatitude, g_mib.OBULongitude);
		}

		else
		{
			if(gpsData.set)
			{
				printf("[PAR_RX] GPSData.set Success\n");
				g_mib.OBULatitude = (int) pow(10,7)*gpsData.fix.latitude;
				g_mib.OBULongitude = (int) pow(10,7)*gpsData.fix.longitude;
				g_mib.OBUSpeed = gpsData.fix.speed;
				g_mib.OBUHeading = gpsData.fix.track;
				printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_mib.OBULatitude, g_mib.OBULongitude, g_mib.OBUSpeed, g_mib.OBUHeading);
			}

			else {
				printf("GPS Invalid\n");
				g_mib.OBULatitude = 900000001;
				g_mib.OBULongitude = 1800000001;
				g_mib.OBUSpeed = 8191;
				g_mib.OBUHeading = 28800;
				printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n",g_mib.OBULatitude, g_mib.OBULongitude, g_mib.OBUSpeed, g_mib.OBUHeading);
			}
		}
		sleep(1);
	}
	gps_close(&gpsData);
	pthread_exit((void *)0);
}
