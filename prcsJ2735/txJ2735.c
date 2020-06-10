#include <prcsJ2735.h>
#include <unistd.h>

/* 전역변수 */
pthread_t tx_thread;

/* 함수원형*/
static void* txThread(void *notused);

void setJ2735tx()
{
    int result;
    int status;
    struct gps_data_t gpsData;

    /* GPSD */
    if(g_mib.sockType == udpServer)
    {
        result = gps_open("localhost", g_mib.gpsdPort, &gpsData);
        if(result < 0 )
        {
        //    printf("[prcsJ2735] gps_opn() fail(%s)\n", gps_errstr(result));
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_open() fail(%s)\n", gps_errstr(result));
            return;
        }
        (void) gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
    }

    /* 송신 타이머 관련 뮤텍스, 컨디션시그널 초기*/
    pthread_mutex_init(&g_mib.txMtx, NULL);
    pthread_cond_init(&g_mib.txCond, NULL);

    setRTCM_mutex(0);//Mutex 열고 닫기 함수 0 이면 열기

    /* 송신 쓰레드 생성 */
    result = pthread_create(&tx_thread, NULL, txThread, NULL);
    if (result < 0) {
        //perror("[prcsJ2735] fail to create tx thread() ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] fail to create tx thread() : %s\n", strerror(errno));
        return;
    }

    /* 타이머 생성 */ 
    InitTxTimer(g_mib.interval);

    /* Socket 쓰레드 생성 */
    createSockThread();

    while(!ending)
    {
        /* Socket connection */
        connection_Check();
       
        if(g_mib.sockType == udpServer)
        {
            if (gps_waiting(&gpsData, 1000000))
            {
                result = gps_read(&gpsData);
                if(result == -1)
                {
                //    printf("[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
                    syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] gps_read() fail( %s)\n", gps_errstr(result));
                }
                else if(result > 0)
                {
                    /* RTCM 송신일경우 RTCM 파싱 */
                    if(g_mib.op == opType_tx_RTCM /*&& gpsData.set & RTCM3_SET*/)
                        rtcmPkt(&gpsData);
                }
            }
        }
        else
            sleep(1);
    }

    /* 송신 쓰레드 종료 */
    result = pthread_join(tx_thread, (void **)status); //송신쓰레드 종료되는것을 기다림
    if( result == 0 )
    {
        if( g_mib.dbg )
        {
            //printf("[prcsJ2735] Completed join with txThread status = %d\n", status);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Completed join with txThread status = %d\n", status);
        }
    }
    else
    {
        printf("[prcsJ2735] ERROR: return code from pthread_join() is %d\n", result);
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] ERROR: return code from pthread_join() is %d\n", result);
    }

    /* Socket 쓰레드 종료 */
    closeSocketThread();

    /* 뮤텍스 및 컨디션시그널 해제 */
    pthread_mutex_destroy(&g_mib.txMtx);
    pthread_cond_destroy(&g_mib.txCond);
    setRTCM_mutex(1); //0이면 실행 1이면 종료

    /* gpsd Socket close */
    if(g_mib.sockType == udpServer)
        gps_close(&gpsData);

    return;
}

static void* txThread(void *notused)//RTK보정정보받은것을 RTCM메세지로 인코딩후 prcsWSM으로 메시지큐 전송
{
    int	result;
    uint8_t pkt[kMpduMaxSize];
    uint32_t len = 0;

    /* 설정된 동작에 따라 메시지를 생성한다. */
    while(!ending)
    {
        /* 송신타이머로부터 시그널이 올때 때까지 대기한다. */
        pthread_mutex_lock(&g_mib.txMtx);
        pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
        pthread_mutex_unlock(&g_mib.txMtx);

        /* 동작모드가 RTCM일때 */
        if(g_mib.op == opType_tx_RTCM)
        {
            result	=	ConstructRTCM(pkt, &len);
            if(result < 0)
                continue;

            /* 생성된 메시지를 송신한다. */
            sendMQ(pkt, len); 
            memset(pkt, 0, sizeof(pkt));
        }
        /* TO DO - MapData, SPaT, PVD, BSM, RSA, TIM 
           추가 필요 */

    }

    pthread_exit((void *)0);
}
