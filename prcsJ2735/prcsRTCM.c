#include <prcsJ2735.h>

#define RTCM_MAX_SIZE 1024 

/* 전역변수 */
uint8_t rtcmBuf[1024];
int rtcmLen;
bool rtcmFlag = false;
//bool renewFlag = true;
//bool restartFlag = false;
pthread_mutex_t rtcmMtx;

void setRTCM_mutex(int op)
{
    if(op == 0) 
        pthread_mutex_init(&rtcmMtx, NULL);
    else if(op == 1)
        pthread_mutex_destroy(&rtcmMtx);

}

#if 0
void set_renewFlag()
{
    static renewCnt = 0;

    if(renewFlag == false)
        renewCnt++;
    else
        renewCnt == 0;

    if(renewCnt >= 10)
    {
        FILE   *restart_fd = NULL;

        restart_fd= fopen("restart.txt", "w" );
        if (restart_fd == NULL)
        {        
            perror("[restart.txt] open error :");
            return -1;
        }

        fclose(restart_fd);
    }
}
#endif

int rtcmPkt(struct gps_data_t * gpsData)
{
    static rtcmData_t rtcmData[6];

    /* rtcm type별 관리 */
    switch(gpsData->rtcm3.type)
    {
        case 1005 :
            rtcmData[0].type = 1005;
            rtcmData[0].len = gpsData->rtcm3.length+6;
            memset(rtcmData[0].buf, 0, 1024); 
            memcpy(rtcmData[0].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[0].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1005 \n");
            break;
        case 1077 :
            rtcmData[1].type = 1077;
            rtcmData[1].len = gpsData->rtcm3.length+6;
            memset(rtcmData[1].buf, 0, 1024); 
            memcpy(rtcmData[1].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[1].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1077\n");
            break;
        case 1087 :
            rtcmData[2].type = 1087;
            rtcmData[2].len = gpsData->rtcm3.length+6;
            memset(rtcmData[2].buf, 0, 1024); 
            memcpy(rtcmData[2].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[2].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1087\n");
            break;
        case 1097 :
            rtcmData[3].type = 1097;
            rtcmData[3].len = gpsData->rtcm3.length+6;
            memset(rtcmData[3].buf, 0, 1024); 
            memcpy(rtcmData[3].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[3].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1097\n");
            break;
        case 1127 :
            rtcmData[4].type = 1127;
            rtcmData[4].len = gpsData->rtcm3.length+6;
            memset(rtcmData[4].buf, 0, 1024); 
            memcpy(rtcmData[4].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[4].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1127\n");
            break;
        case 1230 :
            rtcmData[5].type = 1230;
            rtcmData[5].len = gpsData->rtcm3.length+6;
            memset(rtcmData[5].buf, 0, 1024); 
            memcpy(rtcmData[5].buf, gpsData->rtcm3.rtcmtypes.data, gpsData->rtcm3.length+6);
            rtcmData[5].flag = true;
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Read RTCM 1230\n");
            break;
    }

    /* rtcm 6개 type 데이터 모두 있을 경우 패킷 생성 */
    if( rtcmData[0].flag && rtcmData[1].flag && rtcmData[2].flag && rtcmData[3].flag && rtcmData[4].flag && rtcmData[5].flag)
    {
        pthread_mutex_lock(&rtcmMtx);
        memset(&rtcmBuf, 0, sizeof(rtcmBuf));
        rtcmLen = 0;
        for(int i=0; i<6; i++)
        {
            memcpy(rtcmBuf+rtcmLen, rtcmData[i].buf, rtcmData[i].len);
            rtcmLen += rtcmData[i].len;
            rtcmData[i].flag = false;
        }

        rtcmFlag = true;
        pthread_mutex_unlock(&rtcmMtx);
    }
    else
    {
        syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] No RTCM data\n");
        rtcmFlag == false;
        //    renewFlag = false;
    }

    return 0;

}

int getRTCM(uint8_t *buf)
{
    int i;

    if(rtcmFlag == true)
    {
        memcpy(buf, rtcmBuf, rtcmLen);
        return rtcmLen;
    }
    else
        return 0;

}

void setRTCM(uint8_t *buf, int len)
{
    memset(&rtcmBuf, 0, sizeof(rtcmBuf));
    memcpy(&rtcmBuf, buf, len); 
    rtcmLen = len;
    rtcmFlag = true;
}

int ConstructRTCM(uint8_t *pkt, uint32_t *len)
{
    MessageFrame *frame = NULL;
    void *msg;
    RTCMcorrections *pRTCM = NULL;
    static MsgCount cnt = 0;
    uint32_t result;
    asn1_ssize_t ret;

    /* Allocate Message Frame */
    frame = (MessageFrame *)asn1_malloc(sizeof(MessageFrame));
    if( frame  ==  NULL)
    {
        //perror("[prcsJ2735] Message frame memory allocation failed : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Message frame memory allocation failed : %s", strerror(errno));
        return -1;
    }

    /* Allocate RTCM Paket */
    pRTCM = (RTCMcorrections *)asn1_malloc(sizeof(RTCMcorrections));
    if( pRTCM== NULL)
    {
        //perror("[prcsJ2735] RTCM memory allocation failed : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] RTCM memory allocation failed : %s", strerror(errno));
        return -1;
    }

    /* Message Frame */
    frame->messageId = 28;
    frame->value.type = asn1_type_RTCMcorrections;
    frame->value.u.data = (void *)pRTCM; 

    /* RTCM */
    if(cnt > 127 ) cnt = 0; 
    pRTCM->msgCnt = cnt++;
    pRTCM->rev = RTCM_Revision_rtcmRev3;

    /* rtcm->msg 메모리 할당 */
    pRTCM->msgs.tab = (RTCMmessage *)asn1_malloc(sizeof(RTCMmessage));
    if( pRTCM->msgs.tab == NULL)
    {
        //perror("[prcsJ2735] RTCM->msgs.tab memeory allocation failed : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] RTCM->msgs.tab memeory allocation failed : %s", strerror(errno));
        return -1;
    }
    RTCMmessage * tab = pRTCM->msgs.tab;

    tab->buf = (uint8_t *)asn1_malloc(RTCM_MAX_SIZE);
    if( tab->buf == NULL)
    {
        //perror("[prcsJ2735] RTCM->msgs.tab->buf memeory allocation failed : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] RTCM->msgs.tab->buf memeory allocation failed : %s", strerror(errno));
        return -1;
    }

    /* RTCM 메모리 복사 */
    if(rtcmFlag == true )
    {
        pRTCM->msgs.count = 1;
        tab->len = getRTCM(tab->buf);

//        if(g_mib.dbg)
 //           asn1_xer_printf(asn1_type_MessageFrame, frame);

        /* 인코딩 */
        ret = asn1_uper_encode(&msg, asn1_type_MessageFrame, frame);
        if(ret < 0)
        {
            //printf("[prcsJ2735] RTCM encoding fail\n");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] RTCM encoding fail\n");
            result =  -1;
        }
        else
        {
            if( g_mib.dbg )
            {
                //printf("[prcsJ2735] Success RTCM encoding(%u Btye) \n", ret);
                syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Success RTCM encoding(%u Btye) \n", ret);
            }
            *len = (uint32_t)ret;
            memcpy(pkt, msg, ret);
            result = 0;
        }
    }
    else
        result = -1;

    /* 동적할당 메모리 해제 */
    asn1_free(tab->buf);
    asn1_free(tab);
    asn1_free(pRTCM);
    asn1_free(frame);

    return result;
}
