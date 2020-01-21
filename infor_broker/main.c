#include <infor_broker.h>
#include <signal.h>

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
    memcpy(g_mib.cnvcIP, "192.168.101.2", sizeof("192.168.101.2"));
    g_mib.cnvcPort = 17000;
    memcpy(g_mib.adasIP, "192.168.101.4",  sizeof("192.168.101.4"));
    g_mib.adasPort = 17000;
    g_mib.serverPort = 15000;

	/* 사용자가 입력한 파라미터들을 MIB에 저장한다. */
	result	=	ParsingOptions(argc, argv);
	if(result < 0)
		return	-1;
    else if(result == 2)
        return 0;

    /* 프로그램 종료 위한 시그널 등록 Ctrl+C  */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    /* 쓰레드 생성 */
    if(createSockThread() < 0)
        return -1;

    /* sock check */
    while(!ending)
    {
        connection_Check();

        sleep(1);
    }

    /* 쓰레드 닫기 */
    closeSocketThread();

    return 0;
}

