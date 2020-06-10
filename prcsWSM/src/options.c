#include <v2x-obu.h>
#include <getopt.h>


/****************************************************************************************
	전역변수

****************************************************************************************/
static const char	*optStr	=	"a:x:n:k:p:r:w:o:b:h";


/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/

static void usage(char *cmd) {
  printf("Usage: %s [OPTIONS]\n\n", cmd);

  printf("\nMANDATORY: \n");
  printf("  -a <action>            set Action\n");
  printf("                           rx    : receive only\n");
  printf("                           tx    : transmit only\n");
  printf("                           trx   : transmit and receive\n");
  printf("  -p <psid>              set psid \n");
  printf("                           IMPORTANT!!: You MUST choose PSID\n");

  printf("\nOPTIONS: \n");
  printf("  -a <action>            set Action\n");
  printf("                           rx    : receive only\n");
  printf("                           tx    : transmit only\n");
  printf("                           trx   : transmit and receive\n");
  printf("  -x <netifindex>        set network interface index(for tx)\n");
  printf("                           if not specified, set to 0\n");
  printf("  -n <channel>           set channel number(for tx)\n");
  printf("                           if not specified, set to CCH\n");
  printf("  -k <timeSlot>          set time slot\n");
  printf("                           0 : time slot 0\n");
  printf("                           1 : time slot 1\n");
  printf("                           2 : continuce\n");
  printf("                           if not specified, set to continuce\n");
  printf("  -r <datarate>          set tx datarate(in 500kbps)(for tx)\n");
  printf("                           if not specified, set to 6Mbps\n");
  printf("  -w <power>             set tx power(in dBm)(for tx)\n");
  printf("                           if not specified, set to 20dBm\n");
  printf("  -o <priority>          set tx priority(for tx)\n");
  printf("                           if not specified, set to 7\n");
  printf("  -b                     activate debug message output\n");
  printf("  -h                     Print usage\n");

  printf("\nExample usage\n");
  printf("  Rx         : %s -a rx -p 20\n", cmd);
  printf("  Tx         : %s -a tx -p 20\n", cmd);
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
	int32_t opt;
	bool actionSpecified = false;
	bool psidSpecified = false;

	/*----------------------------------------------------------------------------------*/
	/* 파라미터 파싱 및 저장 */
	/*----------------------------------------------------------------------------------*/
	while((opt = getopt(argc, argv, optStr)) != -1) {

		switch(opt) {
		case 'a':
			if(!strncmp(optarg, "rx", 2))
				g_mib.op	=	opRX;
			else if(!strncmp(optarg, "tx", 2))
				g_mib.op	=	opTX;
			else if(!strncmp(optarg, "trx", 3))
				g_mib.op	=	opTRX;
			else {
				printf("Invalid action - %s\n", optarg);
				return	-1;
			}
			actionSpecified	=	true;
			break;

		case 'x':
			g_mib.netIfIndex	=	(uint32_t)strtol(optarg, NULL, 10);
			break;

		case 'n':
			g_mib.channel=	(Dot3ChannelNumber)strtol(optarg, NULL, 10);
			break;

		case 'k':
			g_mib.timeSlot =	(Dot3TimeSlot)strtoul(optarg, NULL, 10);
			break;

		case 'p':
			g_mib.psid		=	(Dot3Psid)strtoul(optarg, NULL, 10);
			psidSpecified = true;
			break;

		case 'r':
			g_mib.dataRate	=	(Dot3DataRate)strtol(optarg, NULL, 10);
			break;

		case 'w':
			g_mib.power		=	(Dot3Power)strtol(optarg, NULL, 10);
			break;

		case 'o':
			g_mib.priority		=	(Dot3Priority)strtol(optarg, NULL, 10);
			break;

		case 'b':
			g_dbg = (DbgMsgLevel)strtoul(optarg, NULL, 10);
			break;

        case 'h' :
            usage(argv[0]);
            return 0;

		default:
			break;
		}
	}
	/*----------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------*/
	/* 각 Action 별 필수 파라미터 중 하나라도 입력이 되지 않았으면 실패 */
	/*----------------------------------------------------------------------------------*/
	if(!actionSpecified && !psidSpecified) {
		printf("No action or psid specified\n");
        usage(argv[0]);
		return -1;
	}

	return 0;
}

