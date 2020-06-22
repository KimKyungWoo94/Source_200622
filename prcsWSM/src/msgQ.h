#ifndef _CNVC_MSGQ_H_
#define _CNVC_MSGQ_H_

/****************************************************************************************
	시스템 헤더

****************************************************************************************/
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdint.h>
#include <errno.h>
/****************************************************************************************
	프로젝트 헤더

****************************************************************************************/

#define KEY_RECV_J2735 1716
#define KEY_SEND_J2735 1717
#define KEY_SEND_PAR 1718
#define MSGMAX 4096

typedef enum msgType {
   msgq_msgtype_messageframe,
}MSGQ_MSGTYPE;
   
typedef struct msgq_msg
{
   uint32_t	msg_len;
   uint8_t msg[MSGMAX];
}MSGQ_MSG;

struct msgQ_elem_frame
{
   long msgtype;
   uint32_t rxCnt;
   MSGQ_MSG msg;
};


static struct mq_attr cn_MQ_attr = {O_NONBLOCK, 10, sizeof(struct msgQ_elem_frame), 0};
#endif /* !_CNVC_MSGQ_H_ */

/* 함수원형 */
int initMQ(void);
void releaseMQ(void);
int recvMQ(char *pkt);
void sendMQ(uint8_t *pPkt, uint32_t len);
void PARsendMQ(uint8_t *pPkt, uint32_t len);
