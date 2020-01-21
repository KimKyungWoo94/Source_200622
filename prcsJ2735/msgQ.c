#include <signal.h>
#include <msgQ.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <prcsJ2735.h>


/* 전역변수 */
mqd_t fd;
struct msgQ_elem_frame *msgqPkt = NULL; // 메시지 버퍼
uint32_t msgqCnt = 0;

int initMQ(void)
{
    if(g_mib.op == opType_rx)
    {
        /*  수신 메세지 큐용 버퍼 Allocation */
        msgqPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
        if( msgqPkt == NULL )
        {
            //printf("[prcsJ2735] Fail to allocate memory for packet message queue.\n");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Fail to allocate memory for packet message queue.\n");
            return -1;
        }
        /* 수신 Packet 메시지큐 생성 */
        fd = msgget((key_t)KEY_RECV_J2735, IPC_CREAT | 0666);
        if (fd < 0)
        {
            //perror("[prcsJ2735] msgget error ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] msgget error : %s", strerror(errno));
            return -1;
            free(msgqPkt);
            return -1;
        }
    }
    else
    {
        /* 송신 메세지 큐용 버퍼 Allocation */
        msgqPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
        if( msgqPkt == NULL )
        {
            //printf("[prcsJ2735] Fail to allocate memory for packet message queue.\n");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Fail to allocate memory for packet message queue.\n");
            return -1;
        }
        /* 송신 Packet 메시지큐 생성 */
        fd = msgget((key_t)KEY_SEND_J2735, IPC_CREAT | 0666);
        if (fd < 0)
        {
            //perror("[prcsJ2735] msgget error ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] msgget error : %s", strerror(errno));
            free(msgqPkt);
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
    if(g_mib.op == opType_rx)
    {
        /* 수신 Paket 메시지큐 닫기 */
        mq_close(fd);
        /* 수신 메세지 큐용 버퍼 FREE */
        free(msgqPkt);
    }
    else
    {
        /* 송신 Paket 메시지큐 닫기 */
        mq_close(fd);
        /* 송신 메세지 큐용 버퍼 FREE */
        free(msgqPkt);
    }
}

int recvMQ(char *pkt)
{
    memset(msgqPkt->msg.msg, 0, msgqPkt->msg.msg_len);

    if( msgrcv(fd, (char *)msgqPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), 1, 0) == -1 )
    {
        //perror("[prcsJ2735] MQ receive error :  " );
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] MQ receive error : %s", strerror(errno));
        return -1;
    }
    else
    {
        if( g_mib.dbg )
        {
            //printf("[prcsJ2735] MQ receive(len: %d)\n", msgqPkt->msg.msg_len);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] MQ receive(len: %d)\n", msgqPkt->msg.msg_len);
        }
        memcpy(pkt, msgqPkt->msg.msg, msgqPkt->msg.msg_len);
    }

    return msgqPkt->msg.msg_len;
}

void sendMQ(uint8_t *pPkt, uint32_t len)
{
    memset(msgqPkt->msg.msg, 0, msgqPkt->msg.msg_len);
    msgqPkt->msg.msg_len = len;
    memcpy(msgqPkt->msg.msg, pPkt, len);

    msgqPkt->rxCnt = msgqCnt++;
    msgqPkt->msgtype = 1; 

    if( msgsnd( fd, (char *)msgqPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), IPC_NOWAIT) == -1 )
    {
        //perror("[precsJ2735] MQ send error : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] MQ send error : %s", strerror(errno));
    }
    else
    {
        if( g_mib.dbg)
        {
            //printf("[prcsJ2735] MQ send(%d Byte) \n", msgqPkt->msg.msg_len);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] MQ send(%d Byte) \n", msgqPkt->msg.msg_len);
        }
    }
}

