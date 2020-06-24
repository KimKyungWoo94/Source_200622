
/**
 * @file api-test-Dot3_ParseWsmMpdu.cc
 * @date 2019-08-09
 * @author gyun
 * @brief Dot3_ParseWsmMpdu() Open API에 대한 단위테스트
 *
 * 본 파일은 Dot3_ParseWsmMpdu() Open API에 대한 단위테스트를 수행한다.
 * 파라미터 유효성, 크기, 결과값 등에 대해 테스트한다.
 * MPDU 내 WSM 데이터의 파싱에 대해서는 내부함수 단위테스트에서 수행하므로, 여기서는 수행하지 않는다.
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
extern uint8_t g_min_size_wsm_mpdu_with_max_wsmp_hdr[kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMaxSize-1];
extern uint8_t g_max_size_wsm_mpdu_with_min_wsmp_hdr[kMpduMaxSize];
extern uint8_t g_max_size_wsm_mpdu_with_max_wsmp_hdr[kMpduMaxSize];

/*
 * Test case
 *  1) NULL 파라미터(mpdu, outbuf, params, wsr_registered)에 따른 동작 확인
 *  2) mpdu_size 값 범위에 따른 동작 확인
 *  3) outbuf_size 에 따른 동작 확인 (0, 페이로드길이 대비 짧은 크기)
 *  4) MAC 헤더 버전에 따른 동작 확인
 *  5) MAC 헤더 프레임 유형에 따른 동작 확인
 *  6) MAC 헤더 ADDR1 필드값에 따른 동작 확인
 *  7) MAC 헤더 ADDR2 필드값에 따른 동작 확인
 *  8) MAC 헤더 ADDR3 필드값에 따른 동작 확인
 *  9) MAC 헤더 priority 에 따른 동작 확인
 *  10) LLC 헤더 EtherType 에 따른 동작 확인
 *  11) WSM 정보에 따른 params 값, 페이로드 확인
 */


/*
 * 1) NULL 파라미터(mpdu, outbuf, params, wsr_registered)에 따른 동작 확인
 *  - NULL 파라미터 전달 시 실패를 반환해야 한다.
 */
TEST(Dot3_ParseWsmMpdu, params_NULL)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params;
  uint8_t mpdu[kMpduMaxSize];
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;

  int payload_size;

  /*
   * mpdu = NULL 케이스
   */
  payload_size = Dot3_ParseWsmMpdu(NULL, sizeof(mpdu), outbuf, sizeof(outbuf), &params, &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_NullParameters);

  /*
   * outbuf = NULL 케이스
   */
  payload_size = Dot3_ParseWsmMpdu(mpdu, sizeof(mpdu), NULL, sizeof(outbuf), &params, &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_NullParameters);

  /*
   * params = NULL 케이스
   */
  payload_size = Dot3_ParseWsmMpdu(mpdu, sizeof(mpdu), outbuf, sizeof(outbuf), NULL, &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_NullParameters);

  /*
   * wsr_registered = NULL 케이스
   */
  payload_size = Dot3_ParseWsmMpdu(mpdu, sizeof(mpdu), outbuf, sizeof(outbuf), &params, NULL);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_NullParameters);
}


/*
 * 2) mpdu_size 값 범위에 따른 동작 확인
 *  - 최소길이/최대길이 WSM MPDU 에 대한 동작을 확인한다.
 *  - 유효하지 않은 길이의 WSM MPDU 에 대한 동작을 확인한다.
 */
TEST(Dot3_ParseWsmMpdu, mpdu_size)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t mpdu[kMpduMaxSize+100];
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  /*
   * 최소길이 WSM 헤더를 갖는 최소길이 WSM MPDU
   *  - WSM 헤더길이 : 4
   *  - WSM 바디길이 : 0
   *  - WSM 길이 : 4
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_min_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 최대길이 WSM 헤더를 갖는 최소길이 WSM MPDU
   *  - WSM 헤더길이 : 17
   *  - WSM 바디길이 : 0
   *  - WSM 길이 : 17
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_6Mbps;
  expected.tx_power = 30;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 270549119;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_min_size_wsm_mpdu_with_max_wsmp_hdr,
                                   sizeof(g_min_size_wsm_mpdu_with_max_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
#if 0
  printf("ifindex: %u, version: %u, tx_chan_num: %d, rx_chan_num: %d\n",
           params.ifindex,
           params.version,
           params.tx_chan_num,
           params.rx_chan_num);
    printf("tx_datarate: %d, rx_datarate: %d, tx_power: %d, rx_power: %d, rcpi: %d, chan_load: %u\n",
           params.tx_datarate,
           params.rx_datarate,
           params.tx_power,
           params.rx_power,
           params.rcpi,
           params.chan_load);
    printf("priority: %u, src: %02X:%02X:%02X:%02X:%02X:%02X, dst: %02X:%02X:%02X:%02X:%02X:%02X, psid: %u\n",
           params.priority,
           params.src_mac_addr[0],
           params.src_mac_addr[1],
           params.src_mac_addr[2],
           params.src_mac_addr[3],
           params.src_mac_addr[4],
           params.src_mac_addr[5],
           params.dst_mac_addr[0],
           params.dst_mac_addr[1],
           params.dst_mac_addr[2],
           params.dst_mac_addr[3],
           params.dst_mac_addr[4],
           params.dst_mac_addr[5],
           params.psid);
#endif

  /*
   * 최소길이 WSM 헤더를 갖는 최대길이 WSM MPDU
   *  - WSM 헤더길이 : 5
   *  - WSM 바디길이 : 2297
   *  - WSM 길이 : 2302
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, kWsmBodyMaxSize);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 최대길이 WSM 헤더를 갖는 최대길이 WSM MPDU
   *  - WSM 헤더길이 : 18
   *  - WSM 바디길이 : 2284
   *  - WSM 길이 : 2302
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_6Mbps;
  expected.tx_power = 30;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 270549119;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_max_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_max_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, kWsmBodySafeMaxSize);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 유효하지 않은 mpdu_size (작은 값)
   */
  for (int i = 0; i < kWsmMpduMinSize; i++) {
    payload_size = Dot3_ParseWsmMpdu(mpdu, i, outbuf, sizeof(outbuf), &params, &wsr_registered);
    EXPECT_EQ(payload_size, -kDot3Result_Fail_TooShortMpdu);
  }

  /*
   * 유효하지 않은 mpdu_size (큰 값)
   */
  payload_size = Dot3_ParseWsmMpdu(mpdu, kMpduMaxSize + 1, outbuf, sizeof(outbuf), &params, &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_TooLongMpdu);
}


/*
 * 3) outbuf_size 에 따른 동작 확인
 *  - outbuf_size 가 페이로드(WSM body) 길이 이상일 경우 성공
 *  - outbuf_size 가 페이로드(WSM body) 길이보다 작을 경우 실패
 */
TEST(Dot3_ParseWsmMpdu, outbuf_size)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  /*
   * 최소길이 WSM 헤더를 갖는 최소길이 WSM MPDU
   *  - WSM 헤더길이 : 4
   *  - WSM 바디길이 : 0
   *  - WSM 길이 : 4
   *
   * outbuf_size = 0 이어도 성공.
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_min_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   0,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 최대길이 WSM 헤더를 갖는 최소길이 WSM MPDU
   *  - WSM 헤더길이 : 17
   *  - WSM 바디길이 : 0
   *  - WSM 길이 : 17
   *
   * outbuf_size = 0 이어도 성공.
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_6Mbps;
  expected.tx_power = 30;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 270549119;
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_min_size_wsm_mpdu_with_max_wsmp_hdr,
                                   sizeof(g_min_size_wsm_mpdu_with_max_wsmp_hdr),
                                   outbuf,
                                   0,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 최소길이 WSM 헤더를 갖는 최대길이 WSM MPDU
   *  - WSM 헤더길이 : 5
   *  - WSM 바디길이 : 2297
   *  - WSM 길이 : 2302
   *
   * - outbuf_size 가 WSM 바디길이 이상이면 성공
   * - outbuf_size 가 WSM 바디길이보다 작으면 실패
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;
  // outbuf_size 가 WSM 바디길이 이상이면 성공
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   kWsmBodyMaxSize,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, kWsmBodyMaxSize);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  // outbuf_size 가 WSM 바디길이보다 작으면 실패
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   kWsmBodyMaxSize - 1,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_InsufficientBuf);

  /*
   * 최대길이 WSM 헤더를 갖는 최대길이 WSM MPDU
   *  - WSM 헤더길이 : 18
   *  - WSM 바디길이 : 2284
   *  - WSM 길이 : 2302
   *
   * - outbuf_size 가 WSM 바디길이 이상이면 성공
   * - outbuf_size 가 WSM 바디길이보다 작으면 실패
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_6Mbps;
  expected.tx_power = 30;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 270549119;
  // outbuf_size 가 WSM 바디길이 이상이면 성공
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_max_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_max_wsmp_hdr),
                                   outbuf,
                                   kWsmBodySafeMaxSize,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, kWsmBodySafeMaxSize);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  // outbuf_size 가 WSM 바디길이보다 작으면 실패
  payload_size = Dot3_ParseWsmMpdu(g_max_size_wsm_mpdu_with_max_wsmp_hdr,
                                   sizeof(g_max_size_wsm_mpdu_with_max_wsmp_hdr),
                                   outbuf,
                                   kWsmBodySafeMaxSize-1,
                                   &params,
                                   &wsr_registered);
  EXPECT_EQ(payload_size, -kDot3Result_Fail_InsufficientBuf);
}


/*
 * 4) MAC 헤더 버전에 따른 동작 확인
 *  - 802.11 MAC 헤더의 현재 버전은 0이며, 그 외의 값일 경우 실패를 반환해야 한다.
 */
TEST(Dot3_ParseWsmMpdu, mac_protocol_version)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * 버전값 별로 테스트
   */
  for (int i = 0; i < 3; i++) {
    // 입력값 (버전 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] &= ~(3);
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] |= (i & 3);
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    if (i == 0) { // 정상
      EXPECT_EQ(payload_size, 0);
      EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
    } else {  // 비정상
      EXPECT_EQ(payload_size, -kDot3Result_Fail_InvalidLowerLayerProtocolVersion);
    }
  }
}


/*
 * 5) MAC 헤더 프레임 유형에 따른 동작 확인
 *  - WSM 을 수납한 MPDU의 MAC 헤더는 ftype = Data(2), fstype = QoS Data(8) 여야 한다.
 *  - MAC 헤더의 ftype 필드는 Frame Control 필드의 b2~3 이다.
 *  - MAC 헤더의 fstype 필드는 Frame Control 필드의 b4~7 이다.
 */
TEST(Dot3_ParseWsmMpdu, mac_frame_type)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * ftype 값 별로 테스트
   */
  for (int i = 0; i < 3; i++) {
    // 입력값 (ftype 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] &= ~(3 << 2);
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] |= ((i & 3) << 2);
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    if (i == 2) { // 정상
      EXPECT_EQ(payload_size, 0);
      EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
    } else {  // 비정상
      EXPECT_EQ(payload_size, -kDot3Result_Fail_InvalidLowerLayerFrameType);
    }
  }

  /*
   * fstype 값 별로 테스트
   */
  for (int i = 0; i < 15; i++) {
    // 입력값 (fstype 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] &= ~(0xf << 4);
    min_size_wsm_mpdu_with_min_wsmp_hdr[0] |= ((i & 0xf) << 4);
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    if (i == 8) { // 정상
      EXPECT_EQ(payload_size, 0);
      EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
    } else {  // 비정상
      EXPECT_EQ(payload_size, -kDot3Result_Fail_InvalidLowerLayerFrameType);
    }
  }
}


/*
 * 6) MAC 헤더 ADDR1 필드값에 따른 동작 확인
 *  - MAC 헤더에 수납된 ADDR2 가 dst_mac_addr에 제대로 반환되는지 확인한다.
 */
TEST(Dot3_ParseWsmMpdu, mac_addr1)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * ADDR1의 각 바이트 값을 증가시키며 테스트
   */
  for (uint8_t i = 0; i < 255; i++) {
    // 입력값 (ADDR1 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[4] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[5] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[6] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[7] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[8] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[9] = i;
    // 기대값
    expected.dst_mac_addr[0] = i;
    expected.dst_mac_addr[1] = i;
    expected.dst_mac_addr[2] = i;
    expected.dst_mac_addr[3] = i;
    expected.dst_mac_addr[4] = i;
    expected.dst_mac_addr[5] = i;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    EXPECT_EQ(payload_size, 0);
    EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  }
}


/*
 * 7) MAC 헤더 ADDR2 필드값에 따른 동작 확인
 *  - MAC 헤더에 수납된 ADDR2 가 src_mac_addr에 제대로 반환되는지 확인한다.
 */
TEST(Dot3_ParseWsmMpdu, mac_addr2)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * ADDR2의 각 옥텟 값을 증가시키며 테스트
   */
  for (uint8_t i = 0; i < 255; i++) {
    // 입력값 (ADDR2 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[10] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[11] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[12] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[13] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[14] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[15] = i;
    // 기대값
    expected.src_mac_addr[0] = i;
    expected.src_mac_addr[1] = i;
    expected.src_mac_addr[2] = i;
    expected.src_mac_addr[3] = i;
    expected.src_mac_addr[4] = i;
    expected.src_mac_addr[5] = i;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    EXPECT_EQ(payload_size, 0);
    EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  }
}


/*
 * 8) MAC 헤더 ADDR3 필드값에 따른 동작 확인
 *  - ADDR != wildcard BSSID 이면 실패를 반환해야 한다.
 */
TEST(Dot3_ParseWsmMpdu, mac_addr3)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * ADDR3 = wildcard BSSID 이면 성공을 반환해야 한다.
   */
  // 입력값 (ADDR3)
  min_size_wsm_mpdu_with_min_wsmp_hdr[16] = 0xff;
  min_size_wsm_mpdu_with_min_wsmp_hdr[17] = 0xff;
  min_size_wsm_mpdu_with_min_wsmp_hdr[18] = 0xff;
  min_size_wsm_mpdu_with_min_wsmp_hdr[19] = 0xff;
  min_size_wsm_mpdu_with_min_wsmp_hdr[20] = 0xff;
  min_size_wsm_mpdu_with_min_wsmp_hdr[21] = 0xff;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  // 결과비교
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * ADDR3 != wildcard BSSID 이면 실패를 반환해야 한다.
   *  - 각 옥텟의 값을 증가시켜가며 테스트한다.
   */
  for (uint8_t i = 0; i < 254; i++) {
    // 입력값 (ADDR3 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[16] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[17] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[18] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[19] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[20] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[21] = i;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    EXPECT_EQ(payload_size, -kDot3Result_Fail_NotWildcardBssid);
  }
}


/*
 * 9) MAC 헤더 priority 에 따른 동작 확인
 *  - MAC 헤더에 수납된 priority 값이 제대로 반환되는지 확인한다.
 */
TEST(Dot3_ParseWsmMpdu, mac_priority)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  // priority 값 별로 테스트
  for (uint8_t i = 0; i < 8; i++) {
    // 입력값 (priority 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[24] &= ~(0xf);
    min_size_wsm_mpdu_with_min_wsmp_hdr[24] |= (i & 0xf);
    // 기대값
    expected.priority = (i & 0xf);
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    EXPECT_EQ(payload_size, 0);
    EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  }
}

/*
 * 10) LLC 헤더 EtherType 에 따른 동작 확인
 *  - EtherType != WSMP(0x88DC) 이면 실패를 반환해야 한다.
 */
TEST(Dot3_ParseWsmMpdu, llc_ethertype)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  bool wsr_registered;
  int payload_size;

  uint8_t min_size_wsm_mpdu_with_min_wsmp_hdr[kQoSMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize];
  memcpy(min_size_wsm_mpdu_with_min_wsmp_hdr,
         g_min_size_wsm_mpdu_with_min_wsmp_hdr,
         sizeof(g_min_size_wsm_mpdu_with_min_wsmp_hdr));

  /*
   * 공통 기대값
   */
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.priority = 0;
  memcpy(expected.src_mac_addr, my_addr, kDot3MacAddrSize);
  memcpy(expected.dst_mac_addr, bcast_addr, kDot3MacAddrSize);
  expected.psid = 0;

  /*
   * 정상 EtherType 값 테스트 (0x88DC)
   */
  // 입력값 (EtherType 수정 적용)
  min_size_wsm_mpdu_with_min_wsmp_hdr[26] = 0x88;
  min_size_wsm_mpdu_with_min_wsmp_hdr[27] = 0xDC;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                   sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                   outbuf,
                                   sizeof(outbuf),
                                   &params,
                                   &wsr_registered);
  // 결과비교
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 비정상 EtherType 값 테스트
   *  - EtherType 의 두 옥텟을 증가시켜가며 테스트한다.
   *    (두 옥텟이 동시에 증가하기 때문에 0x88DC 는 될 수 없다)
   */
  for (uint8_t i = 0; i < 255; i++) {
    // 입력값 (EtherType 수정 적용)
    min_size_wsm_mpdu_with_min_wsmp_hdr[26] = i;
    min_size_wsm_mpdu_with_min_wsmp_hdr[27] = i;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = Dot3_ParseWsmMpdu(min_size_wsm_mpdu_with_min_wsmp_hdr,
                                     sizeof(min_size_wsm_mpdu_with_min_wsmp_hdr),
                                     outbuf,
                                     sizeof(outbuf),
                                     &params,
                                     &wsr_registered);
    // 결과비교
    EXPECT_EQ(payload_size, -kDot3Result_Fail_NotSupportedEtherType);
  }
}


/*
 * 11) WSM 정보에 따른 params 값, 페이로드 확인
 *  - WSM MPDU 에 수납되어 있는 정보가 제대로 반환되는지 확인한다.
 */
TEST(Dot3_ParseWsmMpdu, wsm_data)
{
  /*
   * "2) mpdu_size 값 범위에 따른 동작 확인" 과 시나리오가 동일하여 생략
   */
}
