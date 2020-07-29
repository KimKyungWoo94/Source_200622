/**
 * @file v2x-obu-tx-wsm.c
 * @date 2019-08-19
 * @author gyun
 * @brief WSM 송수신 관련 기능 구현
 */


#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

//#include "dot3/dot3.h"
#include "wlanaccess/wlanaccess.h"

#include "v2x-obu.h"

#if 0
static timer_t g_tx_timer; ///< 송신타이머
static pthread_mutex_t g_tx_timer_mtx; ///< 송신타이머 뮤텍스
static pthread_cond_t tx_timer_cond; ///< 송신타이머 컨디션
#endif
static pthread_t g_tx_thread; ///< 송신쓰레드


/**
 * WSM 송신 쓰레드 함수
 *  - 대기하고 있다가, 송신타이머쓰레드로부터 컨디션 시그널을 수신하면, WSM을 송신한다.
 *
 * @param notused   사용되지 않음
 * @return          NULL (프로그램 종료시에만 리턴됨)
 */
static void* V2X_OBU_WsmTxThread(void *notused)
{
    int mpdu_size;
    uint8_t mpdu[kMpduMaxSize];

    struct Dot3WsmMpduTxParams wsm_params;
    struct AlMpduTxParams al_params;

    /* 190827- yslee */
    uint8_t pkt[kMpduMaxSize];
    uint32_t len = 0;


    do {
        if (g_dbg >= kDbgMsgLevel_msgdump) {
            //printf("\n-- Sending WSM ---------------------------------------------\n");
            syslog(LOG_INFO | LOG_LOCAL6, "\n-- Sending WSM ---------------------------------------------\n");
        }

        /* Receive MsgQ */
        len = recvMQ(pkt);
        if (len < 0)
            continue;
        else
        {

            /*
             * WSM MPDU 를 생성한다.
             */
            memset(&wsm_params, 0, sizeof(wsm_params));
            wsm_params.hdr_extensions.chan_num = true;
            wsm_params.hdr_extensions.datarate = true;
            wsm_params.hdr_extensions.transmit_power = true;
            wsm_params.ifindex = g_mib.netIfIndex;
            wsm_params.chan_num = g_mib.channel;
            wsm_params.timeslot = g_mib.timeSlot;
            wsm_params.datarate = g_mib.dataRate;
            wsm_params.transmit_power = g_mib.power;
            wsm_params.priority = g_mib.priority;
            memcpy(wsm_params.dst_mac_addr, g_mib.destMac, MAC_ALEN);
            memcpy(wsm_params.src_mac_addr, g_if1_mac_address, MAC_ALEN);
            wsm_params.psid = g_mib.psid;
            mpdu_size = Dot3_ConstructWsmMpdu(&wsm_params, pkt, len, mpdu, sizeof(mpdu));
            if (mpdu_size < 0) {
                //printf("Fail to Dot3_ConstructWsmMpdu() - %d\n", mpdu_size);
                //printf("------------------------------------------------------------\n\n");
                syslog(LOG_ERR | LOG_LOCAL7, "Fail to Dot3_ConstructWsmMpdu() - %d\n", mpdu_size);
                syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
                continue;
            }
            if (g_dbg >= kDbgMsgLevel_event) {
                {
                    //printf("[prcsWSM] Success to construct %d-bytes WSM MPDU\n", mpdu_size);
                    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to construct %d-bytes WSM MPDU\n", mpdu_size);
                }
                if (g_dbg >= kDbgMsgLevel_msgdump) {
                    for (int i = 0; i < mpdu_size; i++) {
                        if ((i!=0) && (i%16==0)) {
                            printf("\n");
                        }
                        printf("%02X ", mpdu[i]);
                    }
                    printf("\n");
                }
            }

            /*
             * WSM MPDU 를 전송한다.
             */
            memset(&al_params, 0, sizeof(al_params));
            al_params.channel = g_mib.channel;
            al_params.timeslot = g_mib.timeSlot; // 현재까지 TimeSlot_0 동작만 확인됨.
            al_params.datarate = g_mib.dataRate;
            al_params.expiry = 0;
            al_params.txpower = g_mib.power;
            int ret = Al_TransmitMpdu(g_mib.netIfIndex, mpdu, mpdu_size, &al_params);
            if (ret < 0) {
                //printf("Fail to Al_TransmitMpdu() - ret: %d\n", ret);
                //printf("------------------------------------------------------------\n\n");
                syslog(LOG_ERR | LOG_LOCAL7, "Fail to Al_TransmitMpdu() - ret: %d\n", ret);
                syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
                continue;
            } else {
                if (g_dbg >= kDbgMsgLevel_event)
                {
                    //printf("[prcsWSM] Success to Al_TransmitMpdu()\n");
                    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to Al_TransmitMpdu()\n");
                }
            }
            if (g_dbg >= kDbgMsgLevel_msgdump)
            {
                //printf("------------------------------------------------------------\n\n");
                syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
            }
        }
    } while(1);
}


#if 0
/**
 * WSM 송신타이머 만기 쓰레드. 송신타이머 만기 시마다 호출된다.
 *  - 송신타이머 컨디션 시그널을 전송하여 송신쓰레드가 깨어나도록 한다.
 *
 * @param arg   사용되지 않음
 */
static void V2X_OBU_WsmTxTimerThread(union sigval arg)
{
    pthread_mutex_lock(&g_tx_timer_mtx);
    pthread_cond_signal(&tx_timer_cond);
    pthread_mutex_unlock(&g_tx_timer_mtx);
}


/**
 * WSM 송신타이머를 초기화한다.
 *
 * @param interval  송신주기(usec)
 * @return          성공 시 0, 실패 시 -1
 */
static int V2X_OBU_InitWsmTxTimer(const uint32_t interval)
{
    int ret;
    struct itimerspec ts;
    struct sigevent se;

    printf("Initializing tx timer - interval: %uusec\n", interval);

    /*
     * 송신타이머 만기 시 송신타이머쓰레드(V2X_WSM_TxTimerThread)가 생성되도록 설정한다.
     */
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &g_tx_timer;
    se.sigev_notify_function = V2X_OBU_WsmTxTimerThread;
    se.sigev_notify_attributes = NULL;

    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 1000000;  // 최초타이머 주기 = 1msec
    ts.it_interval.tv_sec = interval / 1000000;
    ts.it_interval.tv_nsec = (interval % 1000000) * 1000;

    /*
     * 송신타이머 생성
     */
    ret = timer_create(CLOCK_REALTIME, &se, &g_tx_timer);
    if (ret) {
        perror("Fail to cerate timer: ");
        return -1;
    }

    /*
     * 송신타이머 주기 설정
     */
    ret = timer_settime(g_tx_timer, 0, &ts, 0);
    if (ret) {
        perror("Fail to set timer: ");
        return -1;
    }

    printf("Success to initialize tx timer.\n");
    return 0;
}
#endif

/**
 * WSM 송신동작을 초기화한다.
 *
 * @param timer_interval    송신타이머 주기(usec단위)
 * @return                  성공 시 0, 실패 시 -1
 */
int V2X_OBU_InitWsmTx(const uint32_t timer_interval)
{
    //printf("Initializing WSM tx operation\n");
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Initializing WSM tx operation\n");
    int ret = pthread_create(&g_tx_thread, NULL, V2X_OBU_WsmTxThread, NULL);
    if (ret < 0) {
        //perror("Fail to create WSM tx thread() ");
        syslog(LOG_ERR | LOG_LOCAL7, "Fail to create WSM tx thread() : %s\n", strerror(errno));
        return -1;
    }

    //printf("Success to initialize WSM tx operation\n");
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to initialize WSM tx operation\n");
    return 0;
}
