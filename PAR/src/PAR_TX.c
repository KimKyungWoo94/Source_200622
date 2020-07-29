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

/****************************************************************************************
    함수원형
****************************************************************************************/
int par_InitTXoperation();
void par_TXoperation();
static void* gpsdThread(void *notused);

/**
 * par_InitTXoperation()
 * PAR 송신동작을 초기화한다.
 * 송신 타이머 관련 뮤텍스, 컨디션시그널 초기화
 * 타이머 생성 및 GPSD 쓰레드 생성
 * @return   성공 시 0, 실패 시 -1
 */
int par_InitTXoperation(){

	int32_t ret;
	void* status;

	/* 주기 인자값으로 안받았을 경우 */
	if(g_mib.interval ==0)
	{
		g_mib.interval = 10000;/* 송신주기 usec -> 0.1초  10000usec : 10msec : 1초에 100번 10msec은 0.01초 */
		//g_mib.interval = 100000; /* 송신주기 usec -> 0.1초  100000usec : 100msec : 1초에 10번 100msec은 0.1초 */
	}


	/* 송신 타이머 관련 뮤텍스, 컨디션시그널 초기화*/
	pthread_mutex_init(&g_mib.txMtx, NULL);
	pthread_cond_init(&g_mib.txCond, NULL);


	/* 타이머 생성 */
	InitTxTimer(g_mib.interval);

	/* GPSD 쓰레드 생성 */
	ret = pthread_create(&gpsd_thread, NULL, gpsdThread, NULL);
	if(ret <0)
	{
		//perror("[PAR] Fail to create gpsd thread() ");
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_TX] Fail to create gpsd thread() : %s\n ",(char*)status);
		return -1;
	}
	else
	{
		//printf("[PAR] Success gpsd thread() \n");
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] Success create gpsd thread() \n");
	}
	return 0;
}

/**
 * par_TXoperation()
 * PAR 송신동작을 수행한다.
 * RSU 구조체에 있는 정보를 OUTBUF에 저장 후 메세지큐 전송
 */
void par_TXoperation()
{
	int32_t ret;
	uint8_t outBuf[BUFSIZE];
	uint32_t len;
	void* status;

	memset(outBuf, 0, BUFSIZE);

	while(!ending)
	{

		/* 송신타이머로부터 시그널이 올때 때까지 대기한다. */
		pthread_mutex_lock(&g_mib.txMtx);
		pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
		pthread_mutex_unlock(&g_mib.txMtx);
		
		/*RSU 정보 버퍼에 복사 */
		memcpy(outBuf,&g_rsu,sizeof(struct rsuInfo_t));
		len = sizeof(struct rsuInfo_t);

		/* 기지국 정보 전송 */
		sendMQ((char *)&outBuf,len);

		/* 메모리 초기화 */
		memset(outBuf, 0, sizeof(outBuf));
	}
	
	/* gpsd close */
	gps_close(&gpsData);


	/* GPSD 쓰레드 종료 */
	ret = pthread_join(gpsd_thread, /*(void **)*/&status);

	if( ret == 0 )
	{
		//printf("[PAR] Completed join with gpsdThread status = %s\n", (char*)status);
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] Completed join with gpsdThread status = %s\n",(char*)status);
	}
	else
	{
		//printf("[PAR] ERROR: return code from pthread_join() is %d\n", ret);
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_TX] ERROR: return code from pthread_join() is %d\n", ret);
	}

	/* 뮤텍스 및 컨디션시그널 해제 */
	pthread_mutex_destroy(&g_mib.txMtx);
	pthread_cond_destroy(&g_mib.txCond);
}

/**
 * GPSD Thread()
 * GPSD를 열고 읽어서 값을 채워주는 기능
 * 인자로 위도 경도 받으면 GPSD에서 채워주지 않는다.
 * 주기 1초
 */
static  void* gpsdThread(void *notused){

	int32_t result;

	/* gps open */
	result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(result <0)
	{
		//printf("[PAR] gps_open() fail(%s)\n", gps_errstr(result));
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_TX] gps_open() fail(%s)\n", gps_errstr(result));
		shmCheck = true;
	}

	while(!ending)
	{
		/* Connection Check */
		if(shmCheck == true)
		{
			gps_close(&gpsData);
			syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] Re connection to GPSD\n");
			
			result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);

			if(result < 0 )
			{
				syslog(LOG_ERR | LOG_LOCAL5, "[PAR_TX] gps_open() fail(%s)\n", gps_errstr(result));
				pthread_exit((void *)-1);
			}
			shmCheck = false;
		}


		/* GPS Read */
		result = gps_read(&gpsData);
		if(result < 0)
		{
			syslog(LOG_ERR | LOG_LOCAL5, "[PAR_TX] gps_read() fail( %s)\n", gps_errstr(result));
			shmCheck = true;
		}

		/* 기지국 위도 경도 인자값으로 받음 */
		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			g_rsu.rsuID = g_mib.rsuID;
			g_rsu.rsuLatitude = g_mib.Latitude;
			g_rsu.rsuLongitude = g_mib.Longitude;

			syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] INPUT LATI AND LONGI\n");
			if(g_mib.dbg)
			{
				syslog(LOG_INFO | LOG_LOCAL4, "rsuID : %d   rsuLati : %d   rsuLongi : %d \n",g_rsu.rsuID, g_rsu.rsuLatitude, g_rsu.rsuLongitude);
			}
		}

		else
		{
			/* gpsd로 부터 받음 */
			if(gpsData.set)
			{
				g_rsu.rsuID = g_mib.rsuID;
				g_rsu.rsuLatitude = (int) pow(10,7)*gpsData.fix.latitude;
				g_rsu.rsuLongitude = (int) pow(10,7)*gpsData.fix.longitude;

				syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] GPSData.set Success\n");
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL4, "rsuID : %d   rsuLati : %d   rsuLongi : %d \n",g_rsu.rsuID, g_rsu.rsuLatitude, g_rsu.rsuLongitude);
				}
			}

			else {
				/* 인자 값과 gpsd로부터 받지 않음 */
				g_rsu.rsuID = g_mib.rsuID;
				g_rsu.rsuLatitude = 900000001;
				g_rsu.rsuLongitude = 1800000001;

				syslog(LOG_INFO | LOG_LOCAL4, "[PAR_TX] GPSData Invalid\n");
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL4, "rsuID : %d   rsuLati : %d   rsuLongi : %d \n",g_rsu.rsuID, g_rsu.rsuLatitude, g_rsu.rsuLongitude);
				}
			}
		}

		sleep(1);
	}

	/* GPSD CLOSE */
	gps_close(&gpsData);

	/* Thread 종료 */
	pthread_exit((void *)0);
}
