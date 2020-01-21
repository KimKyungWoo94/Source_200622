
/**
 * @file internal-func-test-ParseWsm.cc
 * @date 2019-08-09
 * @author gyun
 * @brief dot3_ParseWsm() 함수에 대한 단위테스트
 *
 * 본 파일은 dot3 라이브러리 내부 함수 중, dot3_ParseWsm() 함수에 대한 단위 테스트를 수행하는 테스트코드를 포함한다.
 * API 파라미터(MPDU, 출력버퍼, WSM 수신파라미터 등) 값에 대한 단위테스트는 검사하지 않는다. (이는 API 단위테스트에서 수행된다)
 * 여기서는 API 파라미터는 유효하다고 가정하고, WSM 디코딩 결과에 대한 단위테스트만을 수행한다.
 *
 * 시험데이터 및 기대값은 https://asn1.io/asn1playground/ 에서 획득하였다.
 */

#include <stdio.h>  // for printf()

#include "gtest/gtest.h"

#include "dot3/dot3.h"
#include "dot3-internal.h"

/*
 * Test case
 *  1) WSMP-N-Header 필수필드 디코딩 결과 확인
 *  2) WSMP-N-Header ChannelNumber 옵션필드 디코딩 결과 확인
 *  3) WSMP-N-Header DataRate 옵션필드 디코딩 결과 확인
 *  4) WSMP-N-Header TransmitPowerUsed 옵션필드 디코딩 결과 확인
 *  5) WSMP-N-Header 모든 옵션필드 포함 디코딩 결과 확인
 *  6) WSMP-T-Header PSID 필드 디코딩 결과 확인
 *  7) 최소길이 WSM body 디코딩 결과 확인
 *  8) 최소길이(4) 헤더일 때, 최대길이(2297) WSM body 디코딩 결과 확인
 *  9) 최대길이(18) 헤더일 때, 최대길이(2284) WSM body 디코딩 결과 확인
 */

/*
 *  1) WSMP-N-Header 필수필드 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_N_HDR_MANDATORY)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  uint8_t msdu[4] = {0x03, 0x00, 0x00, 0x00};

  int payload_size;

  /*
   * WSMP-N-Header 필수필드 디코딩
   */
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu, sizeof(msdu), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * 유효하지 않은 WSMP version 값 (3 외)
   */
  for (int i = 0; i < 8; i++) {
    if (i == 3) { // 정상 케이스 생략
      continue;
    }
    // 입력값
    msdu[0] = i;
    // 테스트
    payload_size = dot3_ParseWsm(msdu, sizeof(msdu), outbuf, sizeof(outbuf), &params);
    EXPECT_EQ(payload_size, -kDot3Result_Fail_InvalidWsmpNHeaderWsmpVersion);
  }
}


/*
 *  2) WSMP-N-Header ChannelNumber 옵션필드 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_N_HDR_EXT_CHAN_NUM)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  uint8_t msdu[] = {0x0B,0x01,0x0F,0x01,0xAC,0x00,0x00,0x00};

  int payload_size;

  /*
   * ChannelNumber 확장필드 범위 값 (0~255) 테스트
   */
  for (int i = 0; i < 255; i++) {
    // 입력값
    msdu[4] = (uint8_t)i;
    // 기대값
    memset(&expected, 0, sizeof(expected));
    expected.version = 3;
    expected.tx_chan_num = i;
    expected.tx_datarate = kDot3DataRate_Unknown;
    expected.tx_power = kDot3Power_Unknown;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = dot3_ParseWsm(msdu, sizeof(msdu), outbuf, sizeof(outbuf), &params);
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
  }
}


/*
 *  3) WSMP-N-Header DataRate 옵션필드 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_N_HDR_EXT_DATARATE)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  uint8_t msdu[] = {0x0B,0x01,0x10,0x01,0x06,0x00,0x00,0x00};

  int payload_size;

  /*
   * DataRate 확장필드 범위 값 (0~255) 테스트
   */
  for (int i = 0; i < 255; i++) {
    // 입력값
    msdu[4] = (uint8_t)i;
    // 기대값
    memset(&expected, 0, sizeof(expected));
    expected.version = 3;
    expected.tx_chan_num = kDot3Channel_Unknown;
    expected.tx_datarate = i;
    expected.tx_power = kDot3Power_Unknown;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = dot3_ParseWsm(msdu, sizeof(msdu), outbuf, sizeof(outbuf), &params);
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
  }
}


/*
 *  4) WSMP-N-Header TransmitPowerUsed 옵션필드 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_N_HDR_EXT_TX_POWER_USED)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];
  uint8_t msdu[] = {0x0B,0x01,0x04,0x01,0x00,0x00,0x00,0x00};

  int payload_size;

  /*
   * TxPower 확장필드 범위 값 (-128~128) 테스트
   */
  for (int i = 0; i < 255; i++) {
    // 입력값
    msdu[4] = (uint8_t)i;
    // 기대값
    memset(&expected, 0, sizeof(expected));
    expected.version = 3;
    expected.tx_chan_num = kDot3Channel_Unknown;
    expected.tx_datarate = kDot3DataRate_Unknown;
    expected.tx_power = i - 128;
    // 테스트
    memset(&params, 0, sizeof(params));
    payload_size = dot3_ParseWsm(msdu, sizeof(msdu), outbuf, sizeof(outbuf), &params);
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
  }
}


/*
 *  5) WSMP-N-Header 모든 옵션필드 포함 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_N_HDR_EXT_MULTI)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];

  int payload_size;

  /*
   * ChannelNumber, DataRate 확장필드 포함 테스트
   */
  // 입력값
  uint8_t msdu1[] = {0x0B,0x02,0x0F,0x01,0xAC,0x10,0x01,0x06,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_3Mbps;
  expected.tx_power = kDot3Power_Unknown;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu1, sizeof(msdu1), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * ChannelNumber, TxPowerUsed 확장필드 포함 테스트
   */
  // 입력값
  uint8_t msdu2[] = {0x0B,0x02,0x0F,0x01,0xAC,0x04,0x01,0x00,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = -128;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu2, sizeof(msdu2), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * DataRate, TxPowerUsed 확장필드 포함 테스트
   */
  // 입력값
  uint8_t msdu3[] = {0x0B,0x02,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_3Mbps;
  expected.tx_power = -128;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu3, sizeof(msdu3), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * ChannelNumber, DataRate, TxPowerUsed 확장필드 포함 테스트
   */
  // 입력값
  uint8_t msdu4[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_3Mbps;
  expected.tx_power = -128;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu4, sizeof(msdu4), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * ChannelNumber, DataRate, TxPowerUsed 확장필드 포함 테스트 - 표준문서 샘플 비교
   */
  // 입력값
  uint8_t msdu5[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x0C,0x04,0x01,0x9E,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_6Mbps;
  expected.tx_power = 30;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu5, sizeof(msdu5), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
}


/*
 *
 *  6) WSMP-T-Header PSID 필드 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSMP_T_HDR_PSID)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];

  int payload_size;

  /*
   * PSID 최소값(0) 테스트
   */
  // 입력값
  uint8_t msdu1[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0x00,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_3Mbps;
  expected.tx_power = -128;
  expected.psid = 0;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu1, sizeof(msdu1), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));

  /*
   * PSID 최대값(270549119) 테스트
   */
  // 입력값
  uint8_t msdu2[] = {0x0B,0x03,0x0F,0x01,0xAC,0x10,0x01,0x06,0x04,0x01,0x00,0x00,0xEF,0xFF,0xFF,0xFF,0x00};
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 172;
  expected.tx_datarate = kDot3DataRate_3Mbps;
  expected.tx_power = -128;
  expected.psid = 270549119;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(msdu2, sizeof(msdu2), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, 0);
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
}


/*
 * 7) 최소길이 WSM body 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSM_BODY_LEN_MIN)
{
  /*
   * 생략 : 위에서 수행된 단위테스트들이 모두 WSM body 최소길이에 대해 수행되었다.
   */
}


/*
 *  8) 최소길이(4) 헤더일 때, 최대길이(2297) WSM body 디코딩 결과 확인
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
TEST(dot3_ParseWsm, WSM_BODY_LEN_MAX_WITH_MIN_HDR)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];

  int payload_size;

  /*
   * 최대길이(2297) WSM body 테스트
   */
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = kDot3Channel_Unknown;
  expected.tx_datarate = kDot3DataRate_Unknown;
  expected.tx_power = kDot3Power_Unknown;
  expected.psid = 0;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(g_encoded_wsm1, sizeof(g_encoded_wsm1), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, sizeof(g_payload1));
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  EXPECT_TRUE(!memcmp(outbuf, g_payload1, payload_size));
}


/*
 *  9) 최대길이(18) 헤더일 때, 최대길이(2284) WSM body 디코딩 결과 확인
 *
 *  msg ShortMsgNpdu ::= {
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
TEST(dot3_ParseWsm, WSM_BODY_LEN_MAX_WITH_MAX_HDR)
{
  Dot3_Init(kDot3LogLevel_none);  // 테스트 실패 원인 확인 시에는 kDot3LogLevel_max 로 변경

  /*
   * 공통 파라미터
   */
  struct Dot3WsmMpduRxParams params, expected;
  uint8_t outbuf[kMpduMaxSize];

  int payload_size;

  /*
   * 최대길이(2284) WSM body 테스트
   */
  // 기대값
  memset(&expected, 0, sizeof(expected));
  expected.version = 3;
  expected.tx_chan_num = 184;
  expected.tx_datarate = kDot3DataRate_27Mbps;
  expected.tx_power = kDot3Power_Max;
  expected.psid = 270549119;
  // 테스트
  memset(&params, 0, sizeof(params));
  payload_size = dot3_ParseWsm(g_encoded_wsm2, sizeof(g_encoded_wsm2), outbuf, sizeof(outbuf), &params);
  EXPECT_EQ(payload_size, sizeof(g_payload2));
  EXPECT_TRUE(!memcmp(&params, &expected, sizeof(params)));
  EXPECT_TRUE(!memcmp(outbuf, g_payload2, payload_size));
}
