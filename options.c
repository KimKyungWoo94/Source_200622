#include <PAR.h>
#include <getopt.h>


/****************************************************************************************
  전역변수

 ****************************************************************************************/
//static const char	*optStr	=	"a:x:n:k:p:r:w:o:b:h";
static const char *optStr = "a:t:c:r:l:L:n:b:h";

/****************************************************************************************
  함수원형(지역/전역)

 ****************************************************************************************/

static void usage(char *cmd) {
	printf("Usage: %s [OPTIONS]\n\n", cmd);

	printf("\nMANDATORY: \n");
	printf("  -a <action>            set Action\n");
	printf("                           rx    : receive only\n");
	printf("                           tx    : transmit only\n");

	printf("\nOPTIONS: \n");
	printf("  -a <action>            set Action\n");
	printf("                           rx    : receive only\n");
	printf("                           tx    : transmit only\n");
	printf("  -t <Interval>   <TX : usec>      if not set, Interval : 10000usec\n");
	printf("                  <RX : usec>      if not set, Interval : 1000000usec\n");
	printf("  -c <Cycle>      <Only RX : msec> if not set, Cycle : 10msec\n");
	printf("  -r <RSUID>                       indicate RSUID\n");
	printf("  -l <Latitude> 	   	   indicate Latitude\n");
	printf("  -L <Longitude>                   indicate Longitude\n");
	printf("  -n <RSU Amount>                  <Only RX>\n");
	printf("  -b                     activate debug message output\n");
	printf("  -h                     Print usage\n");

	printf("\nExample usage\n");
	printf("  Rx         : %s -a rx -n 3 -b 1\n", cmd);
	printf("  Tx         : %s -a tx -t 10000 -r 1 -b 1\n", cmd);
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

	/*----------------------------------------------------------------------------------*/
	/* 파라미터 파싱 및 저장 */
	/*----------------------------------------------------------------------------------*/
	while((opt = getopt(argc, argv, optStr)) != -1) {

		switch(opt) {
			case 'a':
				if(!strncmp(optarg, "rx", 2))
					g_mib.op = opRX;
				else if(!strncmp(optarg, "tx", 2))
					g_mib.op = opTX;
				else {
					printf("Invalid action - %s\n", optarg);
					return	-1;
				}
				actionSpecified	= true;
				break;

			case 't' :
				g_mib.interval = (uint32_t)strtoul(optarg, NULL, 10);
				break;
			case 'c' :
				g_mib.cycle = (uint32_t)strtoul(optarg, NULL, 10);

			case 'r' :
				g_mib.rsuID = strtoul(optarg, NULL, 10);
				break;

			case 'l' :
				g_mib.Latitude = strtoul(optarg, NULL, 10);
				break;

			case 'L' :
				g_mib.Longitude = strtoul(optarg, NULL, 10);
				break;
			case 'n' :
				g_mib.rsuNum = strtoul(optarg, NULL, 10);
				break;

			case 'b':
				g_mib.dbg = (uint32_t)strtoul(optarg, NULL, 10);
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
	if(!actionSpecified) {
		printf("No action specified\n");
		usage(argv[0]);
		return -1;
	}

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
	if(g_mib.op == opRX){
		printf("op  : RX\n");
		
		if(g_mib.interval !=0)
		{
		printf("Timer Interval : %dusec\n", g_mib.interval);
		}

		if(g_mib.cycle !=0)
		{
		printf("Cycle : %dmsec\n",g_mib.cycle);
		}
		printf("RSU Amout : %d\n",g_mib.rsuNum);
		printf("dbg : %d\n",g_mib.dbg);
	}
	else
	{
		printf("op  : TX\n");
		printf("RSUID : %d\n", g_mib.rsuID);
		if(g_mib.interval != 0)
		{
		printf("Interval : %dmsec\n", g_mib.interval);
		}
		printf("dbg  : %d\n", g_mib.dbg);
	}
}
