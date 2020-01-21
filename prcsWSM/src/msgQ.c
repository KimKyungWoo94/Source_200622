#include <signal.h>
#include <msgQ.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <v2x-obu.h>


/* 전역변수 */
mqd_t recvFD, sendFD;
struct msgQ_elem_frame *recvPkt = NULL; // 메시지 버퍼
struct msgQ_elem_frame *sendPkt = NULL; // 메시지 버퍼
uint32_t msgqCnt = 0;

int initMQ(void)
{
    if(g_mib.op == opRX || g_mib.op == opTRX)
    {
        /*  수신 메세지 큐용 버퍼 Allocation */
        recvPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
        if( recvPkt == NULL )
        {
            //printf("[prcsWSM] Fail to allocate memory for receive packet message queue.\n");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] Fail to allocate memory for receive packet message queue.\n");
            return -1;
        }
        /* 수신 Packet 메시지큐 생성 */
        recvFD = msgget((key_t)KEY_RECV_J2735, IPC_CREAT | 0666);
        if (recvFD < 0)
        {
            //perror("[prcsWSM] msgget error ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] msgget error : %s", strerror(errno));
            free(recvPkt);
            return -1;
        }
    }
    if(g_mib.op == opTX || g_mib.op == opTRX)
    {
        /* 송신 메세지 큐용 버퍼 Allocation */
        sendPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
        if( sendPkt == NULL )
        {
            //printf("[prcsWSM] Fail to allocate memory for send packet message queue.\n");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] Fail to allocate memory for send packet message queue.\n");
            return -1;
        }
        /* 송신 Packet 메시지큐 생성 */
        sendFD = msgget((key_t)KEY_SEND_J2735, IPC_CREAT | 0666);
        if (sendFD < 0)
        {
            //perror("[prcsWSM] msgget error ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] msgget error : %s", strerror(errno));
            free(sendPkt);
            return -1;
        }
    }

    return 0;
}

/****************************************************************************************

  releaseMQ()
  메시지 큐 Close

  arguments

  return

 ****************************************************************************************/
void releaseMQ(void)
{
    if(g_mib.op == opRX || g_mib.op == opTRX)
    {
        /* 수신 Paket 메시지큐 닫기 */
        mq_close(recvFD);
        /* 수신 메세지 큐용 버퍼 FREE */
        free(recvPkt);
    }
    if(g_mib.op == opTX || g_mib.op == opTRX)
    {
        /* 송신 Paket 메시지큐 닫기 */
        mq_close(sendFD);
        /* 송신 메세지 큐용 버퍼 FREE */
        free(sendPkt);
    }
}

int recvMQ(char *pkt)
{
    memset(sendPkt->msg.msg, 0, sendPkt->msg.msg_len);

    if( msgrcv(sendFD, (char *)sendPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), 1, 0) == -1 )
    {
        //perror("[prcsWSM] MQ receive error :  " );
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] MQ receive error : %s", strerror(errno));
        return -1;
    }
    else
    {
        if (g_dbg >= kDbgMsgLevel_event)
        {
            //printf("[prcsWSM] MQ receive(len: %d)\n", sendPkt->msg.msg_len);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] MQ receive(len: %d)\n", sendPkt->msg.msg_len);
        }
        memcpy(pkt, sendPkt->msg.msg, sendPkt->msg.msg_len);
    }

    return sendPkt->msg.msg_len;
}

void sendMQ(uint8_t *pPkt, uint32_t len)
{
    memset(recvPkt->msg.msg, 0, recvPkt->msg.msg_len);
    recvPkt->msg.msg_len = len;
    memcpy(recvPkt->msg.msg, pPkt, len);

    recvPkt->rxCnt = msgqCnt++;
    recvPkt->msgtype = 1; 

    if( msgsnd( recvFD, (char *)recvPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), IPC_NOWAIT) == -1 )
    {
        //perror("[precsWSM] MQ send error : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] MQ send error : %s", strerror(errno));
    }
    else
    {
        if (g_dbg >= kDbgMsgLevel_event)
        {
            //printf("[prcsWSM] MQ send(%d Byte) \n", recvPkt->msg.msg_len);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] MQ send(%d Byte) \n", recvPkt->msg.msg_len);
        }
    }
}

