#include <gps.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/file.h>
#include <shm.h>

/* 전역변수 */
int ending = 0;
bool gpsdFlag = false;
struct gps_data_t gpsData;

/* 시그널 핸들러 */
void sigint_handler(int signo)
{
    ending = 1;
}

int openGPSd()
{
    int result;

#if 0
    result = gps_open("localhost", "99", &gpsData);
#else
    result = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
#endif
    if(result < 0 )
    {
        syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] gps_opn() fail(%s)\n", gps_errstr(result));
        gpsdFlag = false;
        return -1;
    }
    //(void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

    gpsdFlag = true;
}

int main(int argc, char *argv[])
{
    int32_t	result;
    FILE *timeSource_fd = NULL;
    time_t realTime;
    struct tm *timeInfo = NULL;
    char *shmPtr = NULL;
    int shmid;
    bool gpsdCheckFlag = false;

    /* 프로그램 종료 위한 시그널 등록 Ctrl+C  */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    /* Shared Memory open */
    if(InitShm(&shmid, &shmPtr) == -1)
        return -1;

    /* UTC Time을 저장할 파일 open */
    timeSource_fd = fopen("timeSource", "w" );
    if (timeSource_fd == NULL)
    {        
        syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] 'timeSource' open error : %s", strerror(errno));
        return -1;
    }

    /* GPSd 소켓 open */
    openGPSd();

    /* 시간정보 파일로 저장 */
    while(!ending)
    {
        /* GPSd 소켓 check */
        if(gpsdFlag == false)
        {
            if(openGPSd() == -1)
            {
                sleep(1);
                continue;
            }
        }

        /* gpsd read */
//        if (gps_waiting(&gpsData, 1000000))
//        {
            //gpsdCheckFlag = false;
            result = gps_read(&gpsData);
            if(result < 0)
            {
                syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] gps_read() fail( %s)\n", gps_errstr(result));
                gpsdFlag = false;
                gpsdCheckFlag = true;
                memcpy(shmPtr, &gpsdCheckFlag, sizeof(bool));
                gps_close(&gpsData);
            }
            else
            {
                if(gpsData.set)
                {
                    realTime = (time_t)gpsData.fix.time;
                    timeInfo = localtime(&realTime);

                    rewind(timeSource_fd);
                    fprintf(timeSource_fd, "DATE='%d-%d-%d %d:%d:%d'\n",timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday, timeInfo->tm_hour+1, timeInfo->tm_min, timeInfo->tm_sec);
#if 0
                    printf("DATE='%d-%d-%d %d:%d:%d'\n",timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday, timeInfo->tm_hour+1, timeInfo->tm_min, timeInfo->tm_sec);
                    printf("DATE='%d-%d-%d %d:%d:%d'\n",gpsData.pvt.year, gpsData.pvt.month, gpsData.pvt.day, gpsData.pvt.hour+1, gpsData.pvt.min, gpsData.pvt.sec);
                    printf("Latitude  : %u\n",gpsData.pvt.lat);
                    printf("Longitude : %u\n",gpsData.pvt.lon);
                    printf("numSV     : %u\n",gpsData.pvt.numSV);
                    printf("itow      : %u\n",gpsData.pvt.itow);
#endif
                    fflush(timeSource_fd);

                }
            }
            sleep(1);
            //usleep(125000);
#if 0
        }
        else
        {
            syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] gps_waiting() fail( %s)\n", gps_errstr(result));
            gpsdCheckFlag = true;
            gpsdFlag = false;
            memcpy(shmPtr, &gpsdCheckFlag, sizeof(bool));
            gps_close(&gpsData);

        }
#endif

    }

    gps_close(&gpsData);
    fclose(timeSource_fd);

    return 0;
}

