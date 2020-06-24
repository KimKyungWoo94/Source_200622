#include <signal.h>
#include <msgQ.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <PAR.h>


/* 전역변수 */
mqd_t recvFD, sendFD;
struct msgQ_elem_frame *recvPkt = NULL; // 메시지 버퍼
struct msgQ_elem_frame *sendPkt = NULL; // 메시지 버퍼
uint32_t msgqCnt = 0;

int initMQ(void)
{
	if(g_mib.op == opRX)
	{
		/*  수신 메세지 큐용 버퍼 Allocation */
		recvPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
		if( recvPkt == NULL )
		{
			//printf("[PAR] Fail to allocate memory for receive packet message queue.\n");
			syslog(LOG_ERR | LOG_LOCAL3, "[PAR] Fail to allocate memory for receive packet message queue.\n");
			return -1;
		}
		/* 수신 Packet 메시지큐 생성 */
		recvFD = msgget((key_t)KEY_SEND_PAR, IPC_CREAT | 0666);
		if (recvFD < 0)
		{
			//perror("[PAR] msgget error ");
			syslog(LOG_ERR | LOG_LOCAL3, "[PAR] msgget error : %s", strerror(errno));
			free(recvPkt);
			return -1;
		}
	}
	if(g_mib.op == opTX)
	{
		/* 송신 메세지 큐용 버퍼 Allocation */
		sendPkt = (struct msgQ_elem_frame *)calloc(1, sizeof(struct msgQ_elem_frame));
		if( sendPkt == NULL )
		{
			//printf("[PAR] Fail to allocate memory for send packet message queue.\n");
			syslog(LOG_ERR | LOG_LOCAL3, "[PAR] Fail to allocate memory for send packet message queue.\n");
			return -1;
		}
		/* 송신 Packet 메시지큐 생성 */
		sendFD = msgget((key_t)KEY_SEND_J2735, IPC_CREAT | 0666);
		if (sendFD < 0)
		{
			//perror("[PAR] msgget error ");
			syslog(LOG_ERR | LOG_LOCAL3, "[PAR] msgget error : %s", strerror(errno));
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
	if(g_mib.op == opRX)
	{
		/* 수신 Paket 메시지큐 닫기 */
		mq_close(recvFD);
		/* 수신 메세지 큐용 버퍼 FREE */
		free(recvPkt);
	}
	if(g_mib.op == opTX)
	{
		/* 송신 Paket 메시지큐 닫기 */
		mq_close(sendFD);
		/* 송신 메세지 큐용 버퍼 FREE */
		free(sendPkt);
	}
}

int recvMQ(char *pkt)
{
	static int cnt = 0;
	memset(recvPkt->msg.msg, 0, recvPkt->msg.msg_len);


	if( msgrcv(recvFD, (char *)recvPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), 1, 0) == -1 )
	{
		//perror("[PAR] MQ receive error :  " );
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR] MQ receive error : %s", strerror(errno));
		return -1;
	}
	else
	{
		cnt++;
#if 0
		if (g_mib.dbg)
		{
			//printf("[PAR] %dth MQ receive(len: %d)\n",cnt, recvPkt->msg.msg_len);
			syslog(LOG_INFO | LOG_LOCAL2, "[PAR] %dth MQ receive(len: %d)\n", cnt, recvPkt->msg.msg_len);
		}
#endif
		memcpy(pkt, recvPkt->msg.msg, recvPkt->msg.msg_len);
	}

	return recvPkt->msg.msg_len;
}

void sendMQ(uint8_t *pPkt, uint32_t len)
{
	static int cnt = 0;
	int result;
	memset(sendPkt->msg.msg, 0, sendPkt->msg.msg_len);
	sendPkt->msg.msg_len = len;
	memcpy(sendPkt->msg.msg, pPkt, len);

	sendPkt->rxCnt = msgqCnt++;
	sendPkt->msgtype = 1; 

	// if( msgsnd( recvFD, (char *)recvPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), IPC_NOWAIT) == -1 )
	result = msgsnd( sendFD, (char *)sendPkt, sizeof(struct msgQ_elem_frame) - sizeof(long), IPC_NOWAIT);
	if( result < 0 )
	{
		//perror("[PAR] MQ send error : ");
		syslog(LOG_ERR | LOG_LOCAL3, "[PAR] MQ send error : %s", strerror(errno));
	}
	else 
	{
		cnt++;
		if (g_mib.dbg)
		{
	//		for(int i=0;i<sendPkt->msg.msg_len;i++){
	//			printf("[0x%2x] ",sendPkt->msg.msg[i]);
	//		}
	//		printf("\n");
			//printf("[PAR] %dth MQ send(%d Byte) \n",cnt, sendPkt->msg.msg_len);
			 syslog(LOG_INFO | LOG_LOCAL2, "[PAR] %dth MQ send(%d Byte) \n", cnt, sendPkt->msg.msg_len);
		}
	}
}

