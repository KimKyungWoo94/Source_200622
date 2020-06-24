/****************************************************************************************
[getopt()]
문자열 파싱함수  
- or -- 구분
1. argc, argv 파라미터 그대로 전달
2. optstring : 파싱해야할 파라미터, 별도 파라미터 받는 경우 : 사용

optarg : 옵션 뒤에 별도의 파라미터 값이 오는 경우, 이를 파싱한 결과 파라미터 값은 optarg에 문자열로 저장

optind : 다음번 처리될 옵션의 인덱스 /
파싱한 옵션이후에 추가적인 파라미터를 받는다면 (예를 들어 입력 파일 이름 같이) 이 값을 활용 /
getopt()함수는 한 번 호출될 때마다 이 값을 업데이트

opterr : 옵션에 문제가 있을 때, 이 값은 0이 아닌 값이되며, getopt()함수가 메시지를 표시

optopt : 알 수 없는 옵션을 만났을 때 getopt의 리턴값은 ? 임
****************************************************************************************/


#include <PAR.h>
#include <getopt.h>


/****************************************************************************************
  전역변수

 ****************************************************************************************/
//static const char *optStr = "a:t:c:r:l:L:n:b:h";
static const char *optStr = "a:t:c:r:l:L:i:b:h";
/****************************************************************************************
  함수원형(지역/전역)

 ****************************************************************************************/



/****************************************************************************************
  usage()
  사용 방법 출력
****************************************************************************************/

static void usage(char *cmd) {
	printf("Usage: %s [OPTIONS]\n\n", cmd);

	printf("\nMANDATORY: \n");
	printf("  -a <action>            set Action\n");
	printf("                           rx    : receive only\n");
	printf("                           tx    : transmit only\n");
	//printf("  -n <RSU Amount>        set Amount\n");
	//printf("                         RX Only\n");


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
	//printf("  -n <RSU Amount>                  <Only RX>\n");
	printf("  -i <Information>                 Indicate Information\n");
	printf("  -b                     activate debug message output\n");
	printf("  -h                     Print usage\n");

	printf("\nExample usage\n");
	//printf("  Rx         : %s -a rx -t 1000000 -c 10  -n 3 -b 1\n", cmd);
	printf("  Rx         : %s -a rx -t 1000000 -c 10 -b 1\n", cmd);
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
	//bool rsuNumSpecified = false;
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
			case 'i' :
				g_mib.Information = strtoul(optarg, NULL,10);
				break;
			/*
			case 'n' :
				g_mib.rsuNum = strtoul(optarg, NULL, 10);
				rsuNumSpecified = true;
				break;
				*/
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
	/*
	if(g_mib.op == opRX  && !rsuNumSpecified)
	{
		printf("No rsuNumSpecified\n");
		usage(argv[0]);
		return -1;
	} */
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
		//printf("RSU Amout : %d\n",g_mib.rsuNum);
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
