//
// Created by gyun on 2019-08-12.
//

#include <stdio.h>
#include <string.h>

#include "dot3/dot3.h"

/**
 * 테스트 어플리케이션 메인 함수
 *
 * @param argc
 * @param argv
 * @return      성공 시 0, 실패 시 -1
 */
int main(int argc, char *argv[])
{
  printf("\nRunning libdot3 test application\n\n");

  /*
   * 라이브러리 초기화
   */
  int ret = Dot3_Init(6);
  if (ret < 0) {
    printf("Fail to Dot3_Init() - %d\n", ret);
    return -1;
  }
  printf("Success to Dot3_Init()\n");

  uint8_t payload[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  uint8_t mpdu[kMpduMaxSize] = {0};
  uint8_t outbuf[kMpduMaxSize] = {0};

  /*
   * MPDU 생성 테스트
   */
  printf("\nTrying to Dot3_ConstructWsmMpdu()\n");
  struct Dot3WsmMpduTxParams tx_params;
  tx_params.hdr_extensions.chan_num = true;
  tx_params.hdr_extensions.datarate = true;
  tx_params.hdr_extensions.transmit_power = true;
  tx_params.ifindex = 0;
  tx_params.chan_num = 172;
  tx_params.timeslot = kDot3TimeSlot_Continuous;
  tx_params.datarate = kDot3DataRate_6Mbps;
  tx_params.transmit_power = 20;
  tx_params.priority = 7;
  memset(tx_params.dst_mac_addr, 0xff, kDot3MacAddrSize);
  tx_params.psid = 32;
  int mpdu_size = Dot3_ConstructWsmMpdu(&tx_params, payload, sizeof(payload), mpdu, sizeof(mpdu));
  if (mpdu_size < 0) {
    printf("Fail to Dot3_ConstructWsmMpdu() %d\n", mpdu_size);
    return -1;
  }
  printf("Success to Dot3_ConstructWsmMpdu() - mpdu_size: %d\n", mpdu_size);
  for (int i = 0; i < mpdu_size; i++) {
    if ((i!=0) && (i%16==0)) {
      printf("\n");
    }
    printf("%02X ", mpdu[i]);
  }
  printf("\n");

  /*
   * MPDU 파싱 테스트
   */
  printf("\nTrying to Dot3_ParseWsmMpdu()\n");
  struct Dot3WsmMpduRxParams rx_params;
  bool wsr_registered;
  int payload_size = Dot3_ParseWsmMpdu(mpdu, mpdu_size, outbuf, sizeof(outbuf), &rx_params, &wsr_registered);
  if (payload_size < 0) {
    printf("Fail to Dot3_ParseWsmMpdu() %d\n", payload_size);
    return -1;
  }
  printf("Success to Dot3_ParseWsmMpdu() - payload_size: %d\n", payload_size);
  for (int i = 0; i < payload_size; i++) {
    if ((i!=0) && (i%16==0)) {
      printf("\n");
    }
    printf("%02X ", outbuf[i]);
  }
  printf("\n");
  printf("Rx parameters - tx_chan_num: %d, tx_datarate: %d, tx_power: %d, priority: %d, psid: %d\n",
    rx_params.tx_chan_num, rx_params.tx_datarate, rx_params.tx_power, rx_params.priority, rx_params.psid);
  printf("    dst_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X, src_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
         rx_params.dst_mac_addr[0], rx_params.dst_mac_addr[1], rx_params.dst_mac_addr[2],
         rx_params.dst_mac_addr[3], rx_params.dst_mac_addr[4], rx_params.dst_mac_addr[5],
         rx_params.src_mac_addr[0], rx_params.src_mac_addr[1], rx_params.src_mac_addr[2],
         rx_params.src_mac_addr[3], rx_params.src_mac_addr[4], rx_params.src_mac_addr[5]);

  printf("\nSuccess to test libdot3\n\n");
  return 0;
}
