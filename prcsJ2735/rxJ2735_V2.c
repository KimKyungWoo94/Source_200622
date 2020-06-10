#include <prcsJ2735.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

#include "udp.h"
#include "ublox_debug.h"
#include "shm.h"

/* 전역변수 */
bool sockCheck = false;
struct gps_data_t gpsData;
pthread_t gpsd_thread;
char *shmPtr = NULL;
int shmid;
bool gpsdCheckFlag = false;
int writeErrCnt = 0;
int recvCnt = 0;
int errCnt = 0;

/* 함수 원형 */
static void* gpsdThread(void *notused);
struct timeval startTime, endTime = {0, };
bool timeFlag = true;

int openGPSD()
{
	int result;

	/* GPSD */
	result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
	if(result < 0 )
	{
		//printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
		syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_open() fail(%s)\n", gps_errstr(result));
		sockCheck = true;
		return -1;
	}
	else
	{
		if(gpsData.gps_fd == 0)
		{
			syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_fd is 0");
			sockCheck = true;
			return -1;
		}
	}
	(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
	syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] gps_open() Success\n");

	sockCheck = false;

	return 0;
}

void setJ2735rx()
{
	int result, status;
	void *msg;
	ASN1Error err;
	char pkt[kMpduMaxSize] = {0, };
	int timeCheck = 0;

	/* 현재 시간 획득 */
	gettimeofday(&startTime, NULL);
	gettimeofday(&endTime, NULL);

	/* Shared Memory open */
	if(InitShm(&shmid, &shmPtr) == -1)
		return;

	//    if(g_mib.dbg)
	//    {
	/* GPSD 쓰레드 생성 */
	result = pthread_create(&gpsd_thread, NULL, gpsdThread, NULL);
	if (result < 0) {
		//perror("[prcsJ2735] Fail to create tx thread() ");
		syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Fail to create tx thread() : %s\n", strerror(errno));
		return;
	}
	//    }
#if 0
	else if(g_mib.dbg == 0)
	{
		/* GPSD */
		result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
		if(result < 0 )
		{
			//printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
			syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_open() fail(%s)\n", gps_errstr(result));
			sockCheck = true;
			return;
		}
		(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

		//gpsdOpenFlag = true;
		openGPSD();
	}
	/* UBLOX LOG En */
	if(g_mib.dbg == 2)
	{
		for(int i=0; i < 37; i++)
		{
			result = write(gpsData.gps_fd, ublox_debug_message[i], 11);
			if(result == -1)
				syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Ublox debug message error : %s\n", strerror(errno));
			else
				syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Ublox debug message send\n");
			usleep(10000);
		}

		for(int i=0; i < 2; i++)
		{
			result = write(gpsData.gps_fd, ublox_debug_message_2[i], 16);
			if(result == -1)
				syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Ublox debug message error : %s\n", strerror(errno));
			else
				syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Ublox debug message send\n");
			usleep(10000);
		}
	}
#endif

	while(!ending)
	{
#if 0
		if(g_mib.dbg == 0)
		{
			/* connection check */
			if(sockCheck == true)
			{
				gps_close(&gpsData);

				//printf("[prcsJ2735] Re connection to GPSD\n");
				syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Re connection to GPSD\n");

				result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
				if(result < 0 )
				{
					//printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
					syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
					sleep(1);
					continue;
				}
				(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
				if(openGPSD() == -1)
				{
					sleep(1);
					continue;
				}
			}
		}
#endif
		/* msgQ read */
		result = recvMQ(pkt);
		if(result < 0 )
			continue;
		else
		{

			/* J2735 Decoding */
			result = asn1_uper_decode(&msg, asn1_type_MessageFrame, pkt, result, &err);
			if(result < 0)
			{
				//printf("[prcsJ2735] Decoding fail \n");
				syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Decoding fail \n");
			}
			else
			{
				if( g_mib.dbg)
				{
					//printf("[prcsJ2735] Decoding success\n");
					//asn1_xer_printf(asn1_type_MessageFrame, msg);
					syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Decoding success\n");
				}

				/* 1초 계산 획득 */
				if(timeFlag == false)
				{
					gettimeofday(&startTime, NULL);
					timeCheck = startTime.tv_sec - endTime.tv_sec;
					if(timeCheck >= 1)
					{
						timeFlag = true;
					}
					else if(timeCheck < 0)
					{
						gettimeofday(&endTime, NULL);
						continue;
					}
				}

				switch( ((MessageFrame *)msg)->messageId)
				{
					case 28 :
						{
							RTCMcorrections *pRTCM = ((MessageFrame *)msg)->value.u.data;

							if( g_mib.dbg)
							{
								//printf("[prcsJ2735] Receive RTCM(%d Byte)\n", result);
								syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Receive RTCM(%d Byte)\n", result);
								//hexdump(pRTCM->msgs.tab->buf, pRTCM->msgs.tab->len);
							}
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] startTime : %d, endTime : %d\n",  startTime.tv_sec, endTime.tv_sec);

							if(sockCheck == false )
							{
								if(timeFlag)
								{
									timeFlag = false;
									gettimeofday(&endTime, NULL);

									syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] gps_fd : %d\n", gpsData.gps_fd);
									result = write(gpsData.gps_fd, pRTCM->msgs.tab->buf, pRTCM->msgs.tab->len);
									if( result < 0)
									{
										//perror("[prcsJ2735] RTCM write fail : ");
										syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735]  RTCM write fail : %s\n", strerror(errno));
										gps_close(&gpsData);
										sockCheck = true;
									}
									else
									{
										if(gpsData.pvt.flags == 0x01)
											writeErrCnt++;
										else
											writeErrCnt = 0;

										if(writeErrCnt >= 3)
										{
											syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] RTCM write err\n");
											gps_close(&gpsData);
											sockCheck = true;
											writeErrCnt = 0;
											continue;
										}

										if( g_mib.dbg)
										{

											//printf("[prcsJ2735] Write RTCM(%d byte)\n",  result);
											syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Write RTCM(%d byte)\n",  result);
										}
									}
								}
							}
							else
								syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] GPSd socket not open\n");

							break;
						}
						/* TO DO - MapData, SPaT, PVD, BSM, RSA, TIM 
						   추가 필요 */
				}
			}
		}
	}

	if(g_mib.dbg)
	{
		/* 송신 쓰레드 종료 */
		result = pthread_join(gpsd_thread, (void **)status);
		if( result == 0 )
		{
			if( g_mib.dbg )
			{
				//printf("[prcsJ2735] Completed join with gpsdThread status = %d\n", status);
				syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Completed join with gpsdThread status = %d\n", status);
			}
		}
		else
		{
			//printf("[prcsJ2735] ERROR: return code from pthread_join() is %d\n", result);
			syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] ERROR: return code from pthread_join() is %d\n", result);
		}
	}
	else if(g_mib.dbg == 0)
		gps_close(&gpsData);

	ReleaseShm(shmPtr);

	return;
}

static void* gpsdThread(void *notused)
{
	int result;
	GPS_Pkt_t shared_GPS;
	uint32_t prevItow = 0;

#if 0
	//result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
	result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(result < 0 )
	{
		//printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
		syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
		sockCheck = true;
	}
	//gpsdOpenFlag = true;
	//(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
#else
	openGPSD();
#endif


	while(!ending)
	{
		/* connection check */
		if(sockCheck == true)
		{
#if 0
			gps_close(&gpsData);

			//printf("[prcsJ2735] Re connection to GPSD\n");
			syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Re connection to GPSD\n");

			//result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
			result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
			if(result < 0 )
			{
				//printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
				syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
				sleep(1);
				continue;
			}
			//(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

			sockCheck = false;
#else
			if(openGPSD() == -1)
			{
				sleep(1);
				continue;
			}
#endif
		}

		/* gps read */
		if (gps_waiting(&gpsData, 1000000))
		{
			//            gpsdCheckFlag = false;
			result = gps_read(&gpsData);
			if(result < 0)
			{
				//printf("[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
				syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
				gps_close(&gpsData);
				sockCheck = true;
				sleep(1);
			}
			else
			{
				if(gpsData.set & UBX_PVT_SET )
				{
					if(g_mib.dbg)
					{
						if(prevItow != gpsData.pvt.itow)
						{
#if 1
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] %u %u %d 0x%02x %u %u-%u-%u %u:%u:%u.%d\n", gpsData.pvt.lat, gpsData.pvt.lon, gpsData.pvt.gSpeed, gpsData.pvt.flags, gpsData.pvt.pDOP, gpsData.pvt.year, gpsData.pvt.month, gpsData.pvt.day, gpsData.pvt.hour, gpsData.pvt.min, gpsData.pvt.sec, gpsData.pvt.nano);
#else
							syslog(LOG_INFO | LOG_LOCAL0, "*******************gpsData*****************\n");
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] itow      : %u\t", gpsData.pvt.itow);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] time      : %u-%u-%u %u:%u:%u.%d\t", gpsData.pvt.year, gpsData.pvt.month, gpsData.pvt.day, gpsData.pvt.hour, gpsData.pvt.min, gpsData.pvt.sec, gpsData.pvt.nano);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] latitude  : %u\t", gpsData.pvt.lat);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] longitude : %u\t", gpsData.pvt.lon);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] speed     : %u\t", gpsData.pvt.gSpeed);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] numSV     : %u\t", gpsData.pvt.numSV);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] pDOP      : %u\t", gpsData.pvt.pDOP);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] hAcc      : %u\t", gpsData.pvt.hAcc);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] vAcc      : %u\t", gpsData.pvt.vAcc);
							syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] flags     : 0x%02x\t", gpsData.pvt.flags);
							syslog(LOG_INFO | LOG_LOCAL0, "*******************************************\n");
#endif
							prevItow = gpsData.pvt.itow;
						}
					}
				}
			}
		}
		else
		{
			syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_waiting() fail( %s)\n", gps_errstr(result));
#if 1
			errCnt++;

			if(errCnt >= 3)
			{
				gpsdCheckFlag = true;
				memcpy(shmPtr, &gpsdCheckFlag, sizeof(bool));
				errCnt = 0;
			}
#else
			gpsdCheckFlag = true;
			memcpy(shmPtr, &gpsdCheckFlag, sizeof(bool));
#endif
			gps_close(&gpsData);
			sockCheck = true;
		}
	}
	gps_close(&gpsData);
	pthread_exit((void *)0);
}



