#include <gps.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/file.h>
#include <shm.h>

/* 전역변수 */
int ending = 0;

/* 시그널 핸들러 */
void sigint_handler(int signo)
{
    ending = 1;
}

int main(int argc, char *argv[])
{
    int32_t	result;
    bool gpsdCheckFlag = false;
    char *shmPtr = NULL;
    int shmid;

    /* 프로그램 종료 위한 시그널 등록 Ctrl+C  */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    /* Shared Memory open */
    if(InitShm(&shmid, &shmPtr) == -1)
        return -1;

    while(!ending)
    {
        memcpy(&gpsdCheckFlag, shmPtr, sizeof(bool));

#if 0
        if(gpsdCheckFlag == true)
            printf("gpsdCheckFlag : true\n");
        else if(gpsdCheckFlag == false)
            printf("gpsdCheckFlag : false\n");

        printf("flag : %d\n", gpsdCheckFlag);
#endif
        if(gpsdCheckFlag == true)
        {
            syslog(LOG_ERR | LOG_LOCAL1, "[checkGPSD] GPSd restart\n");
            system("/home/nvidia/wave/killAll.sh");
            sleep(2);
            system("/home/nvidia/wave/runRTCM.sh obu");

            gpsdCheckFlag = false;
            memcpy(shmPtr, &gpsdCheckFlag, sizeof(bool));

        }
        sleep(1);
    }

    ReleaseShm(shmPtr);

    return 0;
}

