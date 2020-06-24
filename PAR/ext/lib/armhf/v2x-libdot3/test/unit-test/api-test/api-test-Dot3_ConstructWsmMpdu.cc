
/**
 * @file api-test-Dot3_ConstructWsmMpdu.cc
 * @date 2019-07-11
 * @author gyun
 * @brief Dot3_ConstructWsmMpdu() Open API에 대한 단위테스트
 *
 * 본 파일은 Dot3_ConstructWsmMpdu() Open API에 대한 단위테스트를 수행한다.
 * 파라미터 유효성, 크기, 결과값 등에 대해 테스트한다.
 * 생성된 MPDU 내 WSM 데이터의 유효성은 내부함수 단위테스트에서 수행하므로, 여기서는 수행하지 않는다.
 * 따라서, 결과 비교 시에는 MAC + LLC 헤더까지만 비교한다.
 */


#include <dot3/dot3-types.h>
#include "gtest/gtest.h"

#include "dot3/dot3.h"

static const uint8_t bcast_addr[kDot3MacAddrSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t ucast_addr[kDot3MacAddrSize] = {0xfe, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t my_addr[kDot3MacAddrSize] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

/*
 * 테스트용 샘플 데이터
 */
extern uint8_t g_min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize];
extern uint8_t g_min_size_wsm_mpdu_with_chan_num[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
extern uint8_t g_min_size_wsm_mpdu_with_datarate[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
extern uint8_t g_min_size_wsm_mpdu_with_tx_power[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
extern uint8_t g_min_size_wsm_mpdu_with_max_wsmp_hdr[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMaxSize-1];
extern uint8_t g_max_size_wsm_mpdu_with_min_wsmp_hdr[kMpduMaxSize];
extern uint8_t g_max_size_wsm_mpdu_with_max_wsmp_hdr[kMpduMaxSize];


/*
 * Test case
 *  1) params=NULL일 경우, 실패를 반환해야 한다.
 *  2) params->priority 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  3) params->chan_num 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  4) params->datarate 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  5) params->trasnmit_power 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  6) params->dst_mac_addr 값에 따라 정확한 결과 데이터를 반환해야 한다.
 *  7) params->src_mac_addr 값에 따라 정확한 결과 데이터를 반환해야 한다.
 *  8) params->psid 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  9) payload=NULL이거나 payload_size=0인 경우 정확한 결과데이터를 반환해야 한다.
 *  10) payload_size 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  11) outbuf=NULL이거나 outbuf_size=0인 경우, 실패를 반환해야 한다.
 *  12) payload_size 값과 outbuf_size 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *
 *  위 테스트 케이스들에서 정확한 결과 데이터를 확인하는 경우, MAC 헤더와 LLC 헤더만을 확인한다.
 */


/*
 * 1) params=NULL일 경우, 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, params_NULL)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  /*
   * 테스트
   */
  memset(outbuf, 0, sizeof(outbuf));
  int mpdu_size = Dot3_ConstructWsmMpdu(NULL, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_NullParameters);
}


/*
 * 2) params->priority 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, params_priority)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize] = {0};
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * priority 값 및 유효 범위 확인
   */
  int mpdu_size;
  for(int i = (kDot3Priority_Min-1)/*INT32_MIN*/; i <= (kDot3Priority_Max+1)/*INT32_MAX*/; i++) {
    params.priority = i;
    // 정상범위
    if ((params.priority >= kDot3Priority_Min) && (params.priority <= kDot3Priority_Max)) {
      min_size_wsm_mpdu_with_min_wsmp_hdr[24] = 0x20 | (i & 0xf);
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
      EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
    }
      // 비정상범위
    else {
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InvalidPriorityValue);
    }
  }
}


/*
 * 3) params->chan_num 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  - 단, params->chan_num 의 유효성은 WSMP-N-Header 확장필드에 수납될 때만 검사된다.
 */
TEST(Dot3_ConstructWsmMpdu, params_chan_num)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  uint8_t min_size_wsm_mpdu_with_chan_num[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
  memcpy(min_size_wsm_mpdu_with_chan_num,
         g_min_size_wsm_mpdu_with_chan_num,
         sizeof(g_min_size_wsm_mpdu_with_chan_num));

  /*
   * 확장필드 불포함 시, chan_num 값에 상관없이 항상 성공해야 함
   */
  params.hdr_extensions.chan_num = false;
  // kDot3Channel_Min-1 (유효하지 않은 값)
  params.chan_num = (kDot3Channel_Min-1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Channel_Min (유효한 값)
  params.chan_num = (kDot3Channel_Min);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Channel_Max+1 (유효하지 않은 값)
  params.chan_num = (kDot3Channel_Max+1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Channel_Max (유효한 값)
  params.chan_num = (kDot3Channel_Max);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));

  /*
   * 확장필드 포함시, chan_num 값 및 유효 범위 확인
   */
  params.hdr_extensions.chan_num = true;
  for(int32_t i = (kDot3Channel_Min-1)/*INT32_MIN*/; i <= (kDot3Channel_Max+1)/*INT32_MAX*/; i++) {
    params.chan_num = i;
    // 정상범위
    if ((params.chan_num >= kDot3Channel_Min) && (params.chan_num <= kDot3Channel_Max)) {
      min_size_wsm_mpdu_with_chan_num[30] = 0x0F;
      min_size_wsm_mpdu_with_chan_num[32] = (uint8_t)params.chan_num;
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_chan_num));
      EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_chan_num, mpdu_size)));
    }
    // 비정상범위
    else {
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InvalidChannelNumberValue);
    }
  }
}


/*
 * 4) params->datarate 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  - 단, params->datarate 의 유효성은 WSMP-N-Header 확장필드에 수납될 때만 검사된다.
 */
TEST(Dot3_ConstructWsmMpdu, params_datarate)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  uint8_t min_size_wsm_mpdu_with_datarate[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
  memcpy(min_size_wsm_mpdu_with_datarate,
         g_min_size_wsm_mpdu_with_datarate,
         sizeof(g_min_size_wsm_mpdu_with_datarate));

  /*
   * 확장필드 불포함 시, datarate 값에 상관없이 항상 성공해야 함
   */
  params.hdr_extensions.datarate = false;
  // kDot3DataRate_Min-1 (유효하지 않은 값)
  params.datarate = (kDot3DataRate_Min-1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3DataRate_3Mbps (유효한 값)
  params.datarate = (kDot3DataRate_3Mbps);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3DataRate_Max+1 (유효하지 않은 값)
  params.datarate = (kDot3DataRate_Max+1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3DataRate_27Mbps (유효한 값)
  params.datarate = (kDot3DataRate_27Mbps);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));

  /*
   * 확장필드 포함 시, datarate 값 및 유효 범위 확인
   */
  params.hdr_extensions.datarate = true;
  for(int32_t i = (kDot3DataRate_Min-1)/*INT32_MIN*/; i <= (kDot3DataRate_Max+1)/*INT32_MAX*/; i++) {
    params.datarate = i;
    switch(params.datarate) {
      // 정상범위
      case kDot3DataRate_3Mbps:
      case kDot3DataRate_4p5Mbps:
      case kDot3DataRate_6Mbps:
      case kDot3DataRate_9Mbps:
      case kDot3DataRate_12Mbps:
      case kDot3DataRate_18Mbps:
      case kDot3DataRate_24Mbps:
      case kDot3DataRate_27Mbps: {
        min_size_wsm_mpdu_with_datarate[30] = 0x10;
        min_size_wsm_mpdu_with_datarate[32] = (uint8_t)i;
        memset(outbuf, 0, sizeof(outbuf));
        mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
        EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_datarate));
        EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_datarate, mpdu_size)));
        break;
      }
      // 비정상범위
      default: {
        memset(outbuf, 0, sizeof(outbuf));
        mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
        EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InvalidDataRate);
        break;
      }
    }
  }
}


/*
 * 5) params->trasnmit_power 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 *  - 단, params->trasnmit_power 의 유효성은 WSMP-N-Header 확장필드에 수납될 때만 검사된다.
 */
TEST(Dot3_ConstructWsmMpdu, params_transmit_power)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  uint8_t min_size_wsm_mpdu_with_tx_power[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize+4];
  memcpy(min_size_wsm_mpdu_with_tx_power,
         g_min_size_wsm_mpdu_with_tx_power,
         sizeof(g_min_size_wsm_mpdu_with_tx_power));

  /*
   * 확장필드 불포함 시, trasnmit_power 값에 상관없이 항상 성공해야 함
   */
  params.hdr_extensions.transmit_power = false;
  // kDot3Power_Min-1 (유효하지 않은 값)
  params.transmit_power = (kDot3Power_Min-1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Power_Min (유효한 값)
  params.transmit_power = (kDot3Power_Min);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Power_Max+1 (유효하지 않은 값)
  params.transmit_power = (kDot3Power_Max+1);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  // kDot3Power_Max (유효한 값)
  params.transmit_power = (kDot3Power_Max);
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));

  /*
   * 확장필드 포함 시, transmit_power 값 및 유효 범위 확인
   */
  params.hdr_extensions.transmit_power = true;
  for(int32_t i = (kDot3Power_Min-1)/*INT32_MIN*/; i <= (kDot3Power_Max+1)/*INT32_MAX*/; i++) {
    params.transmit_power = i;
    // 정상범위
    if ((params.transmit_power >= kDot3Power_Min) && (params.transmit_power <= kDot3Power_Max)) {
      min_size_wsm_mpdu_with_tx_power[30] = 0x04;
      min_size_wsm_mpdu_with_tx_power[32] = (uint8_t)(params.transmit_power + 128);
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_tx_power));
      EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_tx_power, mpdu_size)));
    }
    // 비정상범위
    else {
      memset(outbuf, 0, sizeof(outbuf));
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InvalidPowerValue);
    }
  }
}

/*
 * 6) params->dst_mac_addr 값에 따라 정확한 결과 데이터를 반환해야 한다.
 *  - 브로드캐스트 주소일 경우 (dst_mac_addr[0]의 b0가 1), QoS Control 필드의 Ack 필드는 No Ack(01) 값을 가져야 한다. (0x20, 0x00)
 *  - 유니캐스트 주소일 경우 (dst_mac_addr[0]의 b0가 0), QoS Control 필드의 Ack 필드는 Ack(00) 값을 가져야 한다. (0x00, 0x00)
 */
TEST(Dot3_ConstructWsmMpdu, params_dst_mac_addr)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   *  dst_mac_addr 의 각 옥텟을 증가시키면서 확인
   */
  for (int i = 0; i < UINT8_MAX; i++) {
    params.dst_mac_addr[0] = i;
    params.dst_mac_addr[1] = i;
    params.dst_mac_addr[2] = i;
    params.dst_mac_addr[3] = i;
    params.dst_mac_addr[4] = i;
    params.dst_mac_addr[5] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[4] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[5] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[6] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[7] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[8] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[9] = i;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    if (params.dst_mac_addr[0] & 1) {  // broadcast
      min_size_wsm_mpdu_with_min_wsmp_hdr[24] = 0x20; // No Ack
    }
    else { // unicast
      min_size_wsm_mpdu_with_min_wsmp_hdr[24] = 0x00; // Ack
    }
    EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
    EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  }
}


/*
 * 7) params->src_mac_addr 값에 따라 정확한 결과 데이터를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, params_src_mac_addr)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   *  src_mac_addr 의 각 옥텟을 증가시키면서 확인
   */
  for (int i = 0; i < UINT8_MAX; i++) {
    params.src_mac_addr[0] = i;
    params.src_mac_addr[1] = i;
    params.src_mac_addr[2] = i;
    params.src_mac_addr[3] = i;
    params.src_mac_addr[4] = i;
    params.src_mac_addr[5] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[10] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[11] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[12] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[13] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[14] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[15] = i;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr));
    EXPECT_TRUE(!(memcmp(outbuf, min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  }
}


/*
 * 8) params->psid 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, params_psid)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  /*
   *  유효한 PSID 값
   *    - 1씩 증가하며 전 범위를 시험해야 하지만, 시간이 오래 소요되어 1111 씩 증가하며 시험한다.
   *    - PSID 값에 따라  WSM 부분은 달라지므로, 전체를 비교하지 않고 MAC+LLC 헤더만 비교한다.
   */
  for (uint32_t i = 0; i < kDot3Psid_Max; i += 1111/*i++*/) {
    params.psid = i;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_TRUE(mpdu_size > 0); // PSID 값에 따라 생성된 데이터의 길이가 달라지므로, 성공 여부만 확인한다.
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 유효하지 않은 PSID 값 (유효값 초과)
   */
  params.psid = kDot3Psid_Max + 1;
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InvalidPsidValue);
}


/*
 * 9) payload=NULL이거나 payload_size=0인 경우 정확한 결과데이터를 반환해야 한다.
 *  - pyaload = NULL 이거나 payload_size = 0 이면 WSM body 가 없는 WSM MPDU가 반환되어야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, payload_NULL_or_ZERO)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize] = {0};
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  /*
   *  payload = NULL, payload_size != 0 인 경우 -> 성공 (WSM body 길이 = 0)
   */
  for (uint16_t i = 1; i < kWsmBodyMaxSize; i++) {
    payload_size = i;
    memset(outbuf, 0, sizeof(outbuf));
    mpdu_size = Dot3_ConstructWsmMpdu(&params, NULL, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
  }

  /*
   *  payload != NULL, payload_size = 0 인 경우 -> 성공 (WSM body 길이 = 0)
   */
  payload_size = 0;
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));

  /*
   *  payload = NULL, payload_size = 0 인 경우 -> 성공 (WSM body 길이 = 0)
   */
  payload_size = 0;
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));
  EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, mpdu_size)));
}


/*
 * 10) payload_size 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, payload_size)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize] = {0};
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  /*
   * 최소길이 WSMP 헤더(WSMP 헤더의 확장필드 = NULL, PSID = 0, Length필드길이 = 1바이트) 상황에서 시험.
   * (페이로드 길이가 127 이하이면 WSMP헤더의 Length 필드는 1바이트가 된다)
   *
   * payload_size <= 127 인 경우 -> 성공.
   *  - 결과 MPDU길이 = MAC헤더길이 + LLC헤더길이 + WSMP최소헤더길이 + 페이로드길이
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.psid = 0;
  for (uint16_t i = 0; i <= 127; i++) {
    payload_size = i;
    memset(outbuf, 0, sizeof(outbuf));
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize + i);
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최소길이 WSMP 헤더(WSMP 헤더의 확장필드 = NULL, PSID = 0, Length필드길이 = 2바이트) 상황에서 시험.
   * (페이로드 길이가 127 보다 크면 WSMP헤더의 Length 필드는 2바이트가 된다)
   *
   * 127 < payload_size <= kWsmBodyMaxSize 인 경우 -> 성공.
   *  - 결과 MPDU길이 = MAC헤더길이 + LLC헤더길이 + (WSMP최소헤더길이 + 1) + 페이로드길이
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.psid = 0;
  for (uint16_t i = (127 + 1); i <= kWsmBodyMaxSize; i++) {
    payload_size = i;
    memset(outbuf, 0, sizeof(outbuf));
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMinSize + 1) + i);
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_min_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최소길이 WSMP 헤더(WSMP 헤더의 확장필드 = NULL, PSID = 0) 상황에서 시험.
   *
   * payload_size > kWsmBodyMaxSize 인 경우 -> 실패
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.psid = 0;
  payload_size = kWsmBodyMaxSize + 1;
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_TooLongWsm);

  /*
   * 최대길이 WSMP헤더(WSMP 헤더의 확장필드 = 모두존재, PSID = 최대값(kDot3Psid_Max), Length필드길이 = 1바이트) 상황에서 시험.
   * (페이로드 길이가 127 이하이면 WSMP헤더의 Length 필드는 1바이트가 된다)
   *
   * payload_size <= 127 인 경우 -> 성공.
   *  - 결과 MPDU길이 = MAC헤더길이 + LLC헤더길이 + (WSMP최대헤더길이-1) + 페이로드길이
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.psid = kDot3Psid_Max;
  for (uint16_t i = 0; i <= 127; i++) {
    payload_size = i;
    memset(outbuf, 0, sizeof(outbuf));
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMaxSize - 1) + i);
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_max_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최대길이 WSMP헤더(WSMP 헤더의 확장필드 = 모두존재, PSID = 최대값(kDot3Psid_Max), Length필드길이 = 2바이트) 상황에서 시험.
   * (페이로드 길이가 127 보다 크면 WSMP헤더의 Length 필드는 2바이트가 된다)
   *
   * 127 < payload_size <= kWsmBodySafeMaxSize 인 경우 -> 성공.
   *  - 결과 MPDU길이 = MAC헤더길이 + LLC헤더길이 + WSMP최대헤더길이 + 페이로드길이
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.psid = kDot3Psid_Max;
  for (uint16_t i = (127 + 1); i <= kWsmBodySafeMaxSize; i++) {
    payload_size = i;
    memset(outbuf, 0, sizeof(outbuf));
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
    EXPECT_EQ(mpdu_size, kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMaxSize + i);
    EXPECT_TRUE(!(memcmp(outbuf, g_min_size_wsm_mpdu_with_max_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최대길이 WSMP헤더(WSMP 헤더의 확장필드 = 모두존재, PSID = 최대값(kDot3Psid_Max)) 상황에서 시험.
   *
   * payload_size > kWsmBodySafeMaxSize 인 경우 -> 실패
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.psid = kDot3Psid_Max;
  payload_size = kWsmBodySafeMaxSize + 1;
  memset(outbuf, 0, sizeof(outbuf));
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_TooLongWsm);
}


/*
 * 11) outbuf=NULL이거나 outbuf_size=0인 경우, 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, outbuf_NULL_or_ZERO)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize] = {0};
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;

  /*
   *  outbuf = NULL, outbuf_size != 0 인 경우 -> 실패
   */
  for (uint16_t i = 1; i < kMpduMaxSize; i++) {
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, NULL, i);
    EXPECT_EQ(mpdu_size, -kDot3Result_Fail_NullParameters);
  }

  /*
   *  outbuf != NULL, outbuf_size = 0 인 경우 -> 실패
   */
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, 0);
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InsufficientBuf);

  /*
   *  outbuf = NULL, outbuf_size = 0 인 경우 -> 실패
   */
  mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, NULL, 0);
  EXPECT_EQ(mpdu_size, -kDot3Result_Fail_NullParameters);
}


/*
 * 12) payload_size 값과 outbuf_size 값에 따라 정확한 결과 데이터를 반환하거나 실패를 반환해야 한다.
 */
TEST(Dot3_ConstructWsmMpdu, payload_size_between_outbuf_size)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  uint8_t payload[kMpduMaxSize];
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize] = {0};
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 172;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_3Mbps;
  params.transmit_power = kDot3Power_Min;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = 0;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  int mpdu_size;
  uint16_t total_hdr_size, outbuf_size;

  /*
   * 최소길이 WSMP헤더(WSMP 헤더의 확장필드 = NULL, PSID = 0, Length필드길이 = 1바이트) 상황에서 시험.
   * (페이로드 길이가 127 이하이면 WSMP헤더의 Length 필드는 1바이트가 된다)
   *
   * 페이로드 길이가 127 이하일 때,
   * outbuf_size 는 payload_size 보다 최소 32바이트(kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize) 이상 커야 한다.
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.psid = 0;
  total_hdr_size = kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize;
  for (uint16_t i = 0; i <= 127; i++) {
    payload_size = i;
    // outbuf_size가 (총헤더길이 + payload_size) 보다 작으면 실패
    for (uint16_t j = 0; j < total_hdr_size + payload_size; j++) {
      outbuf_size = j;
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InsufficientBuf);
    }
    // outbuf_size가 (총헤더길이 + payload_size) 이상이면 성공
    memset(outbuf, 0, sizeof(outbuf));
    outbuf_size = total_hdr_size + payload_size;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
    EXPECT_EQ(mpdu_size, total_hdr_size + payload_size);
    EXPECT_TRUE(!(memcmp(outbuf, g_max_size_wsm_mpdu_with_min_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최소길이 WSMP헤더(WSMP 헤더의 확장필드 = NULL, PSID = 0, Length필드길이 = 2바이트) 상황에서 시험.
   * (페이로드 길이가 127 보다 크면 WSMP헤더의 Length 필드는 2바이트가 된다)
   *
   * 페이로드 길이가 127 보다 클때,
   * outbuf_size 는 payload_size 보다 최소 33바이트(kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMinSize + 1)) 이상 커야 한다.
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = false;
  params.psid = 0;
  total_hdr_size = kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMinSize + 1);
  for (uint16_t i = (127 + 1); i <= kWsmBodyMaxSize; i++) {
    payload_size = i;
    // outbuf_size가 (총헤더길이 + payload_size) 보다 작으면 실패
    for (uint16_t j = 0; j < total_hdr_size + payload_size; j++) {
      outbuf_size = j;
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InsufficientBuf);
    }
    // outbuf_size가 (총헤더길이 + payload_size) 이상이면 성공
    memset(outbuf, 0, sizeof(outbuf));
    outbuf_size = total_hdr_size + payload_size;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
    EXPECT_EQ(mpdu_size, total_hdr_size + payload_size);
    EXPECT_TRUE(!(memcmp(outbuf, g_max_size_wsm_mpdu_with_min_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }


  /*
   * 최대길이 WSMP헤더(WSMP 헤더의 확장필드 = 모두존재, PSID = 최대값(kDot3Psid_Max), Length필드길이 = 1바이트) 상황에서 시험.
   * (페이로드 길이가 127 이하이면 WSMP헤더의 Length 필드는 1바이트가 된다)
   *
   * 페이로드 길이가 127 이하일때,
   * outbuf_size는 payload_size보다 최소 45바이트(kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMaxSize - 1)) 이상 커야 한다.
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.psid = kDot3Psid_Max;
  total_hdr_size = kQoSMacHdrSize + kLLCHdrSize + (kWsmpHdrMaxSize - 1);
  for (uint16_t i = 0; i <= 127; i++) {
    payload_size = i;
    // outbuf_size가 (총헤더길이 + payload_size) 보다 작으면 실패
    for (uint16_t j = 0; j < total_hdr_size + payload_size; j++) {
      outbuf_size = j;
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InsufficientBuf);
    }
    // outbuf_size가 (총헤더길이 + payload_size) 이상이면 성공
    memset(outbuf, 0, sizeof(outbuf));
    outbuf_size = total_hdr_size + payload_size;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
    EXPECT_EQ(mpdu_size, total_hdr_size + payload_size);
    EXPECT_TRUE(!(memcmp(outbuf, g_max_size_wsm_mpdu_with_max_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }

  /*
   * 최대길이 WSMP헤더(WSMP 헤더의 확장필드 = 모두존재, PSID = 최대값(kDot3Psid_Max), Length필드길이 = 2바이트) 상황에서 시험.
   * (페이로드 길이가 127 보다 크면 WSMP헤더의 Length 필드는 2바이트가 된다)
   *
   * 페이로드 길이가 127 보다 클때,
   * outbuf_size는 payload_size보다 최소 46바이트(kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMaxSize) 이상 커야 한다.
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.psid = kDot3Psid_Max;
  total_hdr_size = kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMaxSize;
  for (uint16_t i = (127 + 1); i <= kWsmBodySafeMaxSize; i++) {
    payload_size = i;
    // outbuf_size가 (총헤더길이 + payload_size) 보다 작으면 실패
    for (uint16_t j = 0; j < total_hdr_size + payload_size; j++) {
      outbuf_size = j;
      mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
      EXPECT_EQ(mpdu_size, -kDot3Result_Fail_InsufficientBuf);
    }
    // outbuf_size가 (총헤더길이 + payload_size) 이상이면 성공
    memset(outbuf, 0, sizeof(outbuf));
    outbuf_size = total_hdr_size + payload_size;
    mpdu_size = Dot3_ConstructWsmMpdu(&params, payload, payload_size, outbuf, outbuf_size);
    EXPECT_EQ(mpdu_size, total_hdr_size + payload_size);
    EXPECT_TRUE(!(memcmp(outbuf, g_max_size_wsm_mpdu_with_max_wsmp_hdr, kQoSMacHdrSize + kLLCHdrSize)));
  }
}
