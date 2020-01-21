#include <infor_broker.h>
#include <getopt.h>

/*	전역변수 */
static const char	*optStr	=	"123456";
struct option options[] =
{
    {"serverPort", required_argument, 0, '1'},
    {"cnvIP", required_argument, 0, '2'},
    {"cnvPort", required_argument, 0, '3'},
    {"adasIP", required_argument, 0, '4'},
    {"adasPort", required_argument, 0, '5'},
    {"debug", required_argument, 0, '6'},
    {"help", no_argument, 0, '7'},
    {0, 0, 0, 0} // 옵션 배열은 {0,0,0,0} 센티넬에 의해 만료된다.
};


/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/

static void usage()
{
	printf("\nOPTIONS\n");
    printf("  --serverPort                   Set UDP server port(communication to infor_trans)\n");
    printf("  --cnvIP                        Set CARNAVICOM control center IP\n");
    printf("  --cnvPort                      Set CARNAVICOM control center Port\n");
    printf("  --adasIP                       Set ADAS ONE control center IP\n");
    printf("  --adasPort                     Set ADAS ONE control center Port\n");
    printf("  --debug                        Activate debug message output\n");
    printf("  --help                         Print usage\n");
    printf("\n");
}

/****************************************************************************************

	ParsingOptions()
		사용자가 입력한 옵션 파라미터들을 파싱하여 MIB에 저장

	arguments
		argc		사용자 입력 파라미터 개수
		argv		사용자 입력 파라미터들

	return

****************************************************************************************/
int32_t ParsingOptions(int32_t argc, char *argv[])
{
	int	c;

	/*----------------------------------------------------------------------------------*/
	/* 파라미터 파싱 및 저장 */
	/*----------------------------------------------------------------------------------*/
	while(1) {
		int optionIndex	=	0;

		c	=	getopt_long(argc, argv, optStr, options, &optionIndex);

		/* 마지막 옵션 */
		if(c == -1)
			break;

        switch(c)
        {
            case 0:
                break;
            case '1':
                g_mib.serverPort	=  (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case '2':
                memset(g_mib.cnvcIP, 0, ADDRSIZE);
                memcpy(g_mib.cnvcIP, optarg, strlen(optarg) < ADDRSIZE ? strlen(optarg) : ADDRSIZE );
                break;
            case '3':
                g_mib.cnvcPort = 0;
                g_mib.cnvcPort	=  (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case '4':
                memset(g_mib.adasIP, 0, ADDRSIZE);
                memcpy(g_mib.adasIP, optarg, strlen(optarg) < ADDRSIZE ? strlen(optarg) : ADDRSIZE );
                break;
            case '5':
                g_mib.adasPort = 0;
                g_mib.adasPort	=  (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case '6':
                g_mib.dbg	=   (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case '7':
                usage();
                return 2;
                break;
            default:
                break;
        }
    }
    /*----------------------------------------------------------------------------------*/

    return 0;
}



