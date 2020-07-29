/**
 * @file v2x-obu-rx.c
 * @date 2019-08-19
 * @author gyun
 * @brief WSM 수신 처리 기능 구현
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <dot3/dot3.h>
#include <sys/time.h>
#include <time.h>

#include "dot3/dot3.h"

#include "v2x-obu.h"
#include "hexdump.h"


/**
 * 파싱된 WSA의 내용을 출력한다.
 *
 * @param params    파싱된 WSA 정보
 */
static void V2X_OBU_PrintWsaParseParams(const struct Dot3ParseWsaParams *const params)
{
  //printf("WSA params\n");
  syslog(LOG_INFO | LOG_LOCAL6, "WSA params\n");
  //printf("  version: %u, wsa_id: %u, content_count: %u\n",
  //        params->hdr.version, params->hdr.wsa_id, params->hdr.content_count);
  syslog(LOG_INFO | LOG_LOCAL6, "  version: %u, wsa_id: %u, content_count: %u\n",
          params->hdr.version, params->hdr.wsa_id, params->hdr.content_count);
  if (params->hdr.extensions.repeat_rate) {
      //printf("  repeat_rate: %u\n", params->hdr.repeat_rate);
      syslog(LOG_INFO | LOG_LOCAL6, "  repeat_rate: %u\n", params->hdr.repeat_rate);
  }
  if (params->hdr.extensions.twod_location) {
      //printf("  2DLocation.latitude: %d\n", params->hdr.twod_location.latitude);
      //printf("  2DLocation.longitude: %d\n", params->hdr.twod_location.longitude);
      syslog(LOG_INFO | LOG_LOCAL6, "  2DLocation.latitude: %d\n", params->hdr.twod_location.latitude);
      syslog(LOG_INFO | LOG_LOCAL6, "  2DLocation.longitude: %d\n", params->hdr.twod_location.longitude);
  }
  if (params->hdr.extensions.threed_location) {
      //printf("  3DLocation.latitude: %d\n", params->hdr.threed_location.latitude);
      //printf("  3DLocation.longitude: %d\n", params->hdr.threed_location.longitude);
      //printf("  3DLocation.elevation: %d\n", params->hdr.threed_location.elevation);

      syslog(LOG_INFO | LOG_LOCAL6, "  3DLocation.latitude: %d\n", params->hdr.threed_location.latitude);
      syslog(LOG_INFO | LOG_LOCAL6, "  3DLocation.longitude: %d\n", params->hdr.threed_location.longitude);
      syslog(LOG_INFO | LOG_LOCAL6, "  3DLocation.elevation: %d\n", params->hdr.threed_location.elevation);
  }
  if (params->hdr.extensions.advertiser_id) {
      //printf("  Advertiser Id: %s\n", params->hdr.advertiser_id.id);
      syslog(LOG_INFO | LOG_LOCAL6, "  Advertiser Id: %s\n", params->hdr.advertiser_id.id);
  }
  for (int i = 0; i < params->wsi_num; i++) {
      //printf("  Serv info[%d] - psid: %u, channel_index: %u,", i, params->wsis[i].psid, params->wsis[i].channel_index);
      syslog(LOG_INFO | LOG_LOCAL6, "  Serv info[%d] - psid: %u, channel_index: %u,", i, params->wsis[i].psid, params->wsis[i].channel_index);
      if (params->wsis[i].extensions.ipv6_address) {
          char ip_str[IPV6_ADDR_STR_MAX_LEN];
          inet_ntop(AF_INET6, params->wsis[i].ipv6_address, ip_str, IPV6_ADDR_STR_MAX_LEN);
          //printf("  ip: %s, ", ip_str);
          syslog(LOG_INFO | LOG_LOCAL6, "  ip: %s, ", ip_str);
      }
      if (params->wsis[i].extensions.service_port) {
          //printf("  port: %u, ", params->wsis[i].service_port);
          syslog(LOG_INFO | LOG_LOCAL6, "  port: %u, ", params->wsis[i].service_port);
      }
      //printf("\n");
      syslog(LOG_INFO | LOG_LOCAL6, "\n");
  }
  for (int i = 0; i < params->wci_num; i++) {
      //printf("  Chan info[%d] - chan: %d, power: %d, datarate: %d, adaptable: %u\n",
              //i,params->wcis[i].chan_num,
              //params->wcis[i].transmit_power_level,
              //params->wcis[i].datarate,
              //params->wcis[i].adaptable_datarate);
      syslog(LOG_INFO | LOG_LOCAL6, "  Chan info[%d] - chan: %d, power: %d, datarate: %d, adaptable: %u\n",
              i,params->wcis[i].chan_num,
              params->wcis[i].transmit_power_level,
              params->wcis[i].datarate,
              params->wcis[i].adaptable_datarate);
  }
  if (params->present.wra) {
      char ip_prefix_str[IPV6_ADDR_STR_MAX_LEN], default_gw[IPV6_ADDR_STR_MAX_LEN], primary_dns[IPV6_ADDR_STR_MAX_LEN];
      inet_ntop(AF_INET6, params->wra.ip_prefix, ip_prefix_str, IPV6_ADDR_STR_MAX_LEN);
      inet_ntop(AF_INET6, params->wra.default_gw, default_gw, IPV6_ADDR_STR_MAX_LEN);
      inet_ntop(AF_INET6, params->wra.primary_dns, primary_dns, IPV6_ADDR_STR_MAX_LEN);
      //printf("  WRA - lifetime: %u, ip_prefix: %s/%u, default_gw: %s, primary_dns: %s\n",
              //params->wra.router_lifetime, ip_prefix_str, params->wra.ip_prefix_len, default_gw, primary_dns);
      syslog(LOG_INFO | LOG_LOCAL6, "  WRA - lifetime: %u, ip_prefix: %s/%u, default_gw: %s, primary_dns: %s\n",
              params->wra.router_lifetime, ip_prefix_str, params->wra.ip_prefix_len, default_gw, primary_dns);
  }
}


/**
 * 수신된 MPDU 를 처리한다.
 *  - WSM 파싱을 시도한다.
 *
 *
 * @param mpdu      수신된 MPDU
 * @param mpdu_size 수신된 MPDU의 크기
 */
void V2X_OBU_ProcessRxMpdu(const uint8_t *const mpdu, const uint16_t mpdu_size)
{

    /*
     * WSM MPDU 파싱
     */
    struct Dot3WsmMpduRxParams dot3_params;
    bool wsr_registered;
    uint8_t BUFFER[kMpduMaxSize];
    uint8_t outbuf[kMpduMaxSize];
    int len=0;
    int payload_size = Dot3_ParseWsmMpdu(mpdu, mpdu_size, outbuf, sizeof(outbuf), &dot3_params, &wsr_registered);
    if (payload_size < 0) {
        if(g_dbg)
        {
            //printf("Fail to Dot3_ParseWsmMpdu() %d\n", payload_size);
            //printf("------------------------------------------------------------\n\n");
            syslog(LOG_ERR | LOG_LOCAL7, "Fail to Dot3_ParseWsmMpdu() %d\n", payload_size);
            syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
        }
        return;
    }

    if (g_dbg >= kDbgMsgLevel_event) {
#if 0
        printf("Success to Dot3_ParseWsmMpdu() - payload_size: %d\n", payload_size);
        printf("    tx_chan_num: %d, tx_datarate: %d, tx_power: %d, priority: %d, psid: %d\n",
                dot3_params.tx_chan_num, dot3_params.tx_datarate, dot3_params.tx_power, dot3_params.priority, dot3_params.psid);
        printf("    dst_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X, src_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                dot3_params.dst_mac_addr[0], dot3_params.dst_mac_addr[1], dot3_params.dst_mac_addr[2],
                dot3_params.dst_mac_addr[3], dot3_params.dst_mac_addr[4], dot3_params.dst_mac_addr[5],
                dot3_params.src_mac_addr[0], dot3_params.src_mac_addr[1], dot3_params.src_mac_addr[2],
                dot3_params.src_mac_addr[3], dot3_params.src_mac_addr[4], dot3_params.src_mac_addr[5]);
#endif
        syslog(LOG_INFO | LOG_LOCAL6, "Success to Dot3_ParseWsmMpdu() - payload_size: %d\n", payload_size);
        syslog(LOG_INFO | LOG_LOCAL6, "    tx_chan_num: %d, tx_datarate: %d, tx_power: %d, priority: %d, psid: %d\n",
                dot3_params.tx_chan_num, dot3_params.tx_datarate, dot3_params.tx_power, dot3_params.priority, dot3_params.psid);
	syslog(LOG_INFO | LOG_LOCAL6, "rx_power : %d, rcpi : %d \n",
			g_mib.rxpower, g_mib.rcpi);
        //syslog(LOG_INFO | LOG_LOCAL6, "    dst_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X, src_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                //dot3_params.dst_mac_addr[0], dot3_params.dst_mac_addr[1], dot3_params.dst_mac_addr[2],
                //dot3_params.dst_mac_addr[3], dot3_params.dst_mac_addr[4], dot3_params.dst_mac_addr[5],
                //dot3_params.src_mac_addr[0], dot3_params.src_mac_addr[1], dot3_params.src_mac_addr[2],
                //dot3_params.src_mac_addr[3], dot3_params.src_mac_addr[4], dot3_params.src_mac_addr[5]);

#if 0
        if (g_dbg >= kDbgMsgLevel_msgdump) {
            for (int i = 0; i < payload_size; i++) {
                if ((i != 0) && (i % 16 == 0)) {
                    printf("\n");
                }
                //printf("%02X ", outbuf[i]);
                syslog(LOG_INFO | LOG_LOCAL6, "%02X ", outbuf[i]);
            }
            //printf("\n");
            syslog(LOG_INFO | LOG_LOCAL6, "\n");
        }
#endif
    }

    /*
     * WSA 인 경우 파싱한다.
     */
    if (dot3_params.psid == kDot3Psid_Wsa) {
        struct Dot3ParseWsaParams wsa_params;
        memset(&wsa_params, 0, sizeof(wsa_params));
        int ret = Dot3_ParseWsa(outbuf, payload_size, &wsa_params);
        if (ret < 0) {
            if(g_dbg)
            {
                //printf("Fail to parse WSA - %d\n", ret);
                //printf("------------------------------------------------------------\n\n");
                syslog(LOG_INFO | LOG_LOCAL6, "Fail to parse WSA - %d\n", ret);
                syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
            }
            return;
        }
        if (g_dbg >= kDbgMsgLevel_event) {
            //printf("Success to parse WSA()\n");
            syslog(LOG_INFO | LOG_LOCAL6, "Success to parse WSA()\n");
            V2X_OBU_PrintWsaParseParams(&wsa_params);
        }
    }
    /*
     * 원하는 WSMP인 경우 적당히 처리한다.
     */
    if (dot3_params.psid == g_mib.psid) {
        sendMQ(outbuf, payload_size);
        if (g_dbg >= kDbgMsgLevel_event) {
            //printf("Processing interseted WSM for psid %u\n", dot3_params.psid);
            //printf("------------------------------------------------------------\n\n");
            syslog(LOG_INFO | LOG_LOCAL6, "Processing interseted WSM for psid %u\n", dot3_params.psid);
            syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
        }
        /* TO DO */
    }
    else if (dot3_params.psid == 7777) {
	    memset(BUFFER,0,sizeof(kMpduMaxSize));
	    memcpy(BUFFER+len,outbuf,payload_size);
	    len+=payload_size;
	    memcpy(BUFFER+len, &g_mib.rxpower, sizeof(int16_t)); //int16_t short int 2Byte
	    len+=sizeof(int16_t);
	    memcpy(BUFFER+len, &g_mib.rcpi, sizeof(uint8_t)); //uint8_t unsigned char 1Byte
	    len+=sizeof(uint8_t);
        PARsendMQ(BUFFER, len);
        if (g_dbg >= kDbgMsgLevel_event) {
            //printf("Processing interseted WSM for psid %u\n", dot3_params.psid);
            //printf("------------------------------------------------------------\n\n");
            syslog(LOG_INFO | LOG_LOCAL6, "Processing interseted WSM for PAR psid %u\n", dot3_params.psid);
            syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
        }
        /* TO DO */
    }


    /*
     * 그 외 WSMP는 무시한다.
     */
    else {
        if (g_dbg >= kDbgMsgLevel_event) {
            //printf("Drop not interseted WSM for psid %u\n", dot3_params.psid);
            //printf("------------------------------------------------------------\n\n");
            syslog(LOG_INFO | LOG_LOCAL6, "Drop not interseted WSM for psid %u\n", dot3_params.psid);
            syslog(LOG_INFO | LOG_LOCAL6, "------------------------------------------------------------\n\n");
        }
    }
}


