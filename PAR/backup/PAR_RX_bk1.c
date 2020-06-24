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
#include <PAR.h>


/* 함수원형 */
int par_InitRXoperation();
void par_RXoperation();
void par_Report(void);
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola);
static void* rxThread(void *notused);

/**
 * PAR 수신동작을 초기화한다.
 * 수신 타이머 관련 뮤텍스, 컨디션시그널 초기화
 * 타이머 생성 및 RX 쓰레드 생성
 * GPSD OPEN 
 * @return   성공 시 0, 실패 시 -1
 */
int par_InitRXoperation(){

	int32_t ret;
	
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

void par_RXoperation(){

	int32_t ret;
	uint32_t len;
	uint8_t outbuf[BUFSIZE];
	int status;

	memset(outbuf, 0, BUFSIZE);

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

		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			obu.OBULatitude = g_mib.Latitude;
			obu.OBULongitude = g_mib.Longitude;
			//printf("OBULati : %d   OBULongi : %d \n",obu.OBULatitude, obu.OBULongitude);
			syslog(LOG_INFO | LOG_LOCAL2,"Success INPUT LATI and LONGI\n");
#if 0
			if(g_mib.dbg)
			{
			syslog(LOG_INFO | LOG_LOCAL2,"OBULati : %d   OBULongi : %d \n",obu.OBULatitude, obu.OBULongitude);
			}
#endif
			}

		else
		{
			if(gpsData.set)
			{
				//printf("[PAR_RX] GPSData.set Success\n");
			//	syslog(LOG_INFO | LOG_LOCAL2,"[PAR_RX] GPSData.set Success\n");
				obu.OBULatitude = (int) pow(10,7)*gpsData.fix.latitude;
				obu.OBULongitude = (int) pow(10,7)*gpsData.fix.longitude;
				obu.OBUSpeed = gpsData.fix.speed;
				obu.OBUHeading = gpsData.fix.track;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", obu.OBULatitude, obu.OBULongitude, obu.OBUSpeed, obu.OBUHeading);
#if 0
				if(g_mib.dbg)
				{
				syslog(LOG_INFO | LOG_LOCAL2,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", obu.OBULatitude, obu.OBULongitude, obu.OBUSpeed, obu.OBUHeading);
				}
#endif
			}
			
			else {
				//printf("GPS Invalid\n");
				syslog(LOG_INFO | LOG_LOCAL2, "[PAR_RX] GPS Invalid\n");
				obu.OBULatitude = 900000001;
				obu.OBULongitude = 1800000001;
				obu.OBUSpeed = 8191;
				obu.OBUHeading = 28800;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n",obu.OBULatitude, obu.OBULongitude, obu.OBUSpeed, obu.OBUHeading);
#if 0				
				if(g_mib.dbg)
				{
				syslog(LOG_INFO | LOG_LOCAL2,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", obu.OBULatitude, obu.OBULongitude, obu.OBUSpeed, obu.OBUHeading);
				}
#endif				
			}

		}

		len = recvMQ(outbuf);
		if(len<0)
			continue;

		else if(len >0)
		{
			memcpy(&Packet,outbuf,len);
			if(Packet.rsuID >0 && Packet.rsuID <RSU_SLOT)
			{
				stPARInfo[Packet.rsuID].check =1;
				stPARInfo[Packet.rsuID].rsuID = Packet.rsuID;
				stPARInfo[Packet.rsuID].RLongitude = Packet.RLongitude;
				stPARInfo[Packet.rsuID].RLatitude = Packet.RLatitude;
				stPARInfo[Packet.rsuID].obuSpeed = obu.OBUSpeed;
				stPARInfo[Packet.rsuID].obuHeading = obu.OBUHeading;
				stPARInfo[Packet.rsuID].interval = g_mib.cycle;
				stPARInfo[Packet.rsuID].rxpower = Packet.rxpower;
				stPARInfo[Packet.rsuID].rcpi = Packet.rcpi;
				stPARInfo[Packet.rsuID].obuLongitude = obu.OBULongitude;
				stPARInfo[Packet.rsuID].obuLatitude = obu.OBULatitude;
				stPARInfo[Packet.rsuID].cnt++;

				//printf("stPARInfo[CNT] : %d \n\n\n",stPARInfo[Packet.rsuID].cnt);
#if 0
				if(g_mib.dbg)
				{
				syslog(LOG_INFO | LOG_LOCAL2,"stPARInfo[CNT] : %d \n\n\n",stPARInfo[Packet.rsuID].cnt);
				}
#endif
				}

		}

	}
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


void par_Report(void){

	int idx = 0;
	uint32_t cnt[RSU_SLOT];
	//uint32_t maxPAR = 0;
	//uint32_t curPAR = 0;
	//printf("[PAR_INFO] Running Timer \n");
	//syslog(LOG_INFO | LOG_LOCAL0,"[PAR_INFO] Running Timer \n");

	for(idx=0;idx<RSU_SLOT;idx++)
	{
		//패킷 카운트버퍼 초기화
		cnt[idx]=0;
		//curPAR, maxPAR 초기화
		stPARInfo[idx].curPAR = 0;
		stPARInfo[idx].maxPAR = 0;

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
			stPARInfo[idx].cnt =0;

			// 현재 측정지점에서 최대 PAR 갱신
			//curPAR = (cnt[idx]*100)/(1000/stPARInfo[idx].interval);
			//if( curPAR > maxPAR )
			//	maxPAR = curPAR;

#if 1 // #if 조건문 사용시 해당 용도에 대한 주석기입 필요, 만약 단순 참조를 위한 사용이라면 /* */를 이용 !!! NSW 알간???

			stPARInfo[idx].curPAR = (cnt[idx]*100)/(1000/stPARInfo[idx].interval);
			//PAR최대값 계산
			if(stPARInfo[idx].curPAR > stPARInfo[idx].maxPAR)
				stPARInfo[idx].maxPAR = stPARInfo[idx].curPAR;
			if(g_mib.dbg){
				syslog(LOG_INFO | LOG_LOCAL2, "CHECK : %u, RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, OBU Latitude : %d, OBU Longitude : %d\n", 
						stPARInfo[idx].check,
						stPARInfo[idx].rsuID,
						stPARInfo[idx].RLatitude,
						stPARInfo[idx].RLongitude,
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
#endif
		}

	}
}

/**
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

	pthread_exit((void *)0);
}
