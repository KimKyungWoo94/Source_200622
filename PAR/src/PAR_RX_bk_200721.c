#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "msgQ.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <malloc.h>
#include <PAR.h>



/****************************************************************************************
  함수원형
 ****************************************************************************************/
int par_InitRXoperation();
void par_RXoperation();
void par_Report(void);
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola);
static void* rxThread(void *notused);
static void* userSelectThread(void *notused);
void createNode();
void freeAllNode();
struct parInfo_t* getNode(int index);
void getCalDataRcpi(uint8_t* array, int arrayIdx, int mode);
void getCalDataRxpower(int16_t* array, int arrayIdx, int mode);
bool isThereRSUID(int rsuID);
/**
 * par_InitRXoperation() 
 * PAR 수신동작을 초기화한다.
 * 수신 타이머 관련 뮤텍스, 컨디션시그널 초기화
 * 타이머 생성 및 RX 쓰레드 생성
 * GPSD OPEN 
 * @return   성공 시 0, 실패 시 -1
 */
int par_InitRXoperation(){

	int32_t ret;
	int status;
	/* 링크드리스트 구조체 동적할당 */
	if (ListPtr == NULL) {
		ListPtr = (linkedList*)malloc(sizeof(linkedList));
		ListPtr->cur = NULL;
		ListPtr->head = NULL;
		ListPtr->tail = NULL;
		ListPtr->numOfList = 0;
	}

	/* 구조체 동적할당 */
	//stPARInfo = malloc(sizeof(struct parInfo_t));

	//stPARInfo = malloc(sizeof(struct parInfo_t) * g_mib.rsuNum);
	//int size = sizeof(struct PAR_Info_t);
	//size_t msizeNUM = malloc_usable_size(stPARInfo);
	//printf("size : %d, msize : %d\n",size, msizeNUM);

	//if(stPARInfo ==NULL)
	//{
	//	syslog(LOG_ERR | LOG_LOCAL3, "[PAR_RX] Fail malloc()\n");
	//}

	if(g_mib.interval == 0)
	{
		g_mib.interval = 1000000; /* 타이머주기 usec -> 1초  1000000usec : 1000msec : 1초에 1번 1000msec은 1초 */
	}

	if(g_mib.cycle == 0)
	{
		g_mib.cycle = 10;  /* 10msec 수신 주기 */
	}

	/* 수신 타이머 관련 뮤텍스, 컨디션시그널 초기화*/
	pthread_mutex_init(&g_mib.txMtx, NULL);
	pthread_cond_init(&g_mib.txCond, NULL);


	/* 타이머 생성 */
	InitTxTimer(g_mib.interval); //1초

	/* RX쓰레드 생성 */
	ret = pthread_create(&rx_thread, NULL, rxThread, NULL);
	if(ret <0){
		//perror("[PAR] Fail to create RX thread() ");
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] Fail to create RX thread()\n");
		return -1;
	}
	else{
		//printf("Success RX thread() \n");
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Success create RX thread()\n");
	}

	/* User Select 쓰레드 생성 */
	if(g_mib.Information != 0){
		ret = pthread_create(&userSelect_thread, NULL, userSelectThread, NULL);
		if(ret < 0){
			syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] Fail to create userSelect thread()\n");
		}
		else{
			syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Success create userSelect thread()\n");

		}
	}


	/* GPSD 오픈 */
	ret = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);
	if(ret <0){
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] gps_open() fail(%s)\n", gps_errstr(ret));
		shmCheck = true;
	}
	else{
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Success gps_open()\n");
	}


	return 0;
}

/**
 * par_RXoperation() 
 * PAR 수신동작을 수행한다.
 * GPSD 읽기
 * MQ receive
 * Packet 구조체에 정보 복사
 * stPARInfo 구조체에 정보 저장
 */
void par_RXoperation(){

	int32_t ret;
	uint32_t len;
	uint8_t outBuf[BUFSIZE];
	//int status;
	void* status;
	memset(outBuf, 0, BUFSIZE);

	/* LinkedList 구조체 동적할당 */
	//L = (linkedList *)malloc(sizeof(linkedList));
	//L -> cur = NULL;
	//L -> head = NULL;
	//L -> tail = NULL;

	while(!ending){

		/* Connection Check */
		if(shmCheck == true)
		{
			gps_close(&gpsData);
			syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Re connection to GPSD\n");
			ret = gps_open(GPSD_SHARED_MEMORY, 0, &gpsData);

			if(ret < 0)
			{
				syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] gps_open() fail(%s)\n", gps_errstr(ret));
			}
			shmCheck = false;

		}

		/* GPS Read */
		ret = gps_read(&gpsData);
		if(ret < 0){
			//printf("[PAR_RX] gps_read() fail( %s)\n", gps_errstr(ret));
			syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] gps_read() fail( %s)\n", gps_errstr(ret));
			shmCheck = true;
		}

		//printf("g_mib_Lati : %u  g_mib_ Longi : %u \n",g_mib.Latitude, g_mib.Longitude);

		/* 차량 위도 경도 인자값으로 받음 */
		if( g_mib.Latitude != 0 && g_mib.Longitude != 0)
		{
			g_obu.obuLatitude = g_mib.Latitude;
			g_obu.obuLongitude = g_mib.Longitude;
			//printf("OBULati : %d   OBULongi : %d \n",g_obu.obuLatitude, g_obu.obuLongitude);
			syslog(LOG_INFO | LOG_LOCAL4,"Success INPUT LATI and LONGI\n");
#if 0 
			if(g_mib.dbg)
			{
				syslog(LOG_INFO | LOG_LOCAL2,"OBULati : %d   OBULongi : %d \n",g_obu.obuLatitude, g_obu.obuLongitude);
			}
#endif
		}

		else
		{
			/* gpsd로 부터 받음 */
			if(gpsData.set)
			{
				//printf("[PAR_RX] GPSData.set Success\n");
				//syslog(LOG_INFO | LOG_LOCAL2,"[PAR_RX] GPSData.set Success\n");
				g_obu.obuLatitude = (int) pow(10,7)*gpsData.fix.latitude;
				g_obu.obuLongitude = (int) pow(10,7)*gpsData.fix.longitude;
				g_obu.obuSpeed = gpsData.fix.speed*3.6;
				g_obu.obuHeading = gpsData.fix.track;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.obuLatitude, g_obu.obuLongitude, g_obu.obuSpeed, g_obu.obuHeading);
#if 0 
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL2,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.obuLatitude, g_obu.obuLongitude, g_obu.obuSpeed, g_obu.obuHeading);
				}
#endif
			}

			else {
				/* 인자 값과 gpsd로부터 받지 않음 */
				//printf("GPS Invalid\n");
				syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] GPS Invalid\n");
				g_obu.obuLatitude = 900000001;
				g_obu.obuLongitude = 1800000001;
				g_obu.obuSpeed = 8191;
				g_obu.obuHeading = 28800;
				//printf("OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n",g_obu.obuLatitude, g_obu.obuLongitude, g_obu.obuSpeed, g_obu.obuHeading);
#if 0 
				if(g_mib.dbg)
				{
					syslog(LOG_INFO | LOG_LOCAL4,"OBULatitude : %d  OBULongitude : %d OBUSpeed : %f OBUHeading : %f \n", g_obu.obuLatitude, g_obu.obuLongitude, g_obu.obuSpeed, g_obu.obuHeading);
				}
#endif				
			}

		}

		/* 기지국 정보 수신 */
		len = recvMQ(outBuf);
		if(len<0)
			continue;

		/* 통신성능 측정프로그램에 필요한 정보 저장 */
		else if(len >0 && !ending)
		{
			memcpy(&g_Packet,outBuf,len);
			//if(g_Packet.rsuID >0 && g_Packet.rsuID <= g_mib.rsuNum)
			//{
#if 1
			/* CreateNode */
			if(!isThereRSUID(g_Packet.rsuID))
				createNode();
			else
				/* getNode */
				ListPtr->cur = getNode(g_Packet.rsuID);

			/* arrayIdx 초기화 */
			if(ListPtr->cur->arrayIdx == 10)
				ListPtr->cur->arrayIdx = 0;

			ListPtr->cur->check =1;
			ListPtr->cur->rsuID = g_Packet.rsuID;
			ListPtr->cur->rsuLongitude = g_Packet.rsuLongitude;
			ListPtr->cur->rsuLatitude = g_Packet.rsuLatitude;
			ListPtr->cur->obuSpeed = g_obu.obuSpeed;
			ListPtr->cur->obuHeading = g_obu.obuHeading;
			ListPtr->cur->interval = g_mib.cycle;
			ListPtr->cur->rxpower[ListPtr->cur->arrayIdx] = g_Packet.rxPower;
			ListPtr->cur->rcpi[ListPtr->cur->arrayIdx] = g_Packet.rcpi;
			//ListPtr->cur->rcpi2 = g_Packet.rcpi;
			//ListPtr->cur->rxpower2 = g_Packet.rxPower;
			ListPtr->cur->obuLongitude = g_obu.obuLongitude;
			ListPtr->cur->obuLatitude = g_obu.obuLatitude;
			ListPtr->cur->cnt++;
			ListPtr->cur->pastArrayidx = ListPtr->cur->arrayIdx;
			ListPtr->cur->arrayIdx++;
#else
			stPARInfo[g_Packet.rsuID].check =1;
			stPARInfo[g_Packet.rsuID].rsuID = g_Packet.rsuID;
			stPARInfo[g_Packet.rsuID].rsuLongitude = g_Packet.rsuLongitude;
			stPARInfo[g_Packet.rsuID].rsuLatitude = g_Packet.rsuLatitude;
			stPARInfo[g_Packet.rsuID].obuSpeed = g_obu.obuSpeed;
			stPARInfo[g_Packet.rsuID].obuHeading = g_obu.obuHeading;
			stPARInfo[g_Packet.rsuID].interval = g_mib.cycle;
			stPARInfo[g_Packet.rsuID].rxpower = g_Packet.rxPower;
			stPARInfo[g_Packet.rsuID].rcpi = g_Packet.rcpi;
			stPARInfo[g_Packet.rsuID].obuLongitude = g_obu.obuLongitude;
			stPARInfo[g_Packet.rsuID].obuLatitude = g_obu.obuLatitude;
			stPARInfo[g_Packet.rsuID].cnt++;
#endif
			//printf("stPARInfo[CNT] : %d \n\n\n",stPARInfo[g_Packet.rsuID].cnt);
#if 0 
			if(g_mib.dbg)
			{
				syslog(LOG_INFO | LOG_LOCAL4,"stPARInfo[CNT] : %d \n\n\n",stPARInfo[g_Packet.rsuID].cnt);
			}
#endif
			//	}

		}

	}

	/*동적할당 해제*/
	// free(stPARInfo);


	/* RX쓰레드 종료 */
	//ret = pthread_join(rx_thread, (void **)status);
	ret = pthread_join(rx_thread, &status);
	if( ret == 0 )
	{
		//printf("[PAR_RX] Completed join with rxThread status = %s\n", (char*)status);
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Completed join with rxThread status = %s\n", (char*)status);
	}
	else
	{
		//printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
		syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
	}

	/* userSelect쓰레드 종료 */
	if(g_mib.Information != 0){
		//ret = pthread_join(userSelect_thread, (void **)status);
		ret = pthread_join(userSelect_thread, &status);
		if( ret == 0 )
		{
			//printf("[PAR_RX] Completed join with userSelectThread status = %s\n", (char*)status);
			syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Completed join with userSelectThread status = %s\n", (char*)status);
		}
		else
		{
			//printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
			syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
		}	

	}
	/* gpsd close */
	gps_close(&gpsData);


	/* 뮤텍스 및 컨디션시그널 해제 */
	pthread_mutex_destroy(&g_mib.txMtx);
	pthread_cond_destroy(&g_mib.txCond);

	/* 동적할당 해제 */
	freeAllNode();
}

/**
 * par_Report()
 * 해당 각 기지국에 대하여 거리계산
 * 및 
 * PAR 계산을 수행하고
 * 해당 각 기지국에 대한 정보들 출력
 */
void par_Report(void){

	int idx = 0;
	//uint32_t cnt[g_mib.rsuNum];
	uint32_t cnt[ListPtr->numOfList];
	//uint32_t maxPAR = 0;
	//uint32_t curPAR = 0;
	//printf("[PAR_INFO] Running Timer \n");
	//syslog(LOG_INFO | LOG_LOCAL4,"[PAR_INFO] Running Timer \n");


	struct parInfo_t *ptrTemp = ListPtr->head;
	/* RSU 갯수 만큼 반복 */
	//for(idx=0;idx<=g_mib.rsuNum;idx++)
	for(idx=1;idx<=ListPtr->numOfList;idx++)
	{
		//syslog(LOG_INFO | LOG_LOCAL4, "Report function Test : %d\n",idx);
		if(ptrTemp == NULL)
			break;

		//패킷 카운트버퍼 초기화
		cnt[idx]=0;
		//curPAR, maxPAR 초기화
		//stPARInfo[idx].curPAR = 0;
		//stPARInfo[idx].maxPAR = 0;

		ptrTemp->curPAR = 0;
		ptrTemp->maxPAR = 0;


		/* RSUID 일치 여부 */
		//if(ptrTemp->rsuID >0 && ptrTemp->rsuID <=RSU_SLOT)
		//{
		//printf("RSUID : %d RSUNUM : %d 일치여부\n",ptrTemp->rsuID, g_mib.rsuNum);
		//printf("--------------------------------------------------------------------------------------------------\n");

		//기존 들어오던 기지국 정보가 수신되지 않기 시작함
		if(/*ptrTemp->check >0 &&*/ ptrTemp->cnt==0)
		{
			ptrTemp->check = false;

		}
		else
			ptrTemp->check = true;

		/*
		// 5초 동안 연속적으로 수신되지 않음
		if(ptrTemp->check > MAX_ZERO_COUNT && ptrTemp->cnt==0)
		{
			// 다음 수신 정보가 있을때까지 통신성능측정프로그램으로 정보전달하지 않음.
			ptrTemp->check=0;
		}
		*/

		// 체크되어 있지 않은 기지국의 정보는 통신성능측정프로그램으로 정보전달하지 않음.
		if(!ptrTemp->check)
		{
//			ptrTemp->arrayIdx =0;
//			ptrTemp = ptrTemp->next;
//			continue;
			setZeroParInfo(ptrTemp);
		}
		else
		{
			/* 거리 계산 */
			ptrTemp->distance = ldCaldistance(ptrTemp->rsuLongitude, ptrTemp->rsuLatitude, ptrTemp->obuLongitude, ptrTemp->obuLatitude);
	
			/* 해당 기지국 받은 CNT 갯수 저장 */
			cnt[idx] = ptrTemp->cnt;

			/* 해당 기지국 CNT 초기화 */
			ptrTemp->cnt = 0;

		// 현재 측정지점에서 최대 PAR 갱신
		//curPAR = (cnt[idx]*100)/(1000/stPARInfo[idx].interval);
		//if( curPAR > maxPAR )
		//	maxPAR = curPAR;

#if 1 

			/* 현재 PAR 계산 */
			ptrTemp->curPAR = (cnt[idx]*100)/(1000/ptrTemp->interval);
	
			/* PAR최대값 계산 */
			if(ptrTemp->curPAR > ptrTemp->maxPAR)
				ptrTemp->maxPAR = ptrTemp->curPAR;
	
		}

		/* dbg모드 */
		if(g_mib.dbg){
			//printf("RSU ID : %d  RSUNUM : %d dbg모드\n",ptrTemp->rsuID,g_mib.rsuNum);
			//printf("--------------------------------------------------------------------------------------------------\n");
			syslog(LOG_INFO | LOG_LOCAL4, "RSUID : %d, RSULatitude : %d, RSULongitude : %d, OBULatitude : %d, OBULongitude : %d\n", 
//					ptrTemp->check,
					ptrTemp->rsuID,
					ptrTemp->rsuLatitude,
					ptrTemp->rsuLongitude,
					ptrTemp->obuLatitude,
					ptrTemp->obuLongitude);

			syslog(LOG_INFO | LOG_LOCAL4, /*"RXPOWER : %d, rcpi : %d,*/"Distance : %.0f, OBUSpeed : %3.2f, OBUHeading : %3.2f, CNT : %u, PAR : %d\n",
					//ptrTemp->rxpower2,
					//ptrTemp->rcpi2,
					ptrTemp->distance,
					(double)ptrTemp->obuSpeed,
					(double)ptrTemp->obuHeading,
					cnt[idx],
					ptrTemp->maxPAR);
			

				if(ptrTemp->check && ptrTemp->arrayIdx == 0)
				{
					getCalDataRcpi(ptrTemp->rcpi, ptrTemp->pastArrayidx,0);
					getCalDataRxpower(ptrTemp->rxpower, ptrTemp->pastArrayidx,0);
				}
				else{
					getCalDataRcpi(ptrTemp->rcpi, ptrTemp->arrayIdx,0);
					getCalDataRxpower(ptrTemp->rxpower, ptrTemp->arrayIdx,0);
				}
			//getCalDataRcpi(ptrTemp->rcpi, ptrTemp->arrayIdx,0);
			//getCalDataRxpower(ptrTemp->rxpower, ptrTemp->arrayIdx,0);


			syslog(LOG_INFO | LOG_LOCAL4, "--------------------------------------------------------------------------------------------------\n");
		}
#else
		printf("CHECK : %u, RSUID : %d, RSU Latitude : %d, RSU Longitude : %d, OBU Latitude : %d, OBU Longitude : %d,  RXPOWER : %d, rcpi : %d, distance : %.0f, OBUSpeed : %3.2f, OBUHeading : %3.2f, CNT : %u, PAR : %d\n",
				stPARInfo[idx].check,
				stPARInfo[idx].rsuID,
				stPARInfo[idx].rsuLatitude,
				stPARInfo[idx].rsuLongitude,
				stPARInfo[idx].obuLatitude,
				stPARInfo[idx].obuLongitude,
				stPARInfo[idx].rxpower,
				stPARInfo[idx].rcpi,
				stPARInfo[idx].distance,
				(double)stPARInfo[idx].obuSpeed,
				(double)stPARInfo[idx].obuHeading,
				cnt[idx],
				stPARInfo[idx].maxPAR);
#endif
		//if(ptrTemp->arrayIdx !=0)
		//ptrTemp->pastArrayidx = ptrTemp->arrayIdx;
		ptrTemp->arrayIdx =0;
		//ptrTemp->check = true;
		ptrTemp = ptrTemp->next;
		//}

	}
}

void setZeroParInfo(struct parInfo_t* ptr){
	ptr->arrayIdx = 0;
	ptr->rsuLatitude = 0;
	ptr->rsuLongitude = 0;
	ptr->obuLatitude = 0 ;
	ptr->obuLongitude = 0;
	ptr->distance = 0;
	ptr->obuSpeed = 0.0;
	ptr->obuHeading = 0.0;
	ptr->maxPAR = 0;
}

/**
 * createNode()
 * 노드 생성
 */
void createNode() {
	//printf("[PAR_RX] createNode\n");
	syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] CreateNode\n");

	struct parInfo_t* stPARInfoPtr =(struct parInfo_t *) malloc(sizeof(struct parInfo_t));
	stPARInfoPtr->next = NULL;

	/* 현재 집어 넣는 노드가 첫 노드일때 */
	if(ListPtr->head == NULL && ListPtr->tail == NULL)

		/*head와 tail을 방금 할당한 노드의 주소 값으로 넣어준다 */
		ListPtr->head = ListPtr->tail = stPARInfoPtr;	

	/* 첫 노드가 아닐때 */
	else {

		/*지금 끝이라고 정의되어있는 노드(tail)의 next에 할당한 노드의 주소 값을 넣어주고 그 끝이라고 정의된 노드에 방금 새롭게 할당한 노드의 주소 값을 넣어주는 것 */
		ListPtr->tail->next = stPARInfoPtr;
		ListPtr->tail = stPARInfoPtr; //ListPtr->tail = tail->next;
	}
	ListPtr->cur = ListPtr->tail;
	ListPtr->tail->cnt = 0;
	ListPtr->tail->arrayIdx = 0;
	ListPtr->numOfList++;
}

/**
 * freeAllNode()
 * 동적할당 해제
 */
void freeAllNode(){

	struct parInfo_t* tempPtr = ListPtr->head; //tempPtr = a
	
	while(ListPtr->head != NULL){
	//	struct parInfo_t* tempPtr = ListPtr->head; //tempPtr = a
		ListPtr->head = ListPtr->head->next; // header = b;
		free(tempPtr); //a free
		tempPtr = ListPtr->head; //b
		syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Free Node\n");
	}
	ListPtr->cur = NULL;
	ListPtr->tail = NULL;
	ListPtr->numOfList = 0;
	
	syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] There is nothing to free Node \n");
}

/**
 * getNode()
 * 링크드리스트의 idx번째 아이템을 가져오는 함수
 */
struct parInfo_t* getNode(int rsuID){
	struct parInfo_t* returnPtr = ListPtr->head;

	while(returnPtr != NULL){
		if(rsuID == returnPtr->rsuID)
			break;
		returnPtr = returnPtr->next;
	}
	return returnPtr;
}


/**
 * getCalDataRcpi()
 * RCPI 값 MIN, MAX, avg, 마지막 값 출력 
 */
void getCalDataRcpi(uint8_t* array, int arrayIdx, int mode){
	if(arrayIdx > 0){
		uint16_t sum = array[0];
		uint8_t min = array[0];
		uint8_t max = array[0];

		//syslog(LOG_INFO | LOG_LOCAL4, "rcpi[0]: %d", array[0]);

		for(int i=1; i<arrayIdx; i++){

			sum += array[i];

			if(array[i] < min)
				min = array[i];

			if(array[i] > max)
				max = array[i];

			//syslog(LOG_INFO | LOG_LOCAL4, "rcpi[%d]: %d", i, array[i]);
		}

		if(mode == 0)
			syslog(LOG_INFO | LOG_LOCAL4, "Max_rcpi : %d, Min_rcpi : %d, Avr_rcpi : %.1f, Last_rcpi : %d\n",
					max, min, (double)(sum/arrayIdx), array[arrayIdx-1]);
		else
			printf("Max_rcpi : %d, Min_rcpi : %d, Avr_rcpi : %.1f, Last_rcpi : %d\n",
					max, min, (double)(sum/arrayIdx), array[arrayIdx-1]);
	}
	else{
		 if(mode == 0)
			 syslog(LOG_INFO | LOG_LOCAL4, "Max_rcpi : 0, Min_rcpi : 0, Avr_rcpi : 0.0, Last_rcpi : 0\n");
		 else
			 printf("Max_rcpi : 0, Min_rcpi : 0, Avr_rcpi : 0.0, Last_rcpi : 0\n");
	}
}

/**
 * getCalDataRxpower()
 * RXpower 값 MIN, MAX, avg, 마지막 값 출력 
 */
void getCalDataRxpower(int16_t* array, int arrayIdx, int mode){
	if(arrayIdx > 0){
		int16_t sum = array[0];
		int16_t min = array[0];
		int16_t max = array[0];

		//syslog(LOG_INFO | LOG_LOCAL4, "rxpower[0]: %d", array[0]);

		for(int i=1; i<arrayIdx; i++){
			sum += array[i];

			if(array[i] < min)
				min = array[i];

			if(array[i] > max)
				max = array[i];

			//syslog(LOG_INFO | LOG_LOCAL4, "rxpower[%d]: %d", i, array[i]);
		}

		if(mode==0)
			syslog(LOG_INFO | LOG_LOCAL4, "Max_rxpower : %d, Min_rxpower : %d, Avr_rxpower : %.1f, Last_rxpower : %d\n",
					max, min, (double)(sum/arrayIdx), array[arrayIdx-1]);
		else
			printf("Max_rxpower : %d, Min_rxpower : %d, Avr_rxpower : %.1f, Last_rxpower : %d\n",
					max, min, (double)(sum/arrayIdx), array[arrayIdx-1]);
	}
	else{
		if(mode == 0)
			syslog(LOG_INFO | LOG_LOCAL4, "Max_rxpower : 0, Min_rxpower : 0, Avr_rxpower : 0.0, Last_rxpower : 0\n");
		else
			printf("Max_rxpower : 0, Min_rxpower : 0, Avr_rxpower : 0.0, Last_rxpower : 0\n");
		}
}



/**
 * ldCaldistance()
 * 거리 계산 함수
 * RSU 위도, 경도 와 OBU 위도, 경도 이용하여 거리 계산
 * @return 거리 값
 */
long double ldCaldistance(uint32_t rlo, uint32_t rla, uint32_t olo, uint32_t ola)
{
	long double a, b, c, d, x, y, z;
	a = (long double)((rlo * 1e-7) * M_PI / 180);
	b = (long double)((rla * 1e-7) * M_PI / 180);
	c = (long double)((olo * 1e-7) * M_PI / 180);
	d = (long double)((ola * 1e-7) * M_PI / 180);
	x = sin(b) * sin(d);
	y = cos(b) * cos(d);
	z = fabs(c - a);
	y = y*cos(z);
	x = acos(x + y);
	y = x * 6371009;

	return y;
}

/**
 * rxThread()
 * RX Thread
 * 1초마다 타이머를 울려 PAR_SendReport()함수 호출
 */
static  void* rxThread(void *notused){


	while(!ending){
		/* 송신타이머로부터 시그널이 올때 때까지 대기한다. */
		pthread_mutex_lock(&g_mib.txMtx);
		pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
		pthread_mutex_unlock(&g_mib.txMtx);
		par_Report();
	}

	/* Thread 종료 */
	pthread_exit((void *)0);
}
/***
 * userSelectThread()
 * 유저 메뉴 쓰레드
 * 연결된 기지국 ID 와 원하는 기지국 정보 출력
 */
static void* userSelectThread(void *notused){
	int num;
	int ret;
	int status;
	struct parInfo_t *ptrTemp = NULL;
	num = 0;
	int selectRsuID;

	while(!ending){

		switch(num){
			case -1:
				/* userSelect쓰레드 종료 
				   ret = pthread_join(userSelect_thread, (void **)status);
				   if( ret == 0 )
				   {
				   printf("[PAR_RX] Completed join with userSelectThread status = %d\n", status);
				//syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Completed join with userSelectThread status = %d\n", status);
				}
				else
				{
				printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
				//syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
				}
				break;
				 */
				continue;
			case 0 :
				printf("             MENU\n");
				printf("====Please select a number====\n");
				printf("1. Printing Connected RSUID(s) Information\n");
				printf("2. Printing the RSU information you want\n");
				printf("3. QUit\n>> ");
				scanf("%d",&num);
				break;
			case 1:
				ptrTemp = ListPtr->head;
				int count = 1;
				while(ptrTemp != NULL){
					printf("RSUID of [%d]: %d\n", count++, ptrTemp->rsuID);
					ptrTemp = ptrTemp->next;
				}
				num = 0;
				break;
			case 2 :
				printf("Input wanted RSU ID >> ");
				scanf("%d",&selectRsuID);
				ptrTemp = getNode(selectRsuID);
				if(ptrTemp == NULL){
					printf("# Error(Please input correct RsuID\n");
					break;
				}
				printf("**Information of RSU ID[%d]**\n",selectRsuID); 
				printf( "RSUID : %d\nRSU Latitude : %d\nRSU Longitude : %d\nOBU Latitude : %d\nOBU Longitude : %d\n",
					//	ptrTemp->check,
						ptrTemp->rsuID,
						ptrTemp->rsuLatitude,
						ptrTemp->rsuLongitude,
						ptrTemp->obuLatitude,
						ptrTemp->obuLongitude);
				printf(/*"RXPOWER : %d\nRCPI : %d\n*/"Distance : %.0f\nOBUSpeed : %3.2f\nOBUHeading : %3.2f\nPAR : %d\n",
						//ptrTemp->rxpower2,
						//ptrTemp->rcpi2,
						ptrTemp->distance,
						(double)ptrTemp->obuSpeed,
						(double)ptrTemp->obuHeading,
						ptrTemp->maxPAR);

				if(ptrTemp->check && ptrTemp->arrayIdx == 0)
				{
					getCalDataRcpi(ptrTemp->rcpi, ptrTemp->pastArrayidx,1);
					getCalDataRxpower(ptrTemp->rxpower, ptrTemp->pastArrayidx,1);
				}
				else
				{
					getCalDataRcpi(ptrTemp->rcpi, ptrTemp->arrayIdx,1);
					getCalDataRxpower(ptrTemp->rxpower, ptrTemp->arrayIdx,1);
				}

				num=0;
				break;
			case 3 :
				/* Trhead 종료 */
				printf("Success Quit!\n");
				pthread_exit((void *)0);
				//num = -1;
				//pthread_exit((void *)0);
				break;

			default :
				printf("#error:not usable parameter\n");
				num = 0;
				break;
		}
	}

	//pthread_exit((void *)0);
#if 0
	/* userSelect쓰레드 종료 */
	ret = pthread_join(userSelect_thread, (void **)status);
	if( ret == 0 )
	{
		printf("[PAR_RX] Completed join with userSelectThread status = %d\n", status);
		//syslog(LOG_INFO | LOG_LOCAL4, "[PAR_RX] Completed join with userSelectThread status = %d\n", status);
	}
	else
	{
		printf("[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
		//syslog(LOG_ERR | LOG_LOCAL5, "[PAR_RX] ERROR: return code from pthread_join() is %d\n", ret);
	}
#endif

}

/***
 * isThereRSUID()
 * 리스트에 RSUID가 있는지 확인해주는 함수
 * 있으면 TRUE 없으면 FALSE
 */
bool isThereRSUID(int rsuID){
	struct parInfo_t *ptrTemp = ListPtr->head;
	while(ptrTemp != NULL){
		if(ptrTemp->rsuID == rsuID)
			return true;

		ptrTemp = ptrTemp->next;
	}
	return false;
}
