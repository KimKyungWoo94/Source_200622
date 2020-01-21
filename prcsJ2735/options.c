#include <prcsJ2735.h>
#include <getopt.h>

/*	전역변수 */
static const char	*optStr	=	"123456789";
struct option options[] =
{
	{"op", required_argument, 0, '1'},
	{"sock", required_argument, 0, '2'},
	{"msg", required_argument, 0, '3'},
	{"interval", required_argument, 0, '4'},
	{"port", required_argument, 0, '5'},
	{"debug", required_argument, 0, '6'},
	{"help", no_argument, 0, '7'},
	{"udpPort", required_argument, 0, '8'},
	{"udpIP", required_argument, 0, '9'},
    {0, 0, 0, 0} // 옵션 배열은 {0,0,0,0} 센티넬에 의해 만료된다.
};


/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/

static void usage()
{
	printf("\nOPTIONS\n");
	printf("  --op=<tx|rx>                   indicate tx or rx\n");
	printf("  --sock=<server|client>         indicate server or client\n");
	printf("  --msg=<message>                indicate to message\n");
	printf("                                    possible: pvd, bsm, rtcm, tim, rsa, mapdata, spat\n");
	printf("  --interval<usec>               Set tx interval(for tx)\n");
	printf("                                    if not set, interval : 100ms\n");
	printf("  --port                         Set port for GPSD sock\n");
	printf("  --debug                        activate debug message output\n");
	printf("  --help                         print usage\n");
	printf("  --udpPort                      Set port for UDP\n");
	printf("  --udpIP                        Set IP for UDP\n");

    printf("\nExample usage\n");
    printf("  Rx All    :   ./prcsJ2735 --op=rx --psid=32\n");
    printf("  Tx MapData:   ./prcsJ2735 --op=tx --msg=mapdata\n");
    printf("  Tx SPaT   :   ./prcsJ2735 --op=tx --msg=spat\n");
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
	bool	setOP    = false;
	bool	setMSG   = false;

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
            if(!memcmp(optarg, "rx", 2)) {
                g_mib.op	=	opType_rx;
                setOP	=	true;
            } else if(!memcmp(optarg, "tx", 2)) {
                setOP	=	true;
            } else {
                printf("Invalid operation - %s\n", optarg);
                return	-1;
            }
            break;
        case '2':
            if(!memcmp(optarg, "server", 6)) {
                g_mib.sockType	=	udpServer;
                printf("Set server\n");
            } else if(!memcmp(optarg, "client", 6)) {
                g_mib.sockType	=	udpClient;
            } else {
                printf("Invalid sock type - %s\n", optarg);
                return	-1;
            }
            break;
        case '3':
            if(!memcmp(optarg, "spat", 4)) {
                g_mib.op	=	opType_tx_SPaT;
                setMSG = true;
            } else if(!memcmp(optarg, "mapdata", 7)) {
                g_mib.op	=	opType_tx_MapData;
                setMSG = true;
            } else if(!memcmp(optarg, "bsm", 3)) {
                g_mib.op	=	opType_tx_BSM;
                setMSG = true;
            } else if(!memcmp(optarg, "pvd", 3)) {
                g_mib.op	=	opType_tx_PVD;
                setMSG = true;
            } else if(!memcmp(optarg, "tim", 3)) {
                g_mib.op	=	opType_tx_TIM;
                setMSG = true;
            } else if(!memcmp(optarg, "rsa", 3)) {
                g_mib.op	=	opType_tx_RSA;
                setMSG = true;
            } else if(!memcmp(optarg, "rtcm", 4)) {
                g_mib.op	=	opType_tx_RTCM;
                setMSG = true;
            } else {
                printf("Invalid message - %s\n", optarg);
                return	-1;
            }
            break;
        case '4':
            g_mib.interval	=   (uint32_t)strtoul(optarg, NULL, 10);
            break;
        case '5':
            g_mib.gpsdPort	=  (char *)calloc(1, 5); 
            strcpy(g_mib.gpsdPort, optarg);
            break;
        case '6':
            g_mib.dbg	=   (uint32_t)strtoul(optarg, NULL, 10);
            break;
        case '7':
            usage();
                return 2;
            break;
        case '8':
            g_mib.destPort	=  (uint32_t)strtoul(optarg, NULL, 10);
            break;
        case '9':
            memcpy(g_mib.destIP, optarg, strlen(optarg) < ADDRSIZE ? strlen(optarg) : ADDRSIZE );
            break;
        default:
            break;
        }
    }
    /*----------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------*/
	/* 각 동작 별 필수 파라미터 중 하나라도 입력이 되지 않았으면 실패 */
	/*----------------------------------------------------------------------------------*/
	if( !setOP ) 
    {
		printf("[prcsJ2735] Insufficient op parameters \n");
        usage();
		return -1;
	} 
    else
    {
        if( g_mib.op != opType_rx && !setMSG )
        {
            printf("[prcsJ2735] Insufficient msg parameters \n");
            usage();
            return -1;
        }
    }

    /*----------------------------------------------------------------------------------*/

	return 0;
}


/****************************************************************************************

	PrintOptions()
		저장된 옵션 파라미터들을 출력한다.
		동작 유형에 따라 출력한다.

	arguments

	return

****************************************************************************************/
void PrintOptions(void)
{
    printf("Parameter \n\n");
    if(g_mib.op == opType_rx) 
        printf("op  : RX\n");
    else
    {
        printf("op  : TX\n");

        printf("msg : \n");
        switch(g_mib.op)
        {
            case opType_tx_MapData :
                printf("      - MapData\n");
                break;
            case opType_tx_SPaT :
                printf("      - SPaT\n");
                break;
            case opType_tx_BSM :
                printf("      - BSM\n");
                break;
            case opType_tx_PVD :
                printf("      - PVD\n");
                break;
            case opType_tx_RSA :
                printf("      - RSA\n");
                break;
            case opType_tx_TIM :
                printf("      - TIM\n");
                break;
            case opType_tx_RTCM :
                printf("      - RTCM\n");
                break;
        }
    }
    printf("dbg        : 0x%x\n", g_mib.dbg);
}
