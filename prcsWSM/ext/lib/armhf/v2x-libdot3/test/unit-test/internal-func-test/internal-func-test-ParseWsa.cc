
/**
 * @file internal-func-test-ParseWsa.cc
 * @date 2019-08-19
 * @author gyun
 * @brief dot3_ParseWsa() 함수에 대한 단위테스트
 *
 * 본 파일은 dot3 라이브러리 내부 함수 중, dot3_ParseWsa() 함수에 대한 단위 테스트를 수행하는 테스트코드를 포함한다.
 * API 파라미터(WSA 구성파라미터, 페이로드, 출력버퍼 등) 값에 대한 단위테스트는 검사하지 않는다. (이는 API 단위테스트에서 수행된다)
 * 여기서는 API 파라미터는 유효하다고 가정하고, WSA 인코딩 결과에 대한 단위테스트만을 수행한다.
 *
 * 시험데이터 및 기대값은 https://asn1.io/asn1playground/ 에서 획득하였다.
 */


#include <stdio.h>  // for printf()
#include <dot3/dot3.h>

#include "gtest/gtest.h"

#include "dot3/dot3.h"
#include "dot3-internal.h"


/*
 * Test case
 *  - dot3_ConstructWsa() 단위테스트에서 사용했던 인코딩 데이터가 정확하게 디코딩되는지 확인한다.
 */


static uint8_t g_test_ipv6_address[16] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static uint8_t g_test_mac_address[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
static uint8_t g_test_psc[31] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e
};


/*
 *  - 필수헤더만 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
  msg SrvAdvMsg ::= {
    version {
      messageID saMessage
      rsvAdvPrtVersion 3
    }
    body {
      changeCount {
        saID 0   -- 0..15, +
        contentCount 0  -- 0..15, +
      }
    }
  }
 */
TEST(dot3_ParseWsa, WSA_HDR_MANDATORY)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {0x30, 0x33};

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 3;
  expected_params.hdr.content_count = 3;

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}


/*
 *  - 옵션헤더까지 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
msg SrvAdvMsg ::=
{
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
  }
}
 */
TEST(dot3_ParseWsa, WSA_HDR_ALL)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x38,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72};

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}


/*
 *  - 각각 1개의 Service info 와 Channel info 가 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
msg SrvAdvMsg ::=
{
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
    serviceInfos {
      { serviceID content 0 channelIndex 1 chOptions {} }
    }
    channelInfos {
      { operatingClass 17 channelNumber 172 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
    }
  }
}
 */
TEST(dot3_ParseWsa, WSA_1_SERV_INFO)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x3E,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x01,0x00,0x08,0x01,0x11,0xAC,0xA1,0x8C,0x00
  };

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);
  expected_params.wsi_num = 1;
  expected_params.wsis[0].psid = 0;
  expected_params.wsis[0].channel_index = 1;
  expected_params.wci_num = 1;
  expected_params.wcis[0].operating_class = 17;
  expected_params.wcis[0].chan_num = 172;
  expected_params.wcis[0].transmit_power_level = 33;
  expected_params.wcis[0].datarate = 12;
  expected_params.wcis[0].adaptable_datarate = true;

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}



/*
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
msg SrvAdvMsg ::= {
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
    serviceInfos {
      { serviceID content 0 channelIndex 1 chOptions {} }
      { serviceID content 1 channelIndex 2 chOptions {} }
      { serviceID content 2 channelIndex 3 chOptions {} }
      { serviceID content 3 channelIndex 4 chOptions {} }
      { serviceID content 4 channelIndex 5 chOptions {} }
      { serviceID content 5 channelIndex 6 chOptions {} }
      { serviceID content 6 channelIndex 7 chOptions {} }
      { serviceID content 7 channelIndex 8 chOptions {} }
      { serviceID content 8 channelIndex 9 chOptions {} }
      { serviceID content 9 channelIndex 10 chOptions {} }
      { serviceID content 10 channelIndex 11 chOptions {} }
      { serviceID content 11 channelIndex 12 chOptions {} }
      { serviceID content 12 channelIndex 13 chOptions {} }
      { serviceID content 13 channelIndex 1 chOptions {} }
      { serviceID content 14 channelIndex 2 chOptions {} }
      { serviceID content 15 channelIndex 3 chOptions {} }
      { serviceID content 16 channelIndex 4 chOptions {} }
      { serviceID content 17 channelIndex 5 chOptions {} }
      { serviceID content 18 channelIndex 6 chOptions {} }
      { serviceID content 19 channelIndex 7 chOptions {} }
      { serviceID content 20 channelIndex 8 chOptions {} }
      { serviceID content 21 channelIndex 9 chOptions {} }
      { serviceID content 22 channelIndex 10 chOptions {} }
      { serviceID content 23 channelIndex 11 chOptions {} }
      { serviceID content 24 channelIndex 12 chOptions {} }
      { serviceID content 25 channelIndex 13 chOptions {} }
      { serviceID content 26 channelIndex 1 chOptions {} }
      { serviceID content 27 channelIndex 2 chOptions {} }
      { serviceID content 28 channelIndex 3 chOptions {} }
      { serviceID content 29 channelIndex 4 chOptions {} }
      { serviceID content 30 channelIndex 5 chOptions {} }
    }
    channelInfos {
      { operatingClass 17 channelNumber 172 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 173 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 174 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 175 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 176 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 177 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 178 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 179 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 180 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 181 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 182 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 183 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 184 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
    }
  }
}
 */
TEST(dot3_ParseWsa, WSA_MAX_SERV_INFO)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x3E,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x1F,0x00,0x08,0x01,0x10,0x02,0x18,0x03,0x20,0x04,0x28,0x05,
    0x30,0x06,0x38,0x07,0x40,0x08,0x48,0x09,0x50,0x0A,0x58,0x0B,0x60,0x0C,0x68,0x0D,
    0x08,0x0E,0x10,0x0F,0x18,0x10,0x20,0x11,0x28,0x12,0x30,0x13,0x38,0x14,0x40,0x15,
    0x48,0x16,0x50,0x17,0x58,0x18,0x60,0x19,0x68,0x1A,0x08,0x1B,0x10,0x1C,0x18,0x1D,
    0x20,0x1E,0x28,0x0D,0x11,0xAC,0xA1,0x8C,0x00,0x12,0xAD,0xA1,0x8C,0x00,0x11,0xAE,
    0xA1,0x8C,0x00,0x12,0xAF,0xA1,0x8C,0x00,0x11,0xB0,0xA1,0x8C,0x00,0x12,0xB1,0xA1,
    0x8C,0x00,0x11,0xB2,0xA1,0x8C,0x00,0x12,0xB3,0xA1,0x8C,0x00,0x11,0xB4,0xA1,0x8C,
    0x00,0x12,0xB5,0xA1,0x8C,0x00,0x11,0xB6,0xA1,0x8C,0x00,0x12,0xB7,0xA1,0x8C,0x00,
    0x11,0xB8,0xA1,0x8C,0x00
  };

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);
  expected_params.wsi_num = 31;
  for (unsigned int i = 0; i < expected_params.wsi_num; i++) {
    expected_params.wsis[i].psid = i;
    expected_params.wsis[i].channel_index = 1 + (i % 13);
  }
  expected_params.wci_num = 13;
  for (unsigned int i = 0; i < expected_params.wci_num; i++) {
    if (i%2 == 0) {
      expected_params.wcis[i].operating_class = 17;
    } else {
      expected_params.wcis[i].operating_class = 18;
    }
    expected_params.wcis[i].chan_num = 172 + i;
    expected_params.wcis[i].transmit_power_level = 33;
    expected_params.wcis[i].datarate = 12;
    expected_params.wcis[i].adaptable_datarate = true;
  }

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}



/*
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *    일부 Service info (0번, 30번)는 확장필드를 포함한다.
 *
msg SrvAdvMsg ::= {
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
    serviceInfos {
      {
        serviceID content 0
        channelIndex 1
        chOptions {
          extensions {
            { extensionId 8 value ProviderServiceContext : { fillBit '000'B psc '000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e'H } }
            {  extensionId 9 value IPv6Address : '000102030405060708090a0b0c0d0e0f'H }
            {  extensionId 10 value ServicePort : 200 }
            {  extensionId 11 value ProviderMacAddress : '000102030405'H }
            {  extensionId 19 value RcpiThreshold : 200 }
            {  extensionId 20 value WsaCountThreshold : 200 }
            {  extensionId 22 value WsaCountThresholdInterval : 200 }
          }
        }
      }
      { serviceID content 1 channelIndex 2 chOptions {} }
      { serviceID content 2 channelIndex 3 chOptions {} }
      { serviceID content 3 channelIndex 4 chOptions {} }
      { serviceID content 4 channelIndex 5 chOptions {} }
      { serviceID content 5 channelIndex 6 chOptions {} }
      { serviceID content 6 channelIndex 7 chOptions {} }
      { serviceID content 7 channelIndex 8 chOptions {} }
      { serviceID content 8 channelIndex 9 chOptions {} }
      { serviceID content 9 channelIndex 10 chOptions {} }
      { serviceID content 10 channelIndex 11 chOptions {} }
      { serviceID content 11 channelIndex 12 chOptions {} }
      { serviceID content 12 channelIndex 13 chOptions {} }
      { serviceID content 13 channelIndex 1 chOptions {} }
      { serviceID content 14 channelIndex 2 chOptions {} }
      { serviceID content 15 channelIndex 3 chOptions {} }
      { serviceID content 16 channelIndex 4 chOptions {} }
      { serviceID content 17 channelIndex 5 chOptions {} }
      { serviceID content 18 channelIndex 6 chOptions {} }
      { serviceID content 19 channelIndex 7 chOptions {} }
      { serviceID content 20 channelIndex 8 chOptions {} }
      { serviceID content 21 channelIndex 9 chOptions {} }
      { serviceID content 22 channelIndex 10 chOptions {} }
      { serviceID content 23 channelIndex 11 chOptions {} }
      { serviceID content 24 channelIndex 12 chOptions {} }
      { serviceID content 25 channelIndex 13 chOptions {} }
      { serviceID content 26 channelIndex 1 chOptions {} }
      { serviceID content 27 channelIndex 2 chOptions {} }
      { serviceID content 28 channelIndex 3 chOptions {} }
      { serviceID content 29 channelIndex 4 chOptions {} }
      {
        serviceID content 30
        channelIndex 5
        chOptions {
          extensions {
            { extensionId 8 value ProviderServiceContext : { fillBit '000'B psc '010102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e'H } }
            {  extensionId 9 value IPv6Address : '010102030405060708090a0b0c0d0e0f'H }
            {  extensionId 10 value ServicePort : 100 }
            {  extensionId 11 value ProviderMacAddress : '010102030405'H }
            {  extensionId 19 value RcpiThreshold : 100 }
            {  extensionId 20 value WsaCountThreshold : 100 }
            {  extensionId 22 value WsaCountThresholdInterval : 100 }
          }
        }
      }
    }
    channelInfos {
      { operatingClass 17 channelNumber 172 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 173 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 174 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 175 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 176 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 177 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 178 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 179 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 180 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 181 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 182 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 183 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 184 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
    }
  }
}
 */
TEST(dot3_ParseWsa, WSA_MAX_SERV_INFO_ALL_EXT)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x3E,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x1F,0x00,0x09,0x07,0x08,0x20,0x1F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x09,0x10,0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x0A,0x02,0x00,0xC8,
    0x0B,0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x13,0x01,0xC8,0x14,0x01,0xC8,0x16,0x01,
    0xC8,0x01,0x10,0x02,0x18,0x03,0x20,0x04,0x28,0x05,0x30,0x06,0x38,0x07,0x40,0x08,
    0x48,0x09,0x50,0x0A,0x58,0x0B,0x60,0x0C,0x68,0x0D,0x08,0x0E,0x10,0x0F,0x18,0x10,
    0x20,0x11,0x28,0x12,0x30,0x13,0x38,0x14,0x40,0x15,0x48,0x16,0x50,0x17,0x58,0x18,
    0x60,0x19,0x68,0x1A,0x08,0x1B,0x10,0x1C,0x18,0x1D,0x20,0x1E,0x29,0x07,0x08,0x20,
    0x1F,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x09,0x10,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
    0x0E,0x0F,0x0A,0x02,0x00,0x64,0x0B,0x06,0x01,0x01,0x02,0x03,0x04,0x05,0x13,0x01,
    0x64,0x14,0x01,0x64,0x16,0x01,0x64,0x0D,0x11,0xAC,0xA1,0x8C,0x00,0x12,0xAD,0xA1,
    0x8C,0x00,0x11,0xAE,0xA1,0x8C,0x00,0x12,0xAF,0xA1,0x8C,0x00,0x11,0xB0,0xA1,0x8C,
    0x00,0x12,0xB1,0xA1,0x8C,0x00,0x11,0xB2,0xA1,0x8C,0x00,0x12,0xB3,0xA1,0x8C,0x00,
    0x11,0xB4,0xA1,0x8C,0x00,0x12,0xB5,0xA1,0x8C,0x00,0x11,0xB6,0xA1,0x8C,0x00,0x12,
    0xB7,0xA1,0x8C,0x00,0x11,0xB8,0xA1,0x8C,0x00
  };

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);
  expected_params.wsi_num = 31;
  for (unsigned int i = 0; i < expected_params.wsi_num; i++) {
    expected_params.wsis[i].psid = i;
    expected_params.wsis[i].channel_index = 1 + (i % 13);
    if (i == 0) {
      expected_params.wsis[i].extensions.psc = true;
      expected_params.wsis[i].extensions.provider_mac_address = true;
      expected_params.wsis[i].extensions.rcpi_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold_interval = true;
      expected_params.wsis[i].extensions.ipv6_address = true;
      expected_params.wsis[i].extensions.service_port = true;
      expected_params.wsis[i].psc.len = kDot3PscLen_Max;
      memcpy(expected_params.wsis[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      memcpy(expected_params.wsis[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      expected_params.wsis[i].service_port = 200;
      memcpy(expected_params.wsis[i].provider_mac_address, g_test_mac_address, MAC_ALEN);
      expected_params.wsis[i].rcpi_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold_interval = 200;
    }
    if (i == 30) {
      expected_params.wsis[i].extensions.psc = true;
      expected_params.wsis[i].extensions.provider_mac_address = true;
      expected_params.wsis[i].extensions.rcpi_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold_interval = true;
      expected_params.wsis[i].extensions.ipv6_address = true;
      expected_params.wsis[i].extensions.service_port = true;
      expected_params.wsis[i].psc.len = kDot3PscLen_Max;
      memcpy(expected_params.wsis[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      expected_params.wsis[i].psc.psc[0] = 0x01;
      memcpy(expected_params.wsis[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      expected_params.wsis[i].ipv6_address[0] = 0x01;
      expected_params.wsis[i].service_port = 100;
      memcpy(expected_params.wsis[i].provider_mac_address, g_test_mac_address, MAC_ALEN);
      expected_params.wsis[i].provider_mac_address[0] = 0x01;
      expected_params.wsis[i].rcpi_threshold = 100;
      expected_params.wsis[i].wsa_cnt_threshold = 100;
      expected_params.wsis[i].wsa_cnt_threshold_interval = 100;
    }
  }
  expected_params.wci_num = 13;
  for (unsigned int i = 0; i < expected_params.wci_num; i++) {
    if (i%2 == 0) {
      expected_params.wcis[i].operating_class = 17;
    } else {
      expected_params.wcis[i].operating_class = 18;
    }
    expected_params.wcis[i].chan_num = 172 + i;
    expected_params.wcis[i].transmit_power_level = 33;
    expected_params.wcis[i].datarate = 12;
    expected_params.wcis[i].adaptable_datarate = true;
  }

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}

/*
 *  - 서로 동일한 WSA Channel Info를 참조하는 다수의 WSA Service Info 가 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
msg SrvAdvMsg ::= {
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
    serviceInfos {
      {
        serviceID content 0
        channelIndex 1
        chOptions {
          extensions {
            { extensionId 8 value ProviderServiceContext : { fillBit '000'B psc '000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e'H } }
            {  extensionId 9 value IPv6Address : '000102030405060708090a0b0c0d0e0f'H }
            {  extensionId 10 value ServicePort : 200 }
            {  extensionId 11 value ProviderMacAddress : '000102030405'H }
            {  extensionId 19 value RcpiThreshold : 200 }
            {  extensionId 20 value WsaCountThreshold : 200 }
            {  extensionId 22 value WsaCountThresholdInterval : 200 }
          }
        }
      }
      { serviceID content 16 channelIndex 2 chOptions {} }  --175
      { serviceID content 32 channelIndex 3 chOptions {} }  --178
      { serviceID content 48 channelIndex 4 chOptions {} } --181
      { serviceID content 64 channelIndex 5 chOptions {} } --184
      { serviceID content 80 channelIndex 6 chOptions {} }  --174
      { serviceID content 96 channelIndex 7 chOptions {} }  --177
      { serviceID content 112 channelIndex 8 chOptions {} } --180
    }
    channelInfos {
      { operatingClass 17 channelNumber 172 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 175 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 178 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 181 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 184 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 174 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 177 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 180 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
    }
  }
}
 */
TEST(dot3_ParseWsa, DUP_CHANNEL_INDEX)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x3E,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x08,0x00,0x09,0x07,0x08,0x20,0x1F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x09,0x10,0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x0A,0x02,0x00,0xC8,
    0x0B,0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x13,0x01,0xC8,0x14,0x01,0xC8,0x16,0x01,
    0xC8,0x10,0x10,0x20,0x18,0x30,0x20,0x40,0x28,0x50,0x30,0x60,0x38,0x70,0x40,0x08,
    0x11,0xAC,0xA1,0x8C,0x00,0x12,0xAF,0xA1,0x8C,0x00,0x11,0xB2,0xA1,0x8C,0x00,0x12,
    0xB5,0xA1,0x8C,0x00,0x11,0xB8,0xA1,0x8C,0x00,0x11,0xAE,0xA1,0x8C,0x00,0x12,0xB1,
    0xA1,0x8C,0x00,0x11,0xB4,0xA1,0x8C,0x00
  };

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);
  expected_params.wsi_num = 8;
  for (unsigned int i = 0; i < expected_params.wsi_num; i++) {
    expected_params.wsis[i].psid = i * 16;
    expected_params.wsis[i].channel_index = 1 + i;
    if (i == 0) {
      expected_params.wsis[i].extensions.psc = true;
      expected_params.wsis[i].extensions.provider_mac_address = true;
      expected_params.wsis[i].extensions.rcpi_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold_interval = true;
      expected_params.wsis[i].extensions.ipv6_address = true;
      expected_params.wsis[i].extensions.service_port = true;
      expected_params.wsis[i].psc.len = kDot3PscLen_Max;
      memcpy(expected_params.wsis[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      memcpy(expected_params.wsis[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      expected_params.wsis[i].service_port = 200;
      memcpy(expected_params.wsis[i].provider_mac_address, g_test_mac_address, MAC_ALEN);
      expected_params.wsis[i].rcpi_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold_interval = 200;
    }
  }
  expected_params.wci_num = 8;
  expected_params.wcis[0].operating_class = 17;
  expected_params.wcis[0].chan_num = 172;
  expected_params.wcis[0].transmit_power_level = 33;
  expected_params.wcis[0].datarate = 12;
  expected_params.wcis[0].adaptable_datarate = true;
  expected_params.wcis[1].operating_class = 18;
  expected_params.wcis[1].chan_num = 175;
  expected_params.wcis[1].transmit_power_level = 33;
  expected_params.wcis[1].datarate = 12;
  expected_params.wcis[1].adaptable_datarate = true;
  expected_params.wcis[2].operating_class = 17;
  expected_params.wcis[2].chan_num = 178;
  expected_params.wcis[2].transmit_power_level = 33;
  expected_params.wcis[2].datarate = 12;
  expected_params.wcis[2].adaptable_datarate = true;
  expected_params.wcis[3].operating_class = 18;
  expected_params.wcis[3].chan_num = 181;
  expected_params.wcis[3].transmit_power_level = 33;
  expected_params.wcis[3].datarate = 12;
  expected_params.wcis[3].adaptable_datarate = true;
  expected_params.wcis[4].operating_class = 17;
  expected_params.wcis[4].chan_num = 184;
  expected_params.wcis[4].transmit_power_level = 33;
  expected_params.wcis[4].datarate = 12;
  expected_params.wcis[4].adaptable_datarate = true;
  expected_params.wcis[5].operating_class = 17;
  expected_params.wcis[5].chan_num = 174;
  expected_params.wcis[5].transmit_power_level = 33;
  expected_params.wcis[5].datarate = 12;
  expected_params.wcis[5].adaptable_datarate = true;
  expected_params.wcis[6].operating_class = 18;
  expected_params.wcis[6].chan_num = 177;
  expected_params.wcis[6].transmit_power_level = 33;
  expected_params.wcis[6].datarate = 12;
  expected_params.wcis[6].adaptable_datarate = true;
  expected_params.wcis[7].operating_class = 17;
  expected_params.wcis[7].chan_num = 180;
  expected_params.wcis[7].transmit_power_level = 33;
  expected_params.wcis[7].datarate = 12;
  expected_params.wcis[7].adaptable_datarate = true;

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}


/*
 *  WRA 가 포함된 WSA가 정확하게 디코딩되는지 확인한다.
 *
msg SrvAdvMsg ::= {
  version {
    messageID saMessage
    rsvAdvPrtVersion 3
  }
  body {
    changeCount {
      saID 0
      contentCount 0
    }
    extensions {
      { extensionId 17 value RepeatRate:50 }
      { extensionId 5 value TwoDLocation : { latitude { fill '0'B lat 900000001 } longitude 1800000001 } }
      { extensionId 6 value ThreeDLocation: { latitude { fill '0'B lat 900000001 } longitude 1800000001 elevation 61439 } }
      { extensionId 7 value AdvertiserIdentifier: "Advertiser Identifier" }
    }
    serviceInfos {
      {
        serviceID content 0
        channelIndex 1
        chOptions {
          extensions {
            { extensionId 8 value ProviderServiceContext : { fillBit '000'B psc '000102030405060708090a0b0c0d0e0f000102030405060708090a0b0c0d0e'H } }
            {  extensionId 9 value IPv6Address : '000102030405060708090a0b0c0d0e0f'H }
            {  extensionId 10 value ServicePort : 200 }
            {  extensionId 11 value ProviderMacAddress : '000102030405'H }
            {  extensionId 19 value RcpiThreshold : 200 }
            {  extensionId 20 value WsaCountThreshold : 200 }
            {  extensionId 22 value WsaCountThresholdInterval : 200 }
          }
        }
      }
      { serviceID content 16 channelIndex 2 chOptions {} }  --175
      { serviceID content 32 channelIndex 3 chOptions {} }  --178
      { serviceID content 48 channelIndex 4 chOptions {} } --181
      { serviceID content 64 channelIndex 5 chOptions {} } --184
      { serviceID content 80 channelIndex 6 chOptions {} }  --174
      { serviceID content 96 channelIndex 7 chOptions {} }  --177
      { serviceID content 112 channelIndex 8 chOptions {} } --180
    }
    channelInfos {
      { operatingClass 17 channelNumber 172 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 175 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 178 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 181 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 184 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 174 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 18 channelNumber 177 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
      { operatingClass 17 channelNumber 180 powerLevel 33 dataRate { adaptable '1'B dataRate 12 } extensions {} }
    }
    routingAdvertisement {
      lifetime 100
      ipPrefix '000102030405060708090a0b0c0d0e0f'H
      ipPrefixLength 100
      defaultGateway '000102030405060708090a0b0c0d0e0f'H
      primaryDns '000102030405060708090a0b0c0d0e0f'H
      extensions {}
    }
  }
}
 */
TEST(dot3_ParseWsa, WRA)
{
  Dot3_Init(kDot3LogLevel_none);

  int ret;
  struct Dot3ParseWsaParams params, expected_params;
  uint8_t encoded_wsa[] = {
    0x3F,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x08,0x00,0x09,0x07,0x08,0x20,0x1F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x09,0x10,0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x0A,0x02,0x00,0xC8,
    0x0B,0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x13,0x01,0xC8,0x14,0x01,0xC8,0x16,0x01,
    0xC8,0x10,0x10,0x20,0x18,0x30,0x20,0x40,0x28,0x50,0x30,0x60,0x38,0x70,0x40,0x08,
    0x11,0xAC,0xA1,0x8C,0x00,0x12,0xAF,0xA1,0x8C,0x00,0x11,0xB2,0xA1,0x8C,0x00,0x12,
    0xB5,0xA1,0x8C,0x00,0x11,0xB8,0xA1,0x8C,0x00,0x11,0xAE,0xA1,0x8C,0x00,0x12,0xB1,
    0xA1,0x8C,0x00,0x11,0xB4,0xA1,0x8C,0x00,0x00,0x64,0x00,0x01,0x02,0x03,0x04,0x05,
    0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x64,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,
    0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x00
  };

  /*
   * 디코딩 기대값
   */
  memset(&expected_params, 0, sizeof(expected_params));
  expected_params.hdr.version = kDot3WsaVersion_Current;
  expected_params.hdr.wsa_id = 0;
  expected_params.hdr.content_count = 0;
  expected_params.hdr.extensions.repeat_rate = true;
  expected_params.hdr.extensions.twod_location = true;
  expected_params.hdr.extensions.threed_location = true;
  expected_params.hdr.extensions.advertiser_id = true;
  expected_params.hdr.repeat_rate = 50;
  expected_params.hdr.twod_location.latitude = 900000001;
  expected_params.hdr.twod_location.longitude = 1800000001;
  expected_params.hdr.threed_location.latitude = 900000001;
  expected_params.hdr.threed_location.longitude = 1800000001;
  expected_params.hdr.threed_location.elevation = 61439;
  expected_params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(expected_params.hdr.advertiser_id.id, "Advertiser Identifier", expected_params.hdr.advertiser_id.len);
  expected_params.wsi_num = 8;
  for (unsigned int i = 0; i < expected_params.wsi_num; i++) {
    expected_params.wsis[i].psid = i * 16;
    expected_params.wsis[i].channel_index = 1 + i;
    if (i == 0) {
      expected_params.wsis[i].extensions.psc = true;
      expected_params.wsis[i].extensions.provider_mac_address = true;
      expected_params.wsis[i].extensions.rcpi_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold = true;
      expected_params.wsis[i].extensions.wsa_cnt_threshold_interval = true;
      expected_params.wsis[i].extensions.ipv6_address = true;
      expected_params.wsis[i].extensions.service_port = true;
      expected_params.wsis[i].psc.len = kDot3PscLen_Max;
      memcpy(expected_params.wsis[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      memcpy(expected_params.wsis[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      expected_params.wsis[i].service_port = 200;
      memcpy(expected_params.wsis[i].provider_mac_address, g_test_mac_address, MAC_ALEN);
      expected_params.wsis[i].rcpi_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold = 200;
      expected_params.wsis[i].wsa_cnt_threshold_interval = 200;
    }
  }
  expected_params.wci_num = 8;
  expected_params.wcis[0].operating_class = 17;
  expected_params.wcis[0].chan_num = 172;
  expected_params.wcis[0].transmit_power_level = 33;
  expected_params.wcis[0].datarate = 12;
  expected_params.wcis[0].adaptable_datarate = true;
  expected_params.wcis[1].operating_class = 18;
  expected_params.wcis[1].chan_num = 175;
  expected_params.wcis[1].transmit_power_level = 33;
  expected_params.wcis[1].datarate = 12;
  expected_params.wcis[1].adaptable_datarate = true;
  expected_params.wcis[2].operating_class = 17;
  expected_params.wcis[2].chan_num = 178;
  expected_params.wcis[2].transmit_power_level = 33;
  expected_params.wcis[2].datarate = 12;
  expected_params.wcis[2].adaptable_datarate = true;
  expected_params.wcis[3].operating_class = 18;
  expected_params.wcis[3].chan_num = 181;
  expected_params.wcis[3].transmit_power_level = 33;
  expected_params.wcis[3].datarate = 12;
  expected_params.wcis[3].adaptable_datarate = true;
  expected_params.wcis[4].operating_class = 17;
  expected_params.wcis[4].chan_num = 184;
  expected_params.wcis[4].transmit_power_level = 33;
  expected_params.wcis[4].datarate = 12;
  expected_params.wcis[4].adaptable_datarate = true;
  expected_params.wcis[5].operating_class = 17;
  expected_params.wcis[5].chan_num = 174;
  expected_params.wcis[5].transmit_power_level = 33;
  expected_params.wcis[5].datarate = 12;
  expected_params.wcis[5].adaptable_datarate = true;
  expected_params.wcis[6].operating_class = 18;
  expected_params.wcis[6].chan_num = 177;
  expected_params.wcis[6].transmit_power_level = 33;
  expected_params.wcis[6].datarate = 12;
  expected_params.wcis[6].adaptable_datarate = true;
  expected_params.wcis[7].operating_class = 17;
  expected_params.wcis[7].chan_num = 180;
  expected_params.wcis[7].transmit_power_level = 33;
  expected_params.wcis[7].datarate = 12;
  expected_params.wcis[7].adaptable_datarate = true;
  expected_params.present.wra = true;
  expected_params.wra.router_lifetime = 100;
  memcpy(expected_params.wra.ip_prefix, g_test_ipv6_address, IPV6_ALEN);
  expected_params.wra.ip_prefix_len = 100;
  memcpy(expected_params.wra.default_gw, g_test_ipv6_address, IPV6_ALEN);
  memcpy(expected_params.wra.primary_dns, g_test_ipv6_address, IPV6_ALEN);

  /*
   * 디코딩 후 디코딩 기대값과 비교
   */
  memset(&params, 0, sizeof(params));
  ret = dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);
  EXPECT_TRUE(!(memcmp(&params, &expected_params, sizeof(params))));
}
