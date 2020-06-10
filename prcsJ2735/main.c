#include <prcsJ2735.h>

/* 전역변수 */
mib_t		g_mib;
int ending = 0;

/* 시그널 핸들러 */
void sigint_handler(int signo)
{
    ending = 1;
}

int main(int argc, char *argv[])
{
	int32_t	result;

	/* MIB 초기화 및 입력 파라미터 설정 */
	memset(&g_mib, 0, sizeof(mib_t));
    g_mib.interval = 100000;//송신주기
    g_mib.gpsdPort = "2947"; //GPSD서버 포트번호

	/* 사용자가 입력한 파라미터들을 MIB에 저장한다. */
	result	=	ParsingOptions(argc, argv);
	if(result < 0)
		return	-1;

    /* 파라미터 출력 */
    PrintOptions();

#if 0
    /* syslog library open */
    openlog(prcsJ2735, LOG_CONS | LOG_NDELAY | LOG_PERROR, LOG_LOCAL0);
#endif

    /* 프로그램 종료 위한 시그널 등록 Ctrl+C  */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN); // 브로큰 파이프 시그널 무시

    /* Messge Queue 초기화 */
    if(initMQ() == -1)
        return -1;

    /* g_mib.op별 동작 실행 */
    if(g_mib.op == opType_rx)
        setJ2735rx();
    else
        setJ2735tx();

    /* Messge Queue 닫기 */
    releaseMQ();

    //closelog();

    return 0;
}

