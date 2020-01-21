/**
 * @file v2x-obu.c
 * @date 2019-08-19
 * @author gyun
 * @brief v2x-obu 어플리케이션 메인 파일
 */


#include "dot3/dot3.h"
#include "wlanaccess/wlanaccess.h"

#include "v2x-obu.h"


struct V2X_OBU_MIB g_mib; ///< 어플리케이션 관리정보
DbgMsgLevel g_dbg = kDbgMsgLevel_nothing;  ///< 디버그메시지 출력레벨

/*
 * 각 인터페이스 별 MAC 주소
 */
const uint8_t g_if0_mac_address[] = { 0x00, 0x49, 0x54, 0x45, 0xCC, 0x00};
const uint8_t g_if1_mac_address[] = { 0x00, 0x49, 0x54, 0x45, 0xCC, 0x01};


/**
 * V2X 라이브러리를 초기화한다.
 *
 * @retrun 성공 시 0, 실패 시 -1
 */
static int V2X_OBU_InitV2XLibs(void)
{
    //printf("Initializing V2X libraries\n");
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Initializing V2X libraries\n");
    int ret;

    /*
     * dot3 라이브러리 초기화
     */
    ret = V2X_OBU_InitDot3Library(0);
    if (ret < 0) {
        //printf("Fail to initialize dot3 library\n");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] Fail to initialize dot3 library\n");
        return -1;
    }

    /*
     * 액세스계층 라이브러리 열기
     */
    ret = V2X_OBU_OpenAccessLibrary(0);
    if (ret < 0) {
        //printf("Fail to initialize access library\n");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] Fail to initialize access library\n");
        return -1;
    }

    //printf("Success to initialize V2X libraries\n");
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Success to initialize V2X libraries\n");
    return 0;
}



/**
 * 어플리케이션 메인 함수
 *
 * @param argc  어플리케이션 실행 시 입력되는 명령줄 내 파라미터들의 개수 (어플리케이션 실행파일명 포함)
 * @param argv  어플리케이션 실행 시 입력되는 명령줄 내 파라미터들의 문자열 집합 (어플리케이션 실행파일명 포함)
 * @return      성공 시 0, 실패 시 -1
 */
#define V2X_OBU_Main  main
int V2X_OBU_Main(int argc, char *argv[])
{
    int ret;

    //printf("Running v2x-obu application..\n");
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Running v2x-obu application..\n");

    /* MIB 초기화 및 입력 파라미터 설정 */
    memset(&g_mib, 0, sizeof(struct V2X_OBU_MIB));
    g_mib.netIfIndex = 0;
    g_mib.channel = 178;
    g_mib.priority = 7;
    g_mib.timeSlot = kDot3TimeSlot_0;
    g_mib.dataRate = 12;
    g_mib.power = 20;
    memset(g_mib.destMac, 0xff, kDot3MacAddrSize);

	/* 사용자가 입력한 파라미터들을 MIB에 저장한다. */
	ret =	ParsingOptions(argc, argv);
	if(ret < 0)
		return	-1;

     /* 라이브러리 초기화 */
    ret = V2X_OBU_InitV2XLibs();
    if (ret < 0) {
        return -1;
    }

    /* 채널 접속*/
    ret = V2X_OBU_AccessChannel(g_mib.netIfIndex, g_mib.channel, g_mib.channel);
    if (ret < 0) {
        return ret;
    }
    //printf("Success to access chanel %d on if%d\n", g_mib.channel, g_mib.netIfIndex);
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Success to access chanel %d on if%d\n", g_mib.channel, g_mib.netIfIndex);

#if 0
    q
    /* 인터페이스 MAC 주소 설정*/
    ret = V2X_OBU_SetIfMacAddress(g_mib.netIfIndex, g_if0_mac_address);
    if (ret < 0) {
        return ret;
    }
    printf("Success to set mac address %02X:%02X:%02X:%02X:%02X:%02X on if0\n",
            g_if0_mac_address[0], g_if0_mac_address[1], g_if0_mac_address[2],
            g_if0_mac_address[3], g_if0_mac_address[4], g_if0_mac_address[5]);
#endif

    /* MsgQ Open */
    if(initMQ() == -1)
        return -1;

    if(g_mib.op == opTX || g_mib.op == opTRX)
    {
        /* WSM 송신 타이머 생성- 시나리오: WSM을 정해진 주기로 전송된다.*/
        ret = V2X_OBU_InitWsmTx(WSM_TX_INTERVAL);
        if (ret < 0) {
            return -1;
        }

        /* 액세스계층 이벤트 폴링 쓰레드 종료 대기 - 프로그램 종료 방지*/
        V2X_OBU_WaitEventPolling();
    }
    else if(g_mib.op == opRX || g_mib.op == opTRX)
    {
#if 0
        ret = rtcmCheckTimer(1000000);
        if(ret < 0)
        {
            printf("prcsWSM] Timer init fail \n");
            return -1;
        }
#endif
        while(1)
        {
            sleep(1);
        }

    }
    /* MsgQ Close */
    releaseMQ();


    return 0;
}
