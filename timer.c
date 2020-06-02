#include <PAR.h>

/* 함수원형 */
static void TxTimerExpired(union sigval arg);


/**
 * WSM 송신타이머를 초기화한다.
 *
 * @param interval  송신주기(usec)
 * @return          성공 시 0, 실패 시 -1
 */
int InitTxTimer(const uint32_t interval)
{
	int ret;
	struct itimerspec ts; //
	struct sigevent se;

	printf("Initializing tx timer - interval: %uusec\n", interval);

	/*
	 * 송신타이머 만기 시 송신타이머쓰레드(V2X_WSM_TxTimerThread)가 생성되도록 설정한다.
	 */
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &g_mib.timer;
	se.sigev_notify_function = TxTimerExpired ;
	se.sigev_notify_attributes = NULL;

	ts.it_value.tv_sec = 0;
	ts.it_value.tv_nsec = 1000000;  // 최초타이머 주기 = 1msec
	ts.it_interval.tv_sec = interval / 1000000; //연속적인 타이머 인터럽트 0이면 1번만보냄
	ts.it_interval.tv_nsec = (interval % 1000000) * 1000;

	/*
	 * 송신타이머 생성
	 */
	ret = timer_create(CLOCK_REALTIME, &se, &g_mib.timer);
	if (ret) {
		perror("Fail to cerate timer: ");
		return -1;
	}

	/*
	 * 송신타이머 주기 설정
	 */
	ret = timer_settime(g_mib.timer, 0, &ts, 0);
	if (ret) {
		perror("Fail to set timer: ");
		return -1;
	}

	printf("Success to initialize tx timer.\n");
	return 0;
}

static void TxTimerExpired(union sigval arg)
{
		pthread_mutex_lock(&g_mib.txMtx);
		pthread_cond_signal(&g_mib.txCond);
		//pthread_cond_broadcast(&g_mib.txCond);//1초 됬어 동작해알려줌
		pthread_mutex_unlock(&g_mib.txMtx);
}
