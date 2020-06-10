/**
* COPYRIGHT (c) 2001~2019 CARNAVICOM ALL RIGHTS RESERVED.
* All rights reserved.
*
* gpsd_To_PotiMsg.c
* Created on: 2019. 9. 03.
*      Author: jwlee
*/

/****************************************************************************************
	시스템 헤더

****************************************************************************************/


/****************************************************************************************
	프로젝트 헤더

****************************************************************************************/

#include "udp.h"

/****************************************************************************************
	상수

****************************************************************************************/

/****************************************************************************************
	매크로

****************************************************************************************/


/****************************************************************************************
	유형 정의

****************************************************************************************/

/****************************************************************************************
	열거형

****************************************************************************************/


/****************************************************************************************
	구조체

****************************************************************************************/


/****************************************************************************************
	전역변수

****************************************************************************************/

/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/
/****************************************************************************************

	ConvertToPosition()
		- NMEA 메시지에서 파싱된 경도/위도 문자열을 경/위도 변수값으로 변환
		- NMEA 메시지에는 도분 단위로 표시되어 있으며, 이를 도초단위로 변환하여 반환한다. (0.1 micorodegree unit)
		- 예: 도분단위(12706.23190) --> 도초단위 (127*10000000) + (623190/60)
		- 참고로, SAE 및 ETSI 모두 이것과 동일한 단위를 사용한다. (0.1 microdegree)


	arguments
		longitudeStr		경도 문자열
		latitudeStr			위도 문자열
		longitude			경도값이 저장될 변수
		latitude			위도값이 저장될 변수
		EW					동경/서경 여부 - 'E' or 'W'
		NS					남위/북위 여부 - 'N' or 'S'

	return

****************************************************************************************/
void ConvertToPosition(IN int32_t *getLongitude, IN int32_t *getLatitude,
		OUT int32_t *cvrtLongitude, OUT int32_t *cvrtLatitude,
		IN int32_t get_stauts)
{
	double Long;
	double Lati;
	double fraction;
	int32_t integer0, integer1, integer2;

//	log_debug("nav_pvt[lon:%d, lat:%d], real_Degree[lon:%d, lat:%d]\n", getLongitude, getLatitude
//				                    , cvrtLongitude,  cvrtLatitude);
//	log_debug("nav_pvt[lon:%d, lat:%d], real_Degree[lon:%d, lat:%d]\n", *getLongitude, *getLatitude
//				                    , *cvrtLongitude,  *cvrtLatitude);
	
	/* 경도/위도 문자열이 비어 있으면 unavailable로 반환한다. */
	if (get_stauts == STATUS_NO_FIX) {
		*cvrtLongitude = 1800000001;
		*cvrtLatitude = 900000001;
		return;
	}

	/* 문자열 형식의 경/위도를 실수로 변환 */
	Long = (double)*getLongitude  * (double)0.0000001;//1e-07;
	Lati = (double)*getLatitude  * (double)0.0000001;

//	log_debug("nav_pvt[long:%d, lat:%d], _real_Degree[long:%d, lat:%d]\n", getLongitude, getLatitude,  Long, Lati );

	/* 경도를 0.1마이크로도 단위로 변환 */
	integer0 = (int32_t) Long;						// 예:  12706
	integer1 = integer0 / 100;						// 예:  127
	integer2 = integer0 % 100;						// 예:	6
	fraction = Long - (double) integer0;// 예:	12706.23190 - 12706 = 0.23190
	Long = (double) integer2 + fraction;		// 예:  6 + 0.23190 = 6.23190
	Long /= 60;									// 예:	6.23190 / 60 = 0.103865
	Long = (double) integer1 + Long;		// 예:	127 + 0.103865 = 127.103865
	Long *= (double) GPS_POS_ADJUST_FACTOR;	// 예:	127.103865 * 1e7 = 127103865

	/* 위도를 0.1마이크로도 단위로 변환 */
	integer0 = (int32_t) Lati;						// 예:  12706
	integer1 = integer0 / 100;						// 예:  127
	integer2 = integer0 % 100;						// 예:	6
	fraction = Lati - (double) integer0;// 예:	12706.23190 - 12706 = 0.23190
	Lati = (double) integer2 + fraction;		// 예:  6 + 0.23190 = 6.23190
	Lati /= 60;									// 예:	6.23190 / 60 = 0.103865
	Lati = (double) integer1 + Lati;		// 예:	127 + 0.103865 = 127.103865
	Lati *= (double) GPS_POS_ADJUST_FACTOR;	// 예:	127.103865 * 1e7 = 127103865

	/* 경/위도 값 반환 */
	*cvrtLongitude = (int32_t) Long;
	*cvrtLatitude = (int32_t) Lati;

	/* 서경, 남위일 경우 음수값으로 변경 */
	if (*getLongitude < 0 ) { *cvrtLongitude *= -1; }
	if (*getLatitude < 0 ) { *cvrtLatitude *= -1; }
	
	//if (EW == 'W') { *longitude *= -1; }
	//if (NS == 'S') { *latitude *= -1; }

	if( *cvrtLongitude > 1800000000 ) { *cvrtLongitude = 1800000001; }
	else if( *cvrtLatitude < -1800000000 ) { *cvrtLatitude = 1800000001; }
	if( *cvrtLatitude> 900000000 ) { *cvrtLatitude = 900000001; }
	else if( *cvrtLatitude < -900000000 ) { *cvrtLatitude = 900000001; }
}

/****************************************************************************************

	ConvertStrToRawPosition()
		- NMEA 메시지에서 파싱된 경도/위도 문자열을 경/위도 변수값으로 변환
		- NMEA 메시지의 도분 단위의 소수점을 제거하여 반환한다.

	arguments
		longitude_str		경도 문자열
		latitude_str		위도 문자열
		long_dir_str		경도 방향 문자열 (E, W)
		lati_dir_str		위도 방향 문자열 (N, S)
		longitude			경도값이 저장될 변수
		latitude			위도값이 저장될 변수
		long_dir			경도방향이 저장될 변수
		lati_dir			위도방향이 저장될 변수

	return

****************************************************************************************/
void ConvertStrToRawPosition(IN BYTE *longitudeStr, IN BYTE *latitudeStr, IN BYTE *longDirStr, IN BYTE *latiDirStr,
						OUT int32_t *longitude, OUT int32_t *latitude, OUT uint8_t *longDir, OUT uint8_t *latiDir)
{
	double Long;
	double Lati;

	/* 문자열 형식의 경/위도를 실수로 변환 */
	Long = strtod(longitudeStr, NULL);
	Lati = strtod(latitudeStr, NULL);

	/* 경/위도 값 반환
	 * 	- 경/위도 값에 10^5를 곱해서 소수점 제거 */
	*longitude = (int32_t) (Long * (double) (1e5));
	*latitude = (int32_t) (Lati * (double) (1e5));

	/* 경/위도 방향 반환 */
	*longDir = longDirStr[0];
	*latiDir = latiDirStr[0];
}


/****************************************************************************************

	ConvertStrToSpeed()
		- GPS RMC 라인에서 현재 속도 정보 추출
		- RMC 라인에서 입력되는 속도는 knots 단위임.
		- SAE J2735에 따라 0.02m/s 단위로 변환해서 반환.
		- RMC 라인 말고 다른 라인인 경우, 속도값의 단위가 어떤건지 확인해서 추가해야 한다.
		- ETSI 표준을 따르는 경우 0.01m/s 단위를 사용한다.

	arguments
		speedStr			속도 문자열

	return
		속도 값 (J2735 or ETSI ITs-Container)

****************************************************************************************/
int32_t ConvertToSpeed(IN int32_t *get_Speed, IN int32_t get_stauts)
{
	int32_t speed;

//	log_debug("get_speed[%d], get_stauts[%d]\n",  *get_Speed, get_stauts);

#ifdef _EU_STD_
	/* 속도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		return 16383/*unavailable*/;
	}

	return (int32_t)*get_Speed;
	
#else
	/* 속도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		return 8191/*unavailable*/;
	}

	return (int32_t)*get_Speed;
	
#endif
}

/****************************************************************************************

	GetDiffsoln()
		- GPS PVT 라인에서 Bitfield flags에서 diffSoln bit 추출
		
	arguments
		Bitfield flags			

	return
		diffSoln bit

****************************************************************************************/
void GetDiff_Carrsoln(IN uint8_t *getFlasBitField, OUT uint8_t *diffsoln, OUT uint8_t *carrsoln, IN int32_t get_stauts)
{
	int32_t speed;

	/* 속도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		*diffsoln = 0x00;
		*carrsoln = 0x00;
		 return;
	}
    
	*diffsoln = ( *getFlasBitField>>1) & (0x01);
	*carrsoln = ( *getFlasBitField>>6) & (0x03);
}
/****************************************************************************************

	ConvertStrToHeading()
		- GPS RMC 라인에서 현재 방면 정보(cog 항목) 추출
		- RMC 라인에서 입력되는 cog는 소수점 있는 0~359 범위의 도 단위임
		- SAE J2735에 따라 0.0125도 단위로 변환하여 반환.
		- ETSI 표준을 따르는 경우 0.1도 단위를 사용한다.

	arguments
		headingStr			방면값 문자열

	return
		속도 값 (J2735 or ETSI ITs-Container)

****************************************************************************************/
int32_t ConvertToHeading(IN double *get_Heading, IN uint32_t get_stauts)
{
	double heading;

//	log_debug("get_Heading[%d], get_stauts[%d]\n",  *get_Heading, get_stauts);

#ifdef _EU_STD_
	/* 방면 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX )  {
		return 3601/*unavailable*/;
	}

	heading = *get_Heading / 0.1;
	if(heading >= 3601) {	/* 이럴리 없겠지만(비정상 입력) */
		return 3601;
	} else if(heading < 0) {	/* 이럴리 없겠지만(비정상 입력) */
		return 3601;
	} else {
		return (uint32_t)heading;
	}
#else
	/* 방면 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX )  {
		return 28800/*unavailable*/;
	}

	heading = *get_Heading / 0.0125;
	if(heading >= 28800.0) {	/* 이럴리 없겠지만(비정상 입력) */
		return 28800;
	} else if(heading < 0.0) {	/* 이럴리 없겠지만(비정상 입력) */
		return 28800;
	} else {
		return (uint32_t)heading;
	}
#endif
}


/****************************************************************************************

	ConvertStrToElevation()
		- GPS PVT 라인에서 현재 고도 정보 추출
		- GPS PVT 라인에서는 mm 단위로 표시된다. (해수면 기준)
		- SAE J2735에 따라 0.1미터 단위로 변환하여 반환.
		- ETSI 표준을 따르는 경우 1cm(0.01m) 단위를 사용한다.
		  (ETSI 표준에는 Altitude로 되어 있지만 변수명은 동일하게 Elevation을 그냥 사용)

	arguments
		elevationStr			고도 문자열

	return
		속도 값

****************************************************************************************/
int32_t ConvertToElevation(IN int32_t *get_elevation, IN int32_t get_stauts)
{
	//#define _KMS_
	int32_t elevation;

//	log_debug("get_elevation[%d], get_stauts[%d]\n",  *get_elevation, get_stauts);
	
	/* 고도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		return -40960/*unavailable*/;
	}

	elevation = *get_elevation / 10;

	if(elevation >= 614390) { /* 최대값 */
		return 614390;
	} else if (elevation <= -40950) { /* 최소값 */
		return -40950;
	} else {
		return elevation;
	}
#if 0

#ifdef _EU_STD_
	/* 고도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		return 800001/*unavailable*/;
	}

	elevation = *get_elevation * 100);
	if(elevation >= 8000001) { /* 최대값 */
		return 800000;
	} else if (elevation <= -100000) { /* 최소값 */
		return -100000;
	} else {
		return elevation;
	}

#else
	/* 고도 문자열이 비어 있으면 Unavailable로 반환한다. */
	if(get_stauts == STATUS_NO_FIX ) {
		return -4096/*unavailable*/;
	}

	elevation = *get_elevation * 10;
	if(elevation >= 61439) { /* 최대값 */
		return 61439;
	} else if (elevation <= -4095) { /* 최소값 */
		return -4095;
	} else {
		return elevation;
	}
#endif

#endif
}


/****************************************************************************************

	CalculateUtcTime()
		- date 및 time 문자열을 기반으로 초 값을 계산한다.
		- date 정보가 없는 경우(yearStr, monthStr, dayStr이 null인 경우 또는 길이가 0인 경우)
		  현재의 시스템 시간의 date 정보를 그대로 사용한다.
		- time 문자열의 길이가 0인 경우, 실패한다.

	arguments
		yearStr			년 문자열 (yy 형식). NMEA 메시지에 date정보가 없는 경우 null로 호출된다.
		monthStr		월 문자열 (mm 형식). NMEA 메시지에 date정보가 없는 경우 null로 호출된다.
		dayStr			일 문자열 (dd 형식). NMEA 메시지에 date정보가 없는 경우 null로 호출된다.
		timeStr			시간 문자열 (hhmmss.ss 형식) - .ss는 무시된다.

	return
		성공 시, utc 시간의 초 값
		실패 시, 0

****************************************************************************************/
unsigned long CalculateUtcTime(IN char *yearStr, IN char *monthStr, IN char *dayStr, IN char *timeStr)
{
	unsigned long year = 0, month = 0, day = 0, hour, min, sec;
	char tmp[10];
	int res;
	struct tm ptm;

	memset(tmp, 0, 10);
#if 0
	/*----------------------------------------------------------------------------------*/
	/* 일자(년/월/일) 문자열을 정수로 변환
	 * 	- RMC 및 ZDA의 경우에만 일자정보가 포함되어 있다.
	 * 	- 따라서 다른 메시지 수신 시에도 일자정보를 사용하기 위해 mib에 저장해 둔다
	 * 	- GGA나 GGL등 메시지 수신 시에는 mib에 저장된 일자정보를 가지고 UTC를 계산한다. */
	/*----------------------------------------------------------------------------------*/
	/* year */
	if ((yearStr) && strlen(yearStr)) {
		memcpy(tmp, yearStr, 2);
		year = 2000 + strtoul(tmp, NULL, 10);
		g_mib.year = year;
	} else {
		year = g_mib.year;
	}
	/* month */
	if ((monthStr) && strlen(monthStr)) {
		memcpy(tmp, monthStr, 2);
		month = strtoul(tmp, NULL, 10);
		g_mib.month = month;
	} else {
		month = g_mib.month;
	}
	/* day */
	if ((dayStr) && strlen(dayStr)) {
		memcpy(tmp, dayStr, 2);
		day = strtoul(tmp, NULL, 10);
		g_mib.day = day;
	} else {
		day = g_mib.day;
	}
	/*----------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------*/
	/* 시간(시/분/초) 문자열을 정수로 변환 */
	/*----------------------------------------------------------------------------------*/
	if (strlen(timeStr) == 9) {
		/* hour
		 * 	- hour가 24가 넘는 경우에도 날짜는 정상적으로 계산됨. */
		memcpy(tmp, timeStr, 2);
		hour = strtoul(tmp, NULL, 10);	/* UTC에 동기화 */
#ifdef _KST_
		hour += 9;	/* KST에 동기화 */
#endif
		/* minute */
		memcpy(tmp, timeStr + 2, 2);
		min = strtoul(tmp, NULL, 10);
		/* second */
		memcpy(tmp, timeStr + 4, 2);
		sec = strtoul(tmp, NULL, 10);
	} else {
		return 0;
	}
	/*----------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------*/
	/* 날짜 및 시간 정보를 초 값으로 변환 */
	/*----------------------------------------------------------------------------------*/
	ptm.tm_year = year - 1900;
	ptm.tm_mon = month - 1;			//월은 0부터 시작
	ptm.tm_mday = day;
	ptm.tm_hour = hour;
	ptm.tm_min = min;
	ptm.tm_sec = sec;
	ptm.tm_isdst = 0;					// 썸머타임 사용 여부
	/* 초 값으로 변환 */
	res = mktime(&ptm);
	if (res == -1) {
		log_error("Fail to mktime: %s\n", strerror(errno));	
		return 0;
	}
	/*----------------------------------------------------------------------------------*/

	//if(dbg)	printf("Get UTC time: %02d%02d%02d.%02d%02d%02d\n", ptm.tm_year + 1900, ptm.tm_mon + 1,
	//		ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
#endif
	return (unsigned long) res;
}

struct timespec sim_CalculateUtcTime(IN timestamp_t time, IN uint32_t nanotime)
{
	struct timespec get_time;

	get_time.tv_sec = time;
	get_time.tv_nsec = (long)(time*(1e9)) + nanotime;

	return (struct timespec) get_time;

}

