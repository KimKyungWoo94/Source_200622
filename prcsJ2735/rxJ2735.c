#include <prcsJ2735.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

#include "udp.h"
#include "ublox_debug.h"

/* 전역변수 */
bool sockCheck = false;
struct gps_data_t gpsData;
pthread_t gpsd_thread;
bool gpsdOpenFlag = false;

/* 함수 원형 */
static void* gpsdThread(void *notused);
struct timeval startTime, endTime = {0, };
bool timeFlag = true;

void setJ2735rx()
{
    int result, status;
    void *msg;
    ASN1Error err;
    char pkt[kMpduMaxSize] = {0, };

    /* 현재 시간 획득 */
    gettimeofday(&startTime, NULL);
    gettimeofday(&endTime, NULL);

    if(g_mib.dbg)
    {
        /* GPSD 쓰레드 생성 */
        result = pthread_create(&gpsd_thread, NULL, gpsdThread, NULL);
        if (result < 0) {
            //perror("[prcsJ2735] Fail to create tx thread() ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Fail to create tx thread() : %s\n", strerror(errno));
            return;
        }

    }
    else if(g_mib.dbg == 0)
    {
        /* GPSD */
        result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
        if(result < 0 )
        {
            //printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
            return;
        }
        (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

        gpsdOpenFlag = true;
    }

#if 0
    /* UBLOX LOG En */
    if(g_mib.dbg == 2)
    {
        for(int i=0; i < 35; i++)
        {
            result = write(gpsData.gps_fd, ublox_debug_message[i], 11);
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
                   pthread_exit((void *)-1);
                }
                (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

                sockCheck = false;
            }
        }

#if 0
        /* 1초 계산 획득 */
        if(timeFlag == false)
        {
            gettimeofday(&startTime, NULL);
            result = startTime.tv_sec - endTime.tv_sec;
            if(result >= 1)
            {
                timeFlag = true;
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

                            if(gpsdOpenFlag )
                            {
                                if(timeFlag)
                                {
                                    //timeFlag = false;
                                    //gettimeofday(&endTime, NULL);

                                    result = write(gpsData.gps_fd, pRTCM->msgs.tab->buf, pRTCM->msgs.tab->len);
                                    if( result < 0)
                                    {
                                        //perror("[prcsJ2735] RTCM write fail : ");
                                        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735]  RTCM write fail : %s\n", strerror(errno));

                                        sockCheck = true;
                                    }
                                    else
                                    {
                                        if( g_mib.dbg)
                                        {
                                            //printf("[prcsJ2735] Write RTCM(%d byte)\n",  result);
                                            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Write RTCM(%d byte)\n",  result);
                                        }
                                    }
                                }
                                sleep(1);
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

    return;
}

static void* gpsdThread(void *notused)
{
    int result;
    GPS_Pkt_t shared_GPS;

    result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
    if(result < 0 )
    {
        //printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
        sockCheck == true;
    }
    gpsdOpenFlag = true;
    (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

    while(!ending)
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
                pthread_exit((void *)-1);
            }
            (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

            sockCheck = false;
        }

        /* gps read */
        if (gps_waiting(&gpsData, 1000000))
        {
            result = gps_read(&gpsData);
            if(result == -1)
            {
                //printf("[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
                syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
            }
            else
            {
                if(gpsData.set & UBX_PVT_SET )
                {
                    if(g_mib.dbg)
                    {
                        syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] %u %u 0x%02x %u-%u-%u %u:%u:%u.%d\n", gpsData.pvt.lat, gpsData.pvt.lon, gpsData.pvt.flags, gpsData.pvt.year, gpsData.pvt.month, gpsData.pvt.day, gpsData.pvt.hour, gpsData.pvt.min, gpsData.pvt.sec, gpsData.pvt.nano);
#if 0
                        //printf("flags   : 0x%02x\n", gpsData.pvt.flags); 
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
                        syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] %u %u 0x%02x %u-%u-%u %u:%u:%u.%d\n", gpsData.pvt.lat, gpsData.pvt.lon, gpsData.pvt.flags, gpsData.pvt.year, gpsData.pvt.month, gpsData.pvt.day, gpsData.pvt.hour, gpsData.pvt.min, gpsData.pvt.sec, gpsData.pvt.nano);
                        syslog(LOG_INFO | LOG_LOCAL0, "*******************************************\n");
                        printf("\r******************gpsData*****************\n");
                        printf("\rstatus  : %d\n", gpsData.status); 
                        printf("\ritow    : %u\n", gpsData.pvt.itow); 
                        printf("\ryear    : %u\n", gpsData.pvt.year); 
                        printf("\rmonth   : %u\n", gpsData.pvt.month); 
                        printf("\rday     : %u\n", gpsData.pvt.day); 
                        printf("\rhour    : %u\n", gpsData.pvt.hour); 
                        printf("\rmin     : %u\n", gpsData.pvt.min); 
                        printf("\rsec     : %d\n", gpsData.pvt.sec); 
                        printf("\rvalid   : %d\n", gpsData.pvt.valid); 
                        printf("\rtAcc    : %u\n", gpsData.pvt.tAcc); 
                        printf("\rnano    : %d\n", gpsData.pvt.nano); 
                        printf("\rfixType : 0x%02x\n", gpsData.pvt.fixType); 
                        printf("\rflags   : 0x%02x\n", gpsData.pvt.flags); 
                        printf("\rflags2  : 0x%02x\n", gpsData.pvt.flags2); 
                        printf("\rnumSV   : %u\n", gpsData.pvt.numSV); 
                        printf("\rlon     : %d\n", gpsData.pvt.lon); 
                        printf("\rlat     : %d\n", gpsData.pvt.lat); 
                        printf("\rheight  : %d\n", gpsData.pvt.height); 
                        printf("\rhMSL    : %d\n", gpsData.pvt.hMSL); 
                        printf("\rhAcc    : %u\n", gpsData.pvt.hAcc); 
                        printf("\rvAcc    : %u\n", gpsData.pvt.vAcc); 
                        printf("\rvelN    : %d\n", gpsData.pvt.velN); 
                        printf("\rvelE    : %d\n", gpsData.pvt.velE); 
                        printf("\rvelD    : %d\n", gpsData.pvt.velD); 
                        printf("\rgSpeed  : %d\n", gpsData.pvt.gSpeed); 
                        printf("\rheadMot : %d\n", gpsData.pvt.headMot); 
                        printf("\rsAcc    : %u\n", gpsData.pvt.sAcc); 
                        printf("\rheadAcc : %u\n", gpsData.pvt.headAcc); 
                        printf("\rpDOP    : %u\n", gpsData.pvt.pDOP); 
                        printf("\rheadVeh : %d\n", gpsData.pvt.headVeh); 
                        printf("\rmsgDec  : %d\n", gpsData.pvt.msgDec); 
                        printf("\rmsgAcc  : %u\n", gpsData.pvt.msgAcc); 
                        printf("\rheading : %d\n", gpsData.fix.track); 
                        printf("\r********************************************\n");
#endif
                    }
                }
            }
        }
    }
    gps_close(&gpsData);
    pthread_exit((void *)0);
}


