#include <gps.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

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

    result = gps_open("localhost", "99", &gpsData);
    if(result < 0 )
    {
        syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] gps_opn() fail(%s)\n", gps_errstr(result));
        return -1;
    }
    (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

    gpsdFlag = true;
}

int main(int argc, char *argv[])
{
    int32_t	result;
    FILE *timeSource_fd = NULL;
    time_t realTime;
    struct tm *timeInfo = NULL;

    /* 프로그램 종료 위한 시그널 등록 Ctrl+C  */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

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
                continue;
        }

        /* gpsd read */
        if (gps_waiting(&gpsData, 1000000))
        {
            result = gps_read(&gpsData);
            if(result == -1)
                syslog(LOG_ERR | LOG_LOCAL1, "[timeSync] gps_read() fail( %s)\n", gps_errstr(result));
            else
            {
                if(gpsData.set)
                {
                    realTime = (time_t)gpsData.fix.time;
                    timeInfo = localtime(&realTime);

                    rewind(timeSource_fd);
                    fprintf(timeSource_fd, "DATE='%d-%d-%d %d:%d:%d'\n",timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday, timeInfo->tm_hour+1, timeInfo->tm_min, timeInfo->tm_sec);
                    fflush(timeSource_fd);
                }
            }
        }
    }

    gps_close(&gpsData);

    return 0;
}

