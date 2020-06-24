/**********************************************************
  [sigevent]
  se.sigev_notify = SIGEV_THREAD : 뭐로 하겠다 알려주는것 (쓰레드 ,signal , NONE)
  se.sigev_notify_function = TxTimerExpired : 통지 쓰레드 함수
  se.sigev_notify_attributes = NULL : 통지 쓰레드 속성
  se.sigev_value.sival_ptr = &g_mib.timer; : 포인터 전달

  [itimerspec]
  초 / 나노초 설정
  주기 : 초 /나노초 설정

  [Mutex]
  상호배제, 쓰레드간 공유하는 데이터 영역 보호 
  임계영역내 단 하나의 쓰레드만 진입가능

  mutex_lock : 임계영역 진입하기 위한 뮤텍스 잠금요청 / 
  최근상태가 unlock이면 잠금얻고 임계영역 진입하고 리턴 / 
  다른 쓰레드가 뮤텍스 잠금 얻은 상태면 기다림

  mutex_unlock : 뮤텍스 잠금 되돌려줌

  [Cond]
  pthread_cond_wait
  동작중인 thread를 잠시 중단 /
  condition과 mutex인자를 모두 적용

  pthread_cond_signal
  pthread_cond_wait() 함수를 실행중인 하나의 thread를 깨움 /
  Thread가 다수일 경우 단 하나의 thread만 깨어남

  pthread_cond_broadcast
  cond 인자를 가지고 pthread_cond_wait() 함수를 실행중인 모든 thread를 깨움 /
  만약 Thread가 다수일 경우 mutex를 먼저 잡은 thread가 먼저 동작 /
  나머지 thread는 mutex를 받을 때 까지 대기상태를 유지

 ************************************************************/

#include <PAR.h>

/* 함수원형 */
static void TxTimerExpired(union sigval arg);


/**
 * PAR 송신타이머를 초기화한다.
 *
 * @param interval  송신주기(usec)
 * @return          성공 시 0, 실패 시 -1
 */
int InitTxTimer(const uint32_t interval)
{
	int ret;
	struct itimerspec ts; 
	struct sigevent se;

	printf("Initializing tx timer - interval: %uusec\n", interval);

	/*
	 * 송신타이머 만기 시 송신타이머쓰레드(TxThread)가 생성되도록 설정한다.
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

/**
 * 타이머 만기함수
 *
 */
static void TxTimerExpired(union sigval arg)
{
	pthread_mutex_lock(&g_mib.txMtx);
	pthread_cond_signal(&g_mib.txCond);
	//pthread_cond_broadcast(&g_mib.txCond);//1초 됬어 동작해알려줌
	pthread_mutex_unlock(&g_mib.txMtx);
}
