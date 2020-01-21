#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <gps.h>

#define GPS_VERSION 1
#define PACKED __attribute__((packed))
#define		GPS_POS_ADJUST_FACTOR			(1e7)
#define IN
#define OUT
#define BYTE char


typedef struct GPS_Pkt  {
    uint8_t     version;        /* default : 1 */
    union {
        uint64_t time64;        /* UTC time */
        struct {
            uint32_t tv_sec;
            uint32_t tv_usec;
        } PACKED time;
    } PACKED;
    uint8_t     fixType;        /*GNSSfix Type: 0: no fix, 1: dead reckoning only, 2: 2D-fix, 3: 3D-fix, 
4: GNSS + dead reckoning combined, 5: time only fix*/
    uint8_t     diffsoln;       /*1 = differential corrections were applied*/  
    uint8_t     carrSoln;       /* Carrier phase range solution status:
0: no carrier phase range solution
1: carrier phase range solution with floating ambiguities
2: carrier phase range solution with fixed ambiguities */                                  
    int32_t     lat;            /* 1/10 micro degree : 0, (-900000000...900000001) */
    int32_t     lon;            /* 1/10 micro degree : 0, (-1799999999..1800000001) */
    int32_t     elev;           /*In units of 1 cm steps, (-40960..614390)   */
    int32_t     nedNorSpd;      /* Units of 0.02 m/s ,0..8191 */
    int32_t     nedEastSpd;     /* Units of 0.02 m/s ,0..8191 */
    int32_t     endDownSpd;     /* Units of 0.02 m/s ,0..8191 */
    uint32_t    heading;        /*heading  0.0125 degrees , (0..28800)  */
    uint8_t     numSV;          /*Number of satellites used in Nav Solution*/  
}PACKED GPS_Pkt_t;

void ConvertToPosition(IN int32_t *getLongitude, IN int32_t *getLatitude,
		OUT int32_t *cvrtLongitude, OUT int32_t *cvrtLatitude,
		IN int32_t get_stauts);


void ConvertStrToRawPosition(IN BYTE *longitudeStr, IN BYTE *latitudeStr, IN BYTE *longDirStr, IN BYTE *latiDirStr,
						OUT int32_t *longitude, OUT int32_t *latitude, OUT uint8_t *longDir, OUT uint8_t *latiDir);

int32_t ConvertToSpeed(IN int32_t *get_Speed, IN int32_t get_stauts);

void GetDiff_Carrsoln(IN uint8_t *getFlasBitField, OUT uint8_t *diffsoln, OUT uint8_t *carrsoln, IN int32_t get_stauts);

int32_t ConvertToHeading(IN double *get_Heading, IN uint32_t get_stauts);

int32_t ConvertToElevation(IN int32_t *get_elevation, IN int32_t get_stauts);

unsigned long CalculateUtcTime(IN char *yearStr, IN char *monthStr, IN char *dayStr, IN char *timeStr);

struct timespec sim_CalculateUtcTime(IN timestamp_t time, IN uint32_t nanotime);
