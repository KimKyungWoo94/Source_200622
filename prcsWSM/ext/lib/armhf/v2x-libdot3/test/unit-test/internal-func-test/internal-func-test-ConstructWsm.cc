
/**
 * @file internal-func-test-ConstructWsm.cc
 * @date 2019-07-11
 * @author gyun
 * @brief dot3_ConstructWsm() 함수에 대한 단위테스트
 *
 * 본 파일은 dot3 라이브러리 내부 함수 중, dot3_ConstructWsm() 함수에 대한 단위 테스트를 수행하는 테스트코드를 포함한다.
 * API 파라미터(WSM 송신파라미터, 페이로드, 출력버퍼 등) 값에 대한 단위테스트는 검사하지 않는다. (이는 API 단위테스트에서 수행된다)
 * 여기서는 API 파라미터는 유효하다고 가정하고, WSM 인코딩 결과에 대한 단위테스트만을 수행한다.
 *
 * 시험데이터 및 기대값은 https://asn1.io/asn1playground/ 에서 획득하였다.
 */

#include <stdio.h>  // for printf()

#include "gtest/gtest.h"

#include "dot3/dot3.h"
#include "dot3-internal.h"

static const uint8_t bcast_addr[kDot3MacAddrSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t my_addr[kDot3MacAddrSize] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};


/*
 * Test case
 *  1) WSMP-N-Header 필수필드 인코딩 데이터 유효성
 *  2) WSMP-N-Header ChannelNumber 옵션필드 인코딩 데이터 유효성
 *  3) WSMP-N-Header DataRate 옵션필드 인코딩 데이터 유효성
 *  4) WSMP-N-Header TransmitPowerUsed 옵션필드 인코딩 데이터 유효성
 *  5) WSMP-N-Header 모든 옵션필드 포함 인코딩 데이터 유효성
 *  6) WSMP-T-Header PSID 필드 인코딩 데이터 유효성
 *  7) 최소길이 WSM body 인코딩 데이터 유효성
 *  8) 최소길이(4) 헤더일 때, 최대길이(2297) WSM body 인코딩 데이터 유효성
 *  9) 최대길이(18) 헤더일 때, 최대길이(2284) WSM body 인코딩 데이터 유효성
 */

/*
 *  1) WSMP-N-Header 필수필드 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSMP_N_HDR_MANDATORY)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
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
  uint8_t expected_output[4] = {0x03, 0x00, 0x00, 0x00};

  /*
   * WSMP-N-Header 필수필드 테스트
   */
  int encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));
}


/*
 *  2) WSMP-N-Header ChannelNumber 옵션필드 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 15 value ChannelNumber80211:172 } --0~255
        }
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSMP_N_HDR_EXT_CHAN_NUM)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = true;
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
  uint8_t expected_output[8] = {0x0B,0x01,0x0F,0x01,0xAC,0x00,0x00,0x00};

  int encoded_size;

  /*
   * ChannelNumber 확장필드 최소값(0) 테스트
   */
  params.chan_num = 0;
  expected_output[4] = 0x00;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

  /*
   * ChannelNumber 확장필드 최대값(255) 테스트
   */
  params.chan_num = 255;
  expected_output[4] = 0xFF;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

  /*
   * 아래 테스트케이스는 ffasn1c 라이브러리를 사용하는 경우에만 테스트한다.
   *  - ffasn1c 는 ChannelNumber 변수형으로 int 를 사용하므로 무효값으로 인코딩 시도가 가능하다.
   *  - objasn1c 는 ChannelNumber 변수형으로 uint8_t 을 사용하므로 무효값으로 인코딩 시도가 불가능하다(wrap around).
   */
#if defined(FFASN1C_)
  /*
   *  ChannelNumber 확장필드 무효값(음수) 테스트
   */
  params.chan_num = -1;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);

  /*
   *  ChannelNumber 확장필드 무효값(큰 값) 테스트
   */
  params.chan_num = 256;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);
#endif
}


/*
 *  3) WSMP-N-Header DataRate 옵션필드 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 16 value DataRate80211:6 }  --0~255
        }
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSMP_N_HDR_EXT_DATARATE)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = true;
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
  uint8_t expected_output[8] = {0x0B,0x01,0x10,0x01,0x06,0x00,0x00,0x00};

  int encoded_size;

  /*
   * DataRate 확장필드 최소값(0) 테스트
   */
  params.datarate = 0;
  expected_output[4] = 0x00;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

  /*
   * DataRate 확장필드 최대값(255) 테스트
   */
  params.datarate = 255;
  expected_output[4] = 0xFF;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

/*
 * 아래 테스트케이스는 ffasn1c 라이브러리를 사용하는 경우에만 테스트한다.
 *  - ffasn1c 는 DataRate 변수형으로 int 를 사용하므로 무효값으로 인코딩 시도가 가능하다.
 *  - objasn1c 는 DataRate 변수형으로 uint8_t 을 사용하므로 무효값으로 인코딩 시도가 불가능하다(wrap around).
 */
#if defined(FFASN1C_)
  /*
   *  DataRate 확장필드 무효값(음수) 테스트
   */
  params.datarate = -1;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);

  /*
   *  DataRate 확장필드 무효값(큰 값) 테스트
   */
  params.datarate = 256;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);
#endif
}


/*
 *  4) WSMP-N-Header TransmitPowerUsed 옵션필드 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 4 value TXpower80211:-128 } -- -128~127, Converted automatically.
        }
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSMP_N_HDR_EXT_TX_POWER_USED)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = true;
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
  uint8_t expected_output[8] = {0x0B,0x01,0x04,0x01,0x00,0x00,0x00,0x00};

  int encoded_size;

  /*
   * TransmitPowerUsed 확장필드 최소값(-128) 테스트
   */
  params.transmit_power = -128;
  expected_output[4] = 0x00;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

  /*
   * TransmitPowerUsed 확장필드 최대값(127) 테스트
   */
  params.transmit_power = 127;
  expected_output[4] = 0xFF;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output));
  EXPECT_TRUE(!memcmp(outbuf, expected_output, encoded_size));

/*
 * 아래 테스트케이스는 ffasn1c 라이브러리를 사용하는 경우에만 테스트한다.
 *  - ffasn1c 는 TransmitPowerUsed 변수형으로 int 를 사용하므로 무효값으로 인코딩 시도가 가능하다.
 *  - objasn1c 는 TransmitPowerUsed 변수형으로 uint8_t 을 사용하므로 무효값으로 인코딩 시도가 불가능하다(wrap around).
 */
#if defined(FFASN1C_)
  /*
   *  TransmitPowerUsed 확장필드 무효값(작은 값) 테스트
   */
  params.transmit_power = -129;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);

  /*
   *  TransmitPowerUsed 확장필드 무효값(큰 값) 테스트
   */
  params.transmit_power = 128;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_Asn1Encode);
#endif
}


/*
 *  5) WSMP-N-Header 모든 옵션필드 포함 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 15 value ChannelNumber80211:172 }
          { extensionId 16 value DataRate80211:6 }
          { extensionId 4 value TXpower80211:-128 } -- Converted automatically.
        }
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSMP_N_HDR_EXT_MULTI)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
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

  int encoded_size;

  /*
   * ChannelNumber, DataRate 확장필드 포함 테스트
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = false;
  uint8_t expected_output1[] = {0x0B,0x02,0x0F,0x01,0xAC,0x10,0x01,0x06,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output1));
  EXPECT_TRUE(!memcmp(outbuf, expected_output1, encoded_size));

  /*
   * ChannelNumber, TxPowerUsed 확장필드 포함 테스트
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = false;
  params.hdr_extensions.transmit_power = true;
  uint8_t expected_output2[] = {0x0B,0x02,0x0F,0x01,0xAC,0x04,0x01,0x00,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output2));
  EXPECT_TRUE(!memcmp(outbuf, expected_output2, encoded_size));

  /*
   * DataRate, TxPowerUsed 확장필드 포함 테스트
   */
  params.hdr_extensions.chan_num = false;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  uint8_t expected_output3[] = {0x0B,0x02,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output3));
  EXPECT_TRUE(!memcmp(outbuf, expected_output3, encoded_size));

  /*
   * ChannelNumber, DataRate, TxPowerUsed 확장필드 포함 테스트
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  uint8_t expected_output4[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output4));
  EXPECT_TRUE(!memcmp(outbuf, expected_output4, encoded_size));

  /*
   * ChannelNumber, DataRate, TxPowerUsed 확장필드 포함 테스트 - 표준문서 샘플 비교
   */
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.chan_num = 172;
  params.datarate = kDot3DataRate_6Mbps;
  params.transmit_power = 30;
  uint8_t expected_output5[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x0C,0x04,0x01,0x9E,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output5));
  EXPECT_TRUE(!memcmp(outbuf, expected_output5, encoded_size));
}


/*
 *
 *  6) WSMP-T-Header PSID 필드 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 15 value ChannelNumber80211:172 }
          { extensionId 16 value DataRate80211:6 }
          { extensionId 4 value TXpower80211:-128 } -- Converted automatically.
        }
      }
      transport bcMode {
        destAddress content 0
        -- destAddress extension extension extension 270549119
      }
      body ''H
    }
 *
 *  - 현재 표준에 따르면, WSMP-T-Header는 확장 필드가 없으므로 기본필드인 Psid에 대해서만 테스트한다.
 *  - Wsm Length는 원래 WSMP-T-Header에 포함되어 있지만, WSM Body 테스트에서 별개로 테스트한다.
 */
TEST(dot3_ConstructWsm, WSMP_T_HDR_PSID)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
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

  int encoded_size;

  /*
   * PSID 최소값(0) 테스트
   */
  params.psid = 0;
  uint8_t expected_output1[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output1));
  EXPECT_TRUE(!memcmp(outbuf, expected_output1, encoded_size));

  /*
   * PSID 최대값(270549119) 테스트
   */
  params.psid = 270549119;
  uint8_t expected_output2[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0xEF,0xFF,0xFF,0xFF,0x00};
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(expected_output2));
  EXPECT_TRUE(!memcmp(outbuf, expected_output2, encoded_size));

  /*
   * PSID 무효값(큰 값) 테스트
   */
  params.psid = 270549119 + 1;
  encoded_size = dot3_ConstructWsm(&params, NULL, payload_size, outbuf, sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_InvalidPsidValue);
}


/*
 * 7) 최소길이 WSM body 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
      subtype nullNetworking {
        version 3
         nExtensions {
          { extensionId 15 value ChannelNumber80211:172 }
          { extensionId 16 value DataRate80211:6 }
          { extensionId 4 value TXpower80211:-128 } -- Converted automatically.
        }
      }
      transport bcMode {
        destAddress content 0
      }
      body ''H
    }
 */
TEST(dot3_ConstructWsm, WSM_BODY_LEN_MIN)
{
  /*
   * 생략 : 위에서 수행된 단위테스트들이 모두 WSM body 최소길이에 대해 수행되었다.
   */
}


/*
 *  8) 최소길이(4) 헤더일 때, 최대길이(2297) WSM body 인코딩 데이터 유효성
 *
 *  msg ShortMsgNpdu ::= {
       subtype nullNetworking {
         version 3
       }
       transport bcMode {
         destAddress content 0
       }
       body '00010203040506....' -- len: 2297
    }
 */
extern uint8_t g_encoded_wsm1[kWsmMaxSize];
extern uint8_t g_payload1[kWsmBodyMaxSize];
TEST(dot3_ConstructWsm, WSM_BODY_LEN_MAX_WITH_MIN_HDR)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
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

  int encoded_size;

  /*
   * 최대길이(2297) WSM body 테스트
   */
  payload_size = sizeof(g_payload1);
  encoded_size = dot3_ConstructWsm(&params,
                              g_payload1,
                              payload_size,
                              outbuf,
                              sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(g_encoded_wsm1));
  EXPECT_TRUE(!memcmp(outbuf, g_encoded_wsm1, encoded_size));

  /*
   * 유효하지 않은 길이(2298 이상)의 WSM body 테스트
   */
  payload_size = kWsmBodyMaxSize + 1;
  encoded_size = dot3_ConstructWsm(&params,
                              g_payload1,
                              payload_size,
                              outbuf,
                              sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_TooLongWsm);
}


/*
 *  8) 최대길이(18) 헤더일 때, 최대길이(2284) WSM body 인코딩 데이터 유효성
 *
   * msg ShortMsgNpdu ::= {
       subtype nullNetworking {
         version 3
         nExtensions {
           { extensionId 15 value ChannelNumber80211:184 }
           { extensionId 16 value DataRate80211:54 }
           { extensionId 4 value TXpower80211:127 } -- Converted automatically.
         }
       }
       transport bcMode {
         destAddress extension extension extension 270549119
       }
       body '00010203040506....' -- len: 2284
     }
 */
extern uint8_t g_encoded_wsm2[kWsmMaxSize];
extern uint8_t g_payload2[kWsmBodySafeMaxSize];
TEST(dot3_ConstructWsm, WSM_BODY_LEN_MAX_WITH_MAX_HDR)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  Dot3PduSize payload_size = 0;
  struct Dot3WsmMpduTxParams params;
  uint8_t outbuf[kMpduMaxSize];
  memset(&params, 0, sizeof(params));
  params.hdr_extensions.chan_num = true;
  params.hdr_extensions.datarate = true;
  params.hdr_extensions.transmit_power = true;
  params.hdr_extensions.chan_load = false;
  params.ifindex = 0;
  params.chan_num = 184;
  params.timeslot = kDot3TimeSlot_0;
  params.datarate = kDot3DataRate_27Mbps;
  params.transmit_power = kDot3Power_Max;
  params.chan_load = 0;
  params.priority = kDot3Priority_Min;
  params.expiry = 0;
  memcpy(params.dst_mac_addr, bcast_addr, sizeof(params.dst_mac_addr));
  params.psid = kDot3Psid_Max;
  memcpy(params.src_mac_addr, my_addr, sizeof(params.src_mac_addr));

  /*
   * 최대길이(2284) WSM body 테스트
   */
  payload_size = sizeof(g_payload2);
  int encoded_size = dot3_ConstructWsm(&params,
                                  g_payload2,
                                  payload_size,
                                  outbuf,
                                  sizeof(outbuf));
  EXPECT_EQ(encoded_size, sizeof(g_encoded_wsm2));
  EXPECT_TRUE(!memcmp(outbuf, g_encoded_wsm2, encoded_size));

  /*
   * 유효하지 않은 길이(2285 이상)의 WSM body 테스트
   */
  payload_size = kWsmBodySafeMaxSize + 1;
  encoded_size = dot3_ConstructWsm(&params,
                              g_payload1,
                              payload_size,
                              outbuf,
                              sizeof(outbuf));
  EXPECT_EQ(encoded_size, -kDot3Result_Fail_TooLongWsm);
}
