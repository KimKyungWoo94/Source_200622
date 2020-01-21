#include <infor_broker.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <hexdump.h>

#define CLIENT_CONNECT_TIMEOUT_USEC 5
#define HEADER_SIZE 12
#define PAYROAD_SIZE 155

/* 전역변수 */
int server_sock = -1;
int cnvc_sock = -1;
int adas_sock = -1;
bool connectFlag = false;
struct sockaddr_in server_addr, cnvc_addr, adas_addr, client_addr;
unsigned int client_addr_size = sizeof(client_addr);
pthread_t sock_thread;

static void printPkt(v2icPkt_t *pkt)
{
    syslog(LOG_INFO | LOG_LOCAL0, "##### [infor_broker] PKT #####\n");
    syslog(LOG_INFO | LOG_LOCAL0, "version             : %u\n", pkt->version);
    syslog(LOG_INFO | LOG_LOCAL0, "deviceType          : %u\n", pkt->deviceType);
    syslog(LOG_INFO | LOG_LOCAL0, "deviceID            : %u\n", pkt->deviceID);
    syslog(LOG_INFO | LOG_LOCAL0, "size                : %u\n", pkt->size);

    if(pkt->size <= 0)
        return;
        
    switch(pkt->type)
    {
        case 1 :
            {
                gpsPkt_t gpsData;
                memset(&gpsData, 0, sizeof(gpsPkt_t));
                memcpy(&gpsData, pkt->data, sizeof(gpsPkt_t));

                syslog(LOG_INFO | LOG_LOCAL0, "type                : Calibration Coordinate of GPS\n\n");
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_sec         : %u\n", gpsData.utcTime_sec);
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_usec        : %u\n", gpsData.utcTime_usec);
                syslog(LOG_INFO | LOG_LOCAL0, "fixType             : %u\n", gpsData.fixType);
                syslog(LOG_INFO | LOG_LOCAL0, "diffSlon            : %u\n", gpsData.diffSoln);
                syslog(LOG_INFO | LOG_LOCAL0, "carSoln             : %u\n", gpsData.carSoln);
                syslog(LOG_INFO | LOG_LOCAL0, "latitude            : %d\n", gpsData.lati);
                syslog(LOG_INFO | LOG_LOCAL0, "longitude           : %d\n", gpsData.longi);
                syslog(LOG_INFO | LOG_LOCAL0, "elevation           : %d\n", gpsData.elev);
                syslog(LOG_INFO | LOG_LOCAL0, "nedNorSpd           : %d\n", gpsData.nedNorSpd);
                syslog(LOG_INFO | LOG_LOCAL0, "nedEastSpd          : %d\n", gpsData.nedEastSpd);
                syslog(LOG_INFO | LOG_LOCAL0, "endDownSpd          : %d\n", gpsData.endDownSpd);
                syslog(LOG_INFO | LOG_LOCAL0, "heading             : %u\n", gpsData.heading);
                syslog(LOG_INFO | LOG_LOCAL0, "numSV               : %u\n", gpsData.numSV);
                break;
            }
        case 3 :
            {
                dtcPkt_t dtcData;
                memset(&dtcData, 0, sizeof(dtcPkt_t));
                memcpy(&dtcData, pkt->data, sizeof(dtcPkt_t));

                syslog(LOG_INFO | LOG_LOCAL0, "type                : DTC Code\n\n");
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_sec         : %u\n", dtcData.utcTime_sec);
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_usec        : %u\n", dtcData.utcTime_usec);
                syslog(LOG_INFO | LOG_LOCAL0, "dtcCode             : %u %u %u %u %u\n", dtcData.dtcCode[0], dtcData.dtcCode[1], dtcData.dtcCode[2], dtcData.dtcCode[3], dtcData.dtcCode[4]);
                break;
            }
        case 5 :
            {
                syslog(LOG_INFO | LOG_LOCAL0, "type                : Forward Target\n\n");
                break;
            }
        case 7 : 
            {
                syslog(LOG_INFO | LOG_LOCAL0, "type                : State of Network Camera\n\n");
                break;
            }
        case 9 :
            {
                syslog(LOG_INFO | LOG_LOCAL0, "type                : State of LiDAR\n\n");
                break;
            }
        case 16 : 
            {
                vifPkt_t vifData;
                memset(&vifData, 0, sizeof(vifPkt_t));
                memcpy(&vifData, pkt->data, sizeof(vifPkt_t));

                syslog(LOG_INFO | LOG_LOCAL0, "type                : State of ADAS(RPM Information)\n\n");
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_sec         : %u\n", vifData.utcTime_sec);
                syslog(LOG_INFO | LOG_LOCAL0, "utcTime_usec        : %u\n", vifData.utcTime_usec);
                syslog(LOG_INFO | LOG_LOCAL0, "curSpeed            : %u\n", vifData.curSpeed);
                syslog(LOG_INFO | LOG_LOCAL0, "curRPM              : %u\n", vifData.curRPM);
                syslog(LOG_INFO | LOG_LOCAL0, "batteryVolt         : %u\n", vifData.batteryVolt);
                syslog(LOG_INFO | LOG_LOCAL0, "coolantTemp         : %hd\n", vifData.coolantTemp);
                syslog(LOG_INFO | LOG_LOCAL0, "engineOilTemp       : %hd\n", vifData.engineOilTemp);
                syslog(LOG_INFO | LOG_LOCAL0, "handleAngle         : %hd\n", vifData.handleAngle);
                syslog(LOG_INFO | LOG_LOCAL0, "remainderOil        : %u\n", vifData.remaindererOil);
                syslog(LOG_INFO | LOG_LOCAL0, "averageFuel         : %u\n", vifData.averageFuel);
                syslog(LOG_INFO | LOG_LOCAL0, "inhalationTemp      : %hd\n", vifData.inhalationTemp);
                syslog(LOG_INFO | LOG_LOCAL0, "inhalationSensor    : %u\n", vifData.inhalationSensor);
                syslog(LOG_INFO | LOG_LOCAL0, "maf                 : %u\n", vifData.maf);
                syslog(LOG_INFO | LOG_LOCAL0, "exhaustTemp         : %hd\n", vifData.exhaustTemp);
                syslog(LOG_INFO | LOG_LOCAL0, "cdf_dpf_capacity    : %hd\n", vifData.cdf_dpf_capacity);
                syslog(LOG_INFO | LOG_LOCAL0, "cdf_dpf_temp        : %hd\n", vifData.cdf_dpf_temp);
                syslog(LOG_INFO | LOG_LOCAL0, "batteryTemp         : %hd\n", vifData.batteryTemp);
                syslog(LOG_INFO | LOG_LOCAL0, "remainderBattery    : %u\n", vifData.remainderBattery);
                syslog(LOG_INFO | LOG_LOCAL0, "pneumatic           : %u\n", vifData.pneumatic);
                syslog(LOG_INFO | LOG_LOCAL0, "torqueScalingFactor : %u\n", vifData.torqueScalingFactor);
                syslog(LOG_INFO | LOG_LOCAL0, "engineTorque        : %hd\n", vifData.engineTorque);
                syslog(LOG_INFO | LOG_LOCAL0, "gear                : %u\n", vifData.gear);

                break;
            }
        default :
            syslog(LOG_INFO | LOG_LOCAL0, "type              : Invalid type\n\n");
            break;
    }
}

int sendPkt(v2icPkt_t *sendPkt, int len)
{
    int result;

    if(len > 0)
    {
        if(sendPkt->type != 16)
        {
            result = sendto( cnvc_sock, sendPkt, len, MSG_DONTWAIT|MSG_NOSIGNAL, (struct sockaddr*)&cnvc_addr, sizeof(cnvc_addr) );
            if(result >  0)
            {
                if(g_mib.dbg)
                {
                    syslog(LOG_INFO | LOG_LOCAL0, "[infor_broker] Success send packet to CARNAVICOM control center%d byte\n", result);
                }
            }
            else if(result == -1)
            {
                syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] packet send error :%s\n", strerror(errno));
                close(cnvc_sock);
                server_sock = -1;
                return -1;
            }
        }
        if(sendPkt->deviceType == 1)
        {
            result = sendto( adas_sock, sendPkt, len, MSG_DONTWAIT|MSG_NOSIGNAL, (struct sockaddr*)&adas_addr, sizeof(adas_addr) );
            if(result >  0)
            {
                if(g_mib.dbg)
                {
                    syslog(LOG_INFO | LOG_LOCAL0, "[infor_broker] Success send packet to ADAS ONE crontrol center %d byte\n", result);
                }
            }
            else if(result == -1)
            {
                syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] packet send error :%s\n", strerror(errno));
                close(adas_sock);
                adas_sock = -1;
                return -1;
            }
        }
    }
    else
        return 0;

    return result;
}

/* socket Thread */
static void* sock_func(void* arg)
{
    uint32_t result;
    v2icPkt_t recvPkt;
    int len; 

    /* Server socket Create */
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        //perror("[prcsJ2735] sock failed");
        syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] server sock failed : %s\n", strerror(errno));
        pthread_exit((void *)-1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(g_mib.serverPort);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* socket bind */
    if( bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0 )
    {
        syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] bind failed : %s\n", strerror(errno));
        exit(errno);
    }

    while(!ending)
    {
        /* buf 초기화 */
        memset(&recvPkt, 0, sizeof(v2icPkt_t));

        /* Pkt 수신- Blocking */
        len = recvfrom(server_sock, &recvPkt, sizeof(v2icPkt_t), 0, (struct sockaddr*)&client_addr, &client_addr_size);
        if( len > 0 )
        {
            if(g_mib.dbg)
                syslog(LOG_INFO | LOG_LOCAL0, "[infor_broker] Success receive packet %d byte\n", len);

      //      hexdump(&recvPkt, len);

            /* 수신 Pkt pass */
            sendPkt(&recvPkt, len);

            /* 패킷 출력 */
            if(g_mib.dbg)
                printPkt(&recvPkt);
        }
    }

    pthread_exit((void *)0);
}


/* 쓰레드 생성 */
int createSockThread()
{
    /* 수신 쓰레드 생성 */
    if(  pthread_create(&sock_thread, NULL, &sock_func, NULL) != 0)
    {
        syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] Fail to create sock thread : %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

/* 쓰레드 닫기 */
void closeSocketThread()
{
    int rc;
    int status;

    rc = pthread_join(sock_thread, (void **)&status);
    if( rc == 0 )
    {
        syslog(LOG_INFO | LOG_LOCAL0, "[infor_broker] Completed join with sockThread status = %d\n", status);
    }
    else
    {
        syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] ERROR: return code from pthread_join() is %d\n", rc);
    }

    return;
}

int sockConnection(int connect_timeout_usec)
{
    int tmpfd = -1;

    if( cnvc_sock == -1 )
    {
        syslog(LOG_INFO | LOG_LOCAL0, ">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        if ((tmpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] cnvc sock failed : %s\n", strerror(errno));
            return -1;
        }

        memset(&cnvc_addr, 0, sizeof(cnvc_addr));
        cnvc_addr.sin_family = AF_INET;
        cnvc_addr.sin_port = htons(g_mib.cnvcPort);
        cnvc_addr.sin_addr.s_addr = inet_addr(g_mib.cnvcIP);

        cnvc_sock = dup(tmpfd);
        close(tmpfd);
    }

    if( adas_sock == -1 )
    {
        syslog(LOG_INFO | LOG_LOCAL0, ">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        if ((tmpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            syslog(LOG_ERR | LOG_LOCAL1, "[infor_broker] adas sock failed : %s\n", strerror(errno));
            return -1;
        }

        memset(&client_addr, 0, sizeof(client_addr));
        adas_addr.sin_family = AF_INET;
        adas_addr.sin_port = htons(g_mib.adasPort);
        adas_addr.sin_addr.s_addr = inet_addr(g_mib.adasIP);

        adas_sock = dup(tmpfd);
        close(tmpfd);
    }

    return 0;
}

void connection_Check(void)
{
    if( cnvc_sock == -1 || adas_sock) 
    {
        if( sockConnection(CLIENT_CONNECT_TIMEOUT_USEC) == -1)
            return;

    }

    return;
}



