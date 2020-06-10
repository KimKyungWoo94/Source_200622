#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>

#define ADDRSIZE 20

/* STRUCT */
typedef struct
{
    uint32_t serverPort;

    char cnvcIP[ADDRSIZE];
    uint32_t cnvcPort;

    char adasIP[ADDRSIZE];
    uint32_t adasPort;

    /* 디버그 변수 */
    uint32_t    dbg;
} mib_t;

typedef struct 
{
    uint8_t version;
    uint32_t utcTime_sec;
    uint32_t utcTime_usec;
    uint8_t type;

    uint8_t fixType;
    uint8_t diffSoln;
    uint8_t carSoln;
    int32_t lati;
    int32_t longi;
    int32_t elev;
    int32_t nedNorSpd;
    int32_t nedEastSpd;
    int32_t endDownSpd;
    uint32_t heading;
    uint8_t numSV;
} gpsPkt_t;

typedef struct
{
    uint8_t version;
    uint32_t utcTime_sec;
    uint32_t utcTime_usec;
    uint8_t type;

    uint8_t dtcCode[5];
} dtcPkt_t;

typedef struct 
{
    uint8_t version;
    uint32_t utcTime_sec;
    uint32_t utcTime_usec;
    uint8_t type;
    
    uint8_t curSpeed;
    uint16_t curRPM;
    uint16_t batteryVolt;
    int16_t coolantTemp;
    int16_t engineOilTemp;
    int16_t handleAngle;
    uint8_t remaindererOil;
    uint16_t averageFuel;
    int16_t inhalationTemp;
    uint16_t inhalationSensor;
    uint8_t maf;
    int16_t exhaustTemp;
    int16_t cdf_dpf_capacity;
    int16_t cdf_dpf_temp;
    int16_t batteryTemp;
    uint8_t remainderBattery;
    uint8_t pneumatic;
    uint8_t torqueScalingFactor;
    int16_t engineTorque;
    uint8_t gear;
} vifPkt_t;

typedef struct 
{
    uint8_t version;
    uint8_t type;

    uint8_t deviceType;
    uint8_t deviceID;
    uint16_t size;
    uint8_t data[65507];
} v2icPkt_t;













/****************************************************************************************
	전역변수

****************************************************************************************/
extern mib_t		g_mib;
extern int ending;

/****************************************************************************************
	함수원형(지역/전역)

****************************************************************************************/
/* options.c */
int32_t ParsingOptions(int32_t argc, char *argv[]);
/* socket.c */
int createSockThread();
void closeSocketThread();
void connection_Check(void);
