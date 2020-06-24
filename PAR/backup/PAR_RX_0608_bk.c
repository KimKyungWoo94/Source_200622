#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "msgQ.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <malloc.h>
#include <PAR.h>



/****************************************************************************************
    함수원형
****************************************************************************************/
int par_InitRXoperation();
void par_RXoperation();
void par_Report(void);
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola);
static void* rxThread(void *notused);

/**
 * par_InitRXoperation() 
 * PAR 수신동작을 초기화한다.
 * 수신 타이머 관련 뮤텍스, 컨디션시그널 초기화
 * 타이머 생성 및 RX 쓰레드 생성
 * GPSD OPEN 
 * @return   성공 시 0, 실패 시 -1
 */
int par_InitRXoperation(){

	int32_t ret;
	
	/* 구조체 동적할당 */
	stPARInfo = malloc(sizeof(struct parInfo_t) * g_mib.rsuNum);
	//int size = sizeof(struct PAR_Info_t);
	//size_t msizeNUM = malloc_usable_size(stPARInfo);
	//printf("size : %d, msize : %d\n",size, msizeNUM);
	
	if(stPARInfo ==NULL)
	{
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] Fail malloc()\n");
	}

	if(g_mib.interval == 0)
	{
		g_mib.interval = 1000000; /* 타이머주기 usec -> 1초  1000000usec : 1000msec : 1초에 1번 1000msec은 1초 */
	}

	if(g_mib.cycle == 0)
	{
		g_mib.cycle = 10;  /* 10msec 수신 주기 */
	}

	/* 수신 타이머 관련 뮤텍스, 컨디션시그널 초기화*/
	pthread_mutex_init(&g_mib.txMtx, NULL);
	pthread_cond_init(&g_mib.txCond, NULL);


	/* 타이머 생성 */
	InitTxTimer(g_mib.interval); //1초

	/* RX쓰레드 생성 */
	ret = pthread_create(&rx_thread, NULL, rxThread, NULL);
	if(ret <0){
		//perror("[PAR] Fail to create RX thread() ");
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] Fail to create RX thread()\n");
		return -1;
	}
	else{
		//printf("Success RX thread() \n");
		syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] Success create RX thread() \n");
	}

	/* GPSD 오픈 */
	ret = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(ret <0){
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] gps_open() fail(%s)\n", gps_errstr(ret));
		shmCheck = true;
	}
	else{
		syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] Success gps_open()\n");
	}
	return 0;
}

/**
 * par_RXoperation() 
 * PAR 수신동작을 수행한다.
 * GPSD 읽기
 * MQ receive
 * Packet 구조체에 정보 복사
 * stPARInfo 구조체에 정보 저장
 */
void par_RXoperation(){

	int32_t ret;
	uint32_t len;
	uint8_t outBuf[BUFSIZE];
	int status;

	memset(outBuf, 0, BUFSIZE);

	while(!ending){

		/* Connection Check */
		if(shmCheck == true)
		{
			gps_close(&gpsData);
			syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] Re connection to GPSD\n");
			ret = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);

			if(ret < 0)
			{
				syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] gps_open() fail(%s)\n", gps_errstr(ret));
			}
			shmCheck = false;

		}

		/* GPS Read */
		ret = gps_read(&gpsData);
		if(ret < 0){
			//printf("[PAR_RX] gps_read() fail( %s)\n", gps_errstr(ret));
			syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] gps_read() fail( %s)\n", gps_errstr(ret));
			shmCheck = true;
		}

		//printf("g_mib_Lati : %u  g_mib_ Longi : %u \n",g_mib.Latitude, g_mib.Longitude);

		/* 차량 위도 경도 인자값으로 받음 */
		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			g_obu.obuLatitude = g_mib.Latitude;
			g_obu.obuLongitude = g_mib.Longitude;
			//printf("OBULati : %d   OBULongi : %d \n",g_obu.OBULatitude, g_obu.OBULongitude);
			syslog(LOG_INFO | LOG_LOCAL2,"Success INPUT LATI and LONGI\n");
#if 0 /* dbg 1 or 0 */
			if(g_mib.dbg)
			{
				syslog(LOG_INFO | LOG_LOCAL2,"OBULati : %d   OBULongi : %d \n",g_obu.OBULatitude, g_obu.OBULongitude);
			}
#endif
		}

		else
		{
			/* gpsd로 부터 받음 */
			if(gpsData.set)
			{
				//printf("[PAR_RX] GPSData.set Success\n");
				syslog(LOG_INFO | LOG_LOCAL2,"[PAR_RX] GPSData.set Success\n");
				g_obu.obuLatitude = (int) pow(10,7)*gpsData.fix.latitude;
				g_obu.obuLongitude = (int) pow(10,7)*gpsData.fix.longitude;
				g_obu.obuSpeed = gpsData.fix.speed;
				g_obu.obuHeading = gpsData.fix.track;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.OBULatitude, g_obu.OBULongitude, g_obu.OBUSpeed, g_obu.OBUHeading);
#if 0 /* dbg 1 or 0 */
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL2,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.OBULatitude, g_obu.OBULongitude, g_obu.OBUSpeed, g_obu.OBUHeading);
				}
#endif
			}

			else {
				/* 인자 값과 gpsd로부터 받지 않음 */
				//printf("GPS Invalid\n");
				syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] GPS Invalid\n");
				g_obu.obuLatitude = 900000001;
				g_obu.obuLongitude = 1800000001;
				g_obu.obuSpeed = 8191;
				g_obu.obuHeading = 28800;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n",g_obu.OBULatitude, g_obu.OBULongitude, g_obu.OBUSpeed, g_obu.OBUHeading);
#if 0 /* dbg 1 or 0 */
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL2,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.OBULatitude, g_obu.OBULongitude, g_obu.OBUSpeed, g_obu.OBUHeading);
				}
#endif				
			}

		}

		/* 기지국 정보 수신 */
		len = recvMQ(outBuf);
		if(len<0)
			continue;

		/* 통신성능 측정프로그램에 필요한 정보 저장 */
		else if(len >0)
		{
			memcpy(&g_Packet,outBuf,len);
			if(g_Packet.rsuID >0 && g_Packet.rsuID <= g_mib.rsuNum)
			{
				stPARInfo[g_Packet.rsuID].check =1;
				stPARInfo[g_Packet.rsuID].rsuID = g_Packet.rsuID;
				stPARInfo[g_Packet.rsuID].rsuLongitude = g_Packet.rsuLongitude;
				stPARInfo[g_Packet.rsuID].rsuLatitude = g_Packet.rsuLatitude;
				stPARInfo[g_Packet.rsuID].obuSpeed = g_obu.obuSpeed;
				stPARInfo[g_Packet.rsuID].obuHeading = g_obu.obuHeading;
				stPARInfo[g_Packet.rsuID].interval = g_mib.cycle;
				stPARInfo[g_Packet.rsuID].rxpower = g_Packet.rxPower;
				stPARInfo[g_Packet.rsuID].rcpi = g_Packet.rcpi;
				stPARInfo[g_Packet.rsuID].obuLongitude = g_obu.obuLongitude;
				stPARInfo[g_Packet.rsuID].obuLatitude = g_obu.obuLatitude;
				stPARInfo[g_Packet.rsuID].cnt++;

				//printf("stPARInfo[CNT] : %d \n\n\n",stPARInfo[g_Packet.rsuID].cnt);
#if 0 /* dbg 1 or 0 */
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL2,"stPARInfo[CNT] : %d \n\n\n",stPARInfo[g_Packet.rsuID].cnt);
				}
#endif
			}

		}

	}

	/* 동적할당 해제 */
	free(stPARInfo);

	/* RX쓰레드 종료 */
	ret = pthread_join(rx_thread, (void **)status);
	if( ret == 0 )
	{
		//printf("[PAR_RX] Completed join with rxThread status = %d\n", status);
		syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] Completed join with rxThread status = %d\n", status);
	}
	else
	{
		//printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
	}

	/* gpsd close */
	gps_close(&gpsData);

	/* 뮤텍스 및 컨디션시그널 해제 */
	pthread_mutex_destroy(&g_mib.txMtx);
	pthread_cond_destroy(&g_mib.txCond);
}

/**
 * par_Report()
 * 해당 각 기지국에 대하여 거리계산
 * 및 
 * PAR 계산을 수행하고
 * 해당 각 기지국에 대한 정보들 출력
 */

void par_Report(void){

	int idx = 0;
	uint32_t cnt[g_mib.rsuNum];
	//uint32_t maxPAR = 0;
	//uint32_t curPAR = 0;
	//printf("[PAR_INFO] Running Timer \n");
	//syslog(LOG_INFO | LOG_LOCAL0,"[PAR_INFO] Running Timer \n");

	/* RSU 갯수 만큼 반복 */
	for(idx=0;idx<=g_mib.rsuNum;idx++)
	{
		//패킷 카운트버퍼 초기화
		cnt[idx]=0;
		//curPAR, maxPAR 초기화
		stPARInfo[idx].curPAR = 0;
		stPARInfo[idx].maxPAR = 0;

		/* RSUID 일치 여부 */
		if(stPARInfo[idx].rsuID >0 && stPARInfo[idx].rsuID <= g_mib.rsuNum)
		{
			//기존 들어오던 기지국 정보가 수신되지 않기 시작함
			if(stPARInfo[idx].check >0 && stPARInfo[idx].cnt==0)
			{
				stPARInfo[idx].check++;
			}
			// 5초 동안 연속적으로 수신되지 않음
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

			/* 거리 계산 */
			stPARInfo[idx].distance = ldCaldistance(stPARInfo[idx].rsuLongitude, stPARInfo[idx].rsuLatitude, stPARInfo[idx].obuLongitude, stPARInfo[idx].obuLatitude);

			/* 해당 기지국 받은 CNT 갯수 저장 */
			cnt[idx]=stPARInfo[idx].cnt;

			/* 해당 기지국 CNT 초기화 */
			stPARInfo[idx].cnt =0;

			// 현재 측정지점에서 최대 PAR 갱신
			//curPAR = (cnt[idx]*100)/(1000/stPARInfo[idx].interval);
			//if( curPAR > maxPAR )
			//	maxPAR = curPAR;

#if 1 /* Syslog or Printf */

			/* 현재 PAR 계산 */
			stPARInfo[idx].curPAR = (cnt[idx]*100)/(1000/stPARInfo[idx].interval);
			
			/* PAR최대값 계산 */
			if(stPARInfo[idx].curPAR > stPARInfo[idx].maxPAR)
				stPARInfo[idx].maxPAR = stPARInfo[idx].curPAR;
			
			/* dbg모드 */
			if(g_mib.dbg){
				syslog(LOG_INFO | LOG_LOCAL2, "CHECK : %u, RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, OBU Latitude : %d, OBU Longitude : %d\n", 
						stPARInfo[idx].check,
						stPARInfo[idx].rsuID,
						stPARInfo[idx].rsuLatitude,
						stPARInfo[idx].rsuLongitude,
						stPARInfo[idx].obuLatitude,
						stPARInfo[idx].obuLongitude);

				syslog(LOG_INFO | LOG_LOCAL2, "RXPOWER : %d, rcpi : %d, distance : %.0f, OBUSpeed : %3.2f, OBUHeading : %3.2f, CNT : %u, PAR : %d\n",
						stPARInfo[idx].rxpower,
						stPARInfo[idx].rcpi,
						stPARInfo[idx].distance,
						(double)stPARInfo[idx].obuSpeed,
						(double)stPARInfo[idx].obuHeading,
						cnt[idx],
						stPARInfo[idx].maxPAR);
				syslog(LOG_INFO | LOG_LOCAL2, "--------------------------------------------------------------------------------------------------\n");
			}
#else
			printf("CHECK : %u, RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, OBU Latitude : %d, OBU Longitude : %d,  RXPOWER : %d, rcpi : %d, distance : %.0f, OBUSpeed : %3.2f, OBUHeading : %3.2f, CNT : %u, PAR : %d\n",
					stPARInfo[idx].check,
					stPARInfo[idx].rsuID,
					stPARInfo[idx].rsuLatitude,
					stPARInfo[idx].rsuLongitude,
					stPARInfo[idx].obuLatitude,
					stPARInfo[idx].obuLongitude,
					stPARInfo[idx].rxpower,
					stPARInfo[idx].rcpi,
					stPARInfo[idx].distance,
					(double)stPARInfo[idx].obuSpeed,
					(double)stPARInfo[idx].obuHeading,
					cnt[idx],
					stPARInfo[idx].maxPAR);
#endif
		}

	}
}

/**
 * ldCaldistance()
 * 거리 계산 함수
 * RSU 위도, 경도 와 OBU 위도, 경도 이용하여 거리 계산
 * @return 거리 값
 */
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

/**
 * rxThread()
 * RX Thread
 * 1초마다 타이머를 울려 PAR_SendReport()함수 호출
 */
static  void* rxThread(void *notused){


	while(!ending){
		/* 송신타이머로부터 시그널이 올때 때까지 대기한다. */
		pthread_mutex_lock(&g_mib.txMtx);
		pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
		pthread_mutex_unlock(&g_mib.txMtx);
		par_Report();
	}

	/* Thread 종료 */
	pthread_exit((void *)0);
}
