#include <prcsJ2735.h>
#include <hexdump.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CLIENT_CONNECT_TIMEOUT_USEC 5
#define HEADER_SIZE 12
#define PAYROAD_SIZE 155

/* 전역변수 */
int server_sock1 = -1;
int server_sock2 = -1;
int client_sock = -1;
bool connectFlag = false;
struct sockaddr_in server_addr1, server_addr2, client_addr;
unsigned int client_addr_size = sizeof(client_addr);
pthread_mutex_t fd_mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_t sock_thread;

int sendPkt()
{
    int result;
    uint8_t buf[1024];
    int len; 

    memset(&buf, 0, sizeof(buf));

    len = getRTCM(buf);
    if(len > 0)
    {
        result = sendto( server_sock1, &buf, len, MSG_DONTWAIT|MSG_NOSIGNAL, (struct sockaddr*)&server_addr1, sizeof(client_addr) );
        if(result >  0)
        {
            if(g_mib.dbg)
            {
                //printf("[prcsJ2735] Success send packet %d byte\n", result);
                syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Success send packet %d byte\n", result);
            }
        }
        else if(result == -1)
        {
            //perror("[prcsJ2735] packet send error :");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] packet send error :%s\n", strerror(errno));
            pthread_mutex_lock(&fd_mutex);
            close(server_sock1);
            server_sock1 = -1;
            pthread_mutex_unlock(&fd_mutex);
            return -1;
        }
        result = sendto( server_sock2, &buf, len, MSG_DONTWAIT|MSG_NOSIGNAL, (struct sockaddr*)&server_addr2, sizeof(client_addr) );
        if(result >  0)
        {

            if(g_mib.dbg)
            {
                //printf("[prcsJ2735] Success send packet %d byte\n", result);
                syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Success send packet %d byte\n", result);
              //  hexdump(buf, result);
            }
        }
        else if(result == -1)
        {
            //perror("[prcsJ2735] packet send error :");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] packet send error :%s\n", strerror(errno));
            pthread_mutex_lock(&fd_mutex);
            close(server_sock2);
            server_sock2 = -1;
            pthread_mutex_unlock(&fd_mutex);
            return -1;
        }

    }
    else
        return 0;

    return result;
}

int recvPkt()
{
    int result;
    uint8_t buf[1024];

    /* Receive UDP Packet  */
    result = recvfrom(client_sock, &buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &client_addr_size);
    if( result > 0 )
    {
        if(g_mib.dbg)
        {
            //printf("[prcsJ2735] Success recv packet %d byte\n", result);
            syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Success receive packet %d byte\n", result);
        }
        setRTCM(buf, result);
    }
    else if(result == -1)
    {
        //perror("[prcsJ2735] packet recv error :");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] packet recv error : %s\n", strerror(errno));
        pthread_mutex_lock(&fd_mutex);
        close(client_sock);
        client_sock = -1;
        pthread_mutex_unlock(&fd_mutex);
        return -1;
    }
}


/* socket Thread */
static void* sock_func(void* arg)
{
    uint32_t result;

    while(!ending)
    {
        /* Socket check */
        if(g_mib.sockType == udpServer)
        {
            if(server_sock1 == -1 && server_sock2 == -1)
            {
                //printf("[prcsJ2735] No setting socket yet\n");
                syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] No setting socket yet\n");
                sleep(1);

                continue;
            }

            pthread_mutex_lock(&g_mib.txMtx);
            pthread_cond_wait(&g_mib.txCond, &g_mib.txMtx);
            pthread_mutex_unlock(&g_mib.txMtx);

            sendPkt();
        }
        if(g_mib.sockType == udpClient)
        {
            if(client_sock == -1)
            {
                //printf("[prcsJ2735] No setting socket yet\n");
                syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] No setting socket yet\n");
                sleep(1);

                continue;
            }

            recvPkt();
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
        //perror("[prcsJ2735] Fail to create recv thread : ");
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] Fail to create recv thread : %s\n", strerror(errno));
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
        //printf("Completed join with sockThread status = %d\n", status);
        syslog(LOG_INFO | LOG_LOCAL0, "[prcsJ2735] Completed join with sockThread status = %d\n", status);
    }
    else
    {
        //printf("[prcsJ2735] ERROR: return code from pthread_join() is %d\n", rc);
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] ERROR: return code from pthread_join() is %d\n", rc);
    }

    pthread_mutex_destroy(&fd_mutex);

    return;
}

int serverSockConnection(int connect_timeout_usec)
{
    int tmpfd = -1;

    if( server_sock1 == -1 )
    {
        //printf(">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        syslog(LOG_INFO | LOG_LOCAL0, ">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        if ((tmpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            //perror("[prcsJ2735] sock failed");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] sock failed : %s\n", strerror(errno));
            return -1;
        }

        memset(&server_addr1, 0, sizeof(server_addr1));
        server_addr1.sin_family = AF_INET;
        server_addr1.sin_port = htons(20001);
        server_addr1.sin_addr.s_addr = inet_addr("192.168.200.1");

        server_sock1 = dup(tmpfd);
        close(tmpfd);
    }
    if( server_sock2 == -1 )
    {
        //printf(">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        syslog(LOG_INFO | LOG_LOCAL0, ">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        if ((tmpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            //perror("[prcsJ2735] sock failed");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] sock failed : %s\n", strerror(errno));
            return -1;
        }

        memset(&server_addr2, 0, sizeof(server_addr2));
        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = htons(20003);
        server_addr2.sin_addr.s_addr = inet_addr("192.168.200.3");


        server_sock2 = dup(tmpfd);
        close(tmpfd);
    }
    return 0;
}

int clientSockConnection(int connect_timeout_usec)
{
    int tmpfd = -1;


    if( client_sock == -1 )
    {
        //printf(">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        syslog(LOG_INFO | LOG_LOCAL0, ">>>>>>>>>>>>> SOCKET CONNECTION CHECK <<<<<<<<<<<<<<<<<<<<<<\n");
        if ((tmpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            //perror("[prcsJ2735] sock failed");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] sock failed : %s\n", strerror(errno));
            return -1;
        }

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(g_mib.destPort);
        client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        /* udp : Assign a port number to the socket */
        if( bind(tmpfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) != 0 )
        {
            //perror("[prcsJ2735] udp Socket--bind ");
            syslog(LOG_ERR | LOG_LOCAL1, "[prcsJ2735] bind failed : %s\n", strerror(errno));
            exit(errno);
        }


        client_sock = dup(tmpfd);
        close(tmpfd);
    }
    return 0;
}

void connection_Check(void)
{
    if(g_mib.sockType == udpServer)
    {
        if( server_sock1 == -1 || server_sock2 == -1) 
        {
            if( serverSockConnection(CLIENT_CONNECT_TIMEOUT_USEC) == -1)
                return;

        }
    }
    else if(g_mib.sockType == udpClient)
    {
        if( client_sock == -1) 
        {
            if( clientSockConnection(CLIENT_CONNECT_TIMEOUT_USEC) == -1)
                return;

        }
    }

    return;
}

