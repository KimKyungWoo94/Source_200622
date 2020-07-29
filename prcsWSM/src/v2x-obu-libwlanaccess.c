/**
 * @file v2x-obu-wlanaccess.c
 * @date 2019-08-19
 * @author gyun
 * @brief wlanaccess 라이브러리 관련 기능 구현
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "wlanaccess/wlanaccess.h"

#include "v2x-obu.h"


pthread_t g_poll_thread; ///< 이벤트 폴링 쓰레드


/// 채널접속이 완료되었는지 여부를 나타내는 변수. 채널접속요청 API 와 채널접속결과 콜백함수에서 사용된다.
volatile bool g_chan_access_complete = false;
/// MAC주소설정이 완료되었는지 여부를 나타내는 변수. MAC주소설정요청 API 와 MAC주소설정결과 콜백함수에서 사용된다.
volatile bool g_set_if_mac_addr_done = false;


/**
 * MPDU 수신처리 콜백함수. access 라이브러리에서 호출된다.
 *
 * @param mpdu
 * @param mpdu_size
 * @param rxparams
 */
static void V2X_OBU_ProcessRxMpduCallback(
        const uint8_t *const mpdu,
        const AlMpduSize mpdu_size,
        const struct AlMpduRxParams *const rxparams)
{
    if(g_dbg)
    {
#if 0
        printf("\n-- Processing received MPDU --------------------------------\n");
        printf("Rx MPDU callback - MPDU size: %u, ifindex: %u, timeslot: %u, channel: %u, "
                "rxpower: %d(0.5dBm), rcpi: %u, datarate: %u(500kbps)\n",
                mpdu_size, rxparams->ifindex, rxparams->timeslot, rxparams->channel,
                rxparams->rxpower, rxparams->rcpi, rxparams->datarate);
#endif
        syslog(LOG_INFO | LOG_LOCAL6, "\n-- Processing received MPDU --------------------------------\n");
        syslog(LOG_INFO | LOG_LOCAL6, "Rx MPDU callback - MPDU size: %u, ifindex: %u, timeslot: %u, channel: %u, "
                "rxpower: %d(0.5dBm), rcpi: %u, datarate: %u(500kbps)\n",
                mpdu_size, rxparams->ifindex, rxparams->timeslot, rxparams->channel,
                rxparams->rxpower, rxparams->rcpi, rxparams->datarate);
    }

    g_mib.rcpi = rxparams->rcpi;
    g_mib.rxpower = rxparams->rxpower/2;
    V2X_OBU_ProcessRxMpdu(mpdu, mpdu_size);
}


/**
 * 채널접속결과 결과 수신 콜백 함수.
 *
 * @param ifindex 채널접속된 인터페이스 식별번호
 */
static void V2X_OBU_ProcessAccessChannelResultCallback(const AlIfIndex ifindex)
{
    //printf("Access channel result callback - ifindex: %u\n", ifindex);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Access channel result callback - ifindex: %u\n", ifindex);
    g_chan_access_complete = true;
}


/**
 * 송신 결과 수신 콜백 함수
 *
 * @param result 송신 요청 처리 결과
 * @param dev_specific_errcode 에러 발생 시(result<0), 디바이스별로 정의된 상세 에러코드
 */
static void V2X_OBU_ProcessTransmitResultCallback(const AlTxResultCode result, const int dev_specific_errcode)
{
    if(g_dbg)
        //printf("Transmit result callback - result: %d, errcode: %d\n", result, dev_specific_errcode);
        syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Transmit result callback - result: %d, errcode: %d\n", result, dev_specific_errcode);
}


/**
 * MAC주소설정결과 결과 수신 콜백 함수
 *
 * @param ifindex MAC주소 설정된 인터페이스 식별번호
 */
static void V2X_OBU_ProcessSetIfMacAddressResultCallback(const AlIfIndex ifindex)
{
    //printf("Set interface MAC address result callback - ifindex: %u\n", ifindex);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Set interface MAC address result callback - ifindex: %u\n", ifindex);
    g_set_if_mac_addr_done = true;
}



/**
 * 이벤트 폴링 쓰레드
 *  - Al_PollEvent()를 호출해 두어야 콜백함수가 호출될 수 있다.
 *  - 이 스레드는 어플리케이션이 종료되기 전까지는 종료되지 않는다.
 *
 * @param none
 * @return
 */
static void* V2X_OBU_PollEvent(void *none)
{
    //printf("Polling access layer event\n");
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Polling access layer event\n");
    int ret = Al_PollEvent();
    if (ret < 0) {
        //printf("Fail to poll event - ret: %d\n", ret);
        syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to poll event - ret: %d\n", ret);
    }
    return NULL;
}


/**
 * 액세스계층 라이브러리 관련 초기화를 수행한다.
 *  - 라이브러리를 초기화한다.
 *  - 콜백함수들을 등록한다.
 *
 * @param log_level 로그메시지 출력레벨
 * @return          성공 시 0, 실패 시 -1
 */
int V2X_OBU_OpenAccessLibrary(const int log_level)
{
    int ret;

    //printf("Opening access library - log_level: %d\n", log_level);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Opening access library - log_level: %d\n", log_level);

    /*
     * 라이브러리 열기
     */
    ret = Al_Open(log_level);
    if (ret < 0) {
        //printf("Fail to Al_Open() - %d\n", ret);
        syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to Al_Open() - %d\n", ret);
        return -1;
    }
    g_mib.if_num = ret;

    /*
     * 콜백함수 등록
     */

    if(g_mib.op == opRX || g_mib.op == opTRX)
        Al_RegisterCallbackRxMpdu(V2X_OBU_ProcessRxMpduCallback);
    else if(g_mib.op == opTX || g_mib.op == opTRX)
        Al_RegisterCallbackTransmitResult(V2X_OBU_ProcessTransmitResultCallback);
    Al_RegisterCallbackAccessChannelResult(V2X_OBU_ProcessAccessChannelResultCallback);
    Al_RegisterCallbackSetIfMacAddressResult(V2X_OBU_ProcessSetIfMacAddressResultCallback);

    /*
     * 액세스계층 이벤트 폴링 쓰레드 생성
     *  - 콜백함수 등록 후 이벤트 폴링을 시작한다.
     */
    ret = pthread_create(&g_poll_thread, NULL, V2X_OBU_PollEvent, NULL);
    if (ret) {
        //perror("Fail to create poll thread ");
        syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to create poll thread : %s", strerror(errno));
        return -1;
    }

    //printf("Success to open access library - %d interface is supported\n", g_mib.if_num);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to open access library - %d interface is supported\n", g_mib.if_num);
    return 0;
}


/**
 * 채널 접속을 수행한다.
 *
 * @param if_idx    채널접속한 인터페이스 식별번호
 * @param ts0_chan  TS0 채널번호
 * @param ts1_chan  TS1 채널번호
 * @return          성공 시 0, 실패 시 -1
 */
int V2X_OBU_AccessChannel(const uint8_t if_idx, const uint8_t ts0_chan, const uint8_t ts1_chan)
{
    //printf("Accessing channel - if_idx: %u, ts0_chan: %u, ts1_chan: %u\n", if_idx, ts0_chan, ts1_chan);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Accessing channel - if_idx: %u, ts0_chan: %u, ts1_chan: %u\n", if_idx, ts0_chan, ts1_chan);

    g_chan_access_complete = false;
    int ret = Al_AccessChannel(if_idx, ts0_chan, ts1_chan);
    if (ret < 0) {
        //printf("Fail to Al_AccessChannel() - %d\n", ret);
        syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to Al_AccessChannel() - %d\n", ret);
        return ret;
    }
    //while(!g_chan_access_complete); // 채널접속결과 콜백함수가 호출될 때까지 대기
    usleep(10000);

    //printf("Success to access channel\n");
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to access channel\n");
    return 0;
}


/**
 * 접속채널을 확인한다.
 *
 * @param ts0_chan  TS0 채널번호가 저장될 변수의 포인터
 * @param ts1_chan  TS1 채널번호가 저장될 변수의 포인터
 * @return          성공 시 0, 실패 시 -1
 */
int V2X_OBU_GetCurrentChannel(uint8_t *const ts0_chan, uint8_t *const ts1_chan)
{
    int ret;
    for (int i = 0; i < g_mib.if_num; i++) {
        //printf("Get current channel on interface %d\n", i);
        syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Get current channel on interface %d\n", i);
        ret = Al_GetCurrentChannel(i, ts0_chan, ts1_chan);
        if (ret < 0) {
            //printf("Fail to Al_GetCurrentChannel() on interface %d - %d\n", i, ret);
            syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to Al_GetCurrentChannel() on interface %d - %d\n", i, ret);
            return ret;
        }
        //printf("Success to get current channel on interface %d - ts0: %u, ts1: %u\n", i, *ts0_chan, *ts1_chan);
        syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to get current channel on interface %d - ts0: %u, ts1: %u\n", i, *ts0_chan, *ts1_chan);
    }
    return 0;
}


/**
 * 인터페이스 MAC 주소를 설정한다.
 *
 * @param ts0_chan  TS0 채널번호가 저장될 변수의 포인터
 * @param ts1_chan  TS1 채널번호가 저장될 변수의 포인터
 * @return          성공 시 0, 실패 시 -1
 */
int V2X_OBU_SetIfMacAddress(const uint8_t if_idx, const uint8_t *addr)
{
    int ret;
    //printf("Set interface MAC address %02X:%02X:%02X:%02X:%02X:%02X on interface %d\n",
     //       addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], if_idx);
    syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Set interface MAC address %02X:%02X:%02X:%02X:%02X:%02X on interface %d\n",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], if_idx);

    g_set_if_mac_addr_done = false;
    ret = Al_SetIfMacAddress(if_idx, addr);
    if (ret < 0) {
        //printf("Fail to Al_SetIfMacAddress() - ret: %d\n", ret);
        syslog(LOG_ERR | LOG_LOCAL7, "[prcsWSM] Fail to Al_SetIfMacAddress() - ret: %d\n", ret);
        return -1;
    }
    while(!g_set_if_mac_addr_done);

    //printf("Success to set interface mac address\n");
        syslog(LOG_INFO | LOG_LOCAL6, "[prcsWSM] Success to set interface mac address\n");
    return 0;
}


/**
 * 이벤트 폴링을 대기한다.
 */
void V2X_OBU_WaitEventPolling(void)
{
    int ret;
    pthread_join(g_poll_thread, (void **)&ret);
}
