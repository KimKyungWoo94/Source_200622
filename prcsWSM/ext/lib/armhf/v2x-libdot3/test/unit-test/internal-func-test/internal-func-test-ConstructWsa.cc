
/**
 * @file internal-func-test-ConstructWsa.cc
 * @date 2019-08-18
 * @author gyun
 * @brief dot3_ConstructWsa() 함수에 대한 단위테스트
 *
 * 본 파일은 dot3 라이브러리 내부 함수 중, dot3_ConstructWsa() 함수에 대한 단위 테스트를 수행하는 테스트코드를 포함한다.
 * API 파라미터(WSA 구성파라미터, 페이로드, 출력버퍼 등) 값에 대한 단위테스트는 검사하지 않는다. (이는 API 단위테스트에서 수행된다)
 * 여기서는 API 파라미터는 유효하다고 가정하고, WSA 인코딩 결과에 대한 단위테스트만을 수행한다.
 *
 * 시험데이터 및 기대값은 https://asn1.io/asn1playground/ 에서 획득하였다.
 */


#include <stdio.h>  // for printf()

#include "gtest/gtest.h"

#include "dot3/dot3.h"
#include "dot3-internal.h"


/*
 * Test case
 *  - 필수헤더만 포함된 WSA의 인코딩 유효성을 확인한다.
 *  - 옵션헤더까지 포함된 WSA의 인코딩 유효성을 확인한다.
 *  - 각각 1개의 Service info 와 Channel info 가 포함된 WSA 인코딩 동작을 확인한다.
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA 인코딩 동작을 확인한다.
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA 인코딩 동작을 확인하며,
 *    일부 Service info (0번, 30번)는 확장필드를 포함하도록 한다.
 *   - WSA id 가 동일한 PSR만 WSA에 수납되는 것을 확인한다.
 */

static uint8_t g_test_ipv6_address[16] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static uint8_t g_test_mac_address[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
static struct Dot3ProviderInfo *g_pinfo = &(g_dot3_mib.provider_info);
static uint8_t g_test_psc[31] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e
};

/*
 * Sample PSR 데이터를 생성한다.
 */
static void PrepareSamplePsrs(struct Dot3Psr *const psrs, const uint8_t psr_num, const Dot3WsaIdentifier wsa_id)
{
  struct Dot3Psr *psr;

  /*
   * PSR 중 앞 절반은 모든 확장 영역이 존재하는 PSR로 설정
   */
  for (int i = 0; i < psr_num/2; i++) {
    psr = (psrs + i);
    psr->present.psc = true;
    psr->present.provider_mac_addr = true;
    psr->present.rcpi_threshold = true;
    psr->present.wsa_cnt_threshold = true;
    psr->present.wsa_cnt_threshold_interval = true;
    psr->wsa_id = wsa_id;
    psr->psid = i;
    psr->service_chan_num = kDot3Channel_KoreaV2XMin;
    psr->chan_access = kDot3ProviderChannelAccess_Any;
    sprintf((char *)psr->psc.psc, "PSC %03d - test                ", i);
    psr->psc.len = strlen((char *)psr->psc.psc);
    psr->ip_service = true;
    memcpy(psr->ipv6_address, g_test_ipv6_address, sizeof(g_test_ipv6_address));
    psr->ipv6_address[15] = i;
    psr->service_port = i;
    memcpy(psr->provider_mac_addr, g_test_mac_address, sizeof(g_test_mac_address));
    psr->provider_mac_addr[5] = i;
    psr->rcpi_threshold = i;
    psr->wsa_cnt_threshold = i;
    psr->wsa_cnt_threshold_interval = i+1;
  }

  /*
   * PSR 중 뒤 절반은 모든 확장 영역이 존재하지 않는 PSR로 설정
   */
  for (int i = psr_num/2; i < psr_num; i++) {
    psr = (psrs + i);
    psr->present.psc = false;
    psr->present.provider_mac_addr = false;
    psr->present.rcpi_threshold = false;
    psr->present.wsa_cnt_threshold = false;
    psr->present.wsa_cnt_threshold_interval = false;
    psr->wsa_id = wsa_id;
    psr->psid = i;
    psr->service_chan_num = kDot3Channel_KoreaV2XMin;
    psr->chan_access = kDot3ProviderChannelAccess_Any;
    psr->ip_service = false;
  }
}


/*
 *  - 필수헤더만 포함된 WSA의 인코딩 유효성을 확인한다.
 *    - 각 필수필드에 대한 유효범위 검사
 *    - WSA 헤더만 인코딩하는 조건은 다음 중 하나이다.
 *      1) PSR이 등록되어 있지 않은 경우
 *      2) PSR이 등록되어 있으나, 동일한 wsa_id를 갖는 PSR은 없는 경우
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
TEST(dot3_ConstructWsa, WSA_HDR_MANDATORY)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  /*
   * PSR 이 등록되어 있지 않으면, 헤더만 포함된 WSA가 생성되는 것을 확인한다.
   * 동시에 헤더 내 필수필드의 범위값에 대해 테스트한다.
   */
  {
    // 기본 값 테스트 (위 asn.1 구문)
    memset(&params, 0, sizeof(params));
    uint8_t expected_output[] = {0x30, 0x00};
    int expected_encoded_size = sizeof(expected_output);
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, expected_encoded_size);
    EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));

    // wsa_id 가 유효범위 내의 값인 경우 인코딩 성공하는 것을 확인한다.
    for (int i = kDot3WsaMinId; i <= kDot3WsaMaxId; i++) {
      memset(&params, 0, sizeof(params));
      params.hdr.wsa_id = i;
      expected_output[1] = (i << 4);
      ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
      EXPECT_EQ(ret, expected_encoded_size);
      EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
    }
    // wsa_id 가 유효범위 밖의 값인 경우 인코딩 실패하는 것을 확인한다.
    memset(&params, 0, sizeof(params));
    params.hdr.wsa_id = kDot3WsaMaxId + 1;
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_Asn1Encode);

    // content_count 가 유효범위 내의 값인 경우 인코딩 성공하는 것을 확인한다.
    for (int i = kDot3WsaMinContentCount; i <= kDot3WsaMaxContentCount; i++) {
      memset(&params, 0, sizeof(params));
      params.hdr.content_count = i;
      expected_output[1] = i;
      ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
      EXPECT_EQ(ret, expected_encoded_size);
      EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
    }
    // content_count 가 유효범위 밖의 값인 경우 인코딩 실패하는 것을 확인한다.
    params.hdr.wsa_id = kDot3WsaMaxContentCount + 1;
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_Asn1Encode);
  }

  /*
   * PSR은 등록되어 있지만, 동일한 wsa_id를 갖는 PSR이 없는 상황에서는 헤더만 포함된 WSA가 생성되는 것을 확인한다.
   */
  {
    struct Dot3Psr psrs[kDot3PsrNum_MaxNum];
    memset(psrs, 0, sizeof(psrs));

    // PSR 샘플 데이터 준비 - wsa_id는 모두 0이다.
    PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum, 1/*wsa_id*/);

    // wsa_id != 0 인 WSA 생성을 요청하면, 헤더만 포함된 WSA가 생성되는 것을 확인한다.
    memset(&params, 0, sizeof(params));
    params.hdr.wsa_id = 2;
    uint8_t expected_output[] = {0x30, 0x20};
    int expected_encoded_size = sizeof(expected_output);
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, expected_encoded_size);
    EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
  }
}


/*
 *  - 옵션헤더까지 포함된 WSA의 인코딩 유효성을 확인한다.
 *    - 여기서는 헤더만 포함된 WSA로 테스트한다.
 *    - 각 옵션필드 값의 유효범위 검사는 하지 않는다 (생략)
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
TEST(dot3_ConstructWsa, WSA_HDR_ALL)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  uint8_t expected_output[] = {
    0x38,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72};

  /*
   * PSR 이 등록되지 않은 상태에서 WSA를 생성하면, 헤더만 포함되어 있는 것을 확인한다.
   */
  memset(&params, 0, sizeof(params));
  params.hdr.extensions.repeat_rate = true;       // 옵션필드 추가 설정
  params.hdr.extensions.twod_location = true;     // 옵션필드 추가 설정
  params.hdr.extensions.threed_location = true;   // 옵션필드 추가 설정
  params.hdr.extensions.advertiser_id = true;     // 옵션필드 추가 설정
  params.hdr.repeat_rate = 50;
  params.hdr.twod_location.latitude = 900000001;
  params.hdr.twod_location.longitude = 1800000001;
  params.hdr.threed_location.latitude = 900000001;
  params.hdr.threed_location.longitude = 1800000001;
  params.hdr.threed_location.elevation = 61439;
  params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
  int expected_encoded_size = sizeof(expected_output);
  ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, expected_encoded_size);
  EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
}


/*
 *  - 각각 1개의 Service info 와 Channel info 가 포함된 WSA 인코딩 동작을 확인한다.
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
TEST(dot3_ConstructWsa, WSA_1_SERV_INFO)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  uint8_t expected_output[] = {
    0x3E,0x00,0x04,0x11,0x01,0x32,0x05,0x08,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,
    0x06,0x0A,0x6B,0x49,0xD2,0x01,0xD6,0x93,0xA4,0x00,0xFF,0xFF,0x07,0x16,0x15,0x41,
    0x64,0x76,0x65,0x72,0x74,0x69,0x73,0x65,0x72,0x20,0x49,0x64,0x65,0x6E,0x74,0x69,
    0x66,0x69,0x65,0x72,0x01,0x00,0x08,0x01,0x11,0xAC,0xA1,0x8C,0x00
  };

  /*
   * 1개의 PSR을 등록한다.
   */
  struct Dot3Psr psr = {0,};
  psr.wsa_id = 0;
  psr.psid = 0;
  psr.service_chan_num = 172;
  ret = dot3_AddPsr(g_pinfo, &psr);
  EXPECT_EQ(ret, 1);

  /*
   * 등록된 PSR과 관련된 1개의 Service info와 channel info를 포함하는 WSA가 정상적으로 생성되는 것을 확인한다. (동일한 wsa_id)
   */
  memset(&params, 0, sizeof(params));
  params.hdr.extensions.repeat_rate = true;
  params.hdr.extensions.twod_location = true;
  params.hdr.extensions.threed_location = true;
  params.hdr.extensions.advertiser_id = true;
  params.present.wra = false;
  params.hdr.wsa_id = psr.wsa_id;
  params.hdr.content_count = 0;
  params.hdr.repeat_rate = 50;
  params.hdr.twod_location.latitude = 900000001;
  params.hdr.twod_location.longitude = 1800000001;
  params.hdr.threed_location.latitude = 900000001;
  params.hdr.threed_location.longitude = 1800000001;
  params.hdr.threed_location.elevation = 61439;
  params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
  int expected_encoded_size = sizeof(expected_output);
  ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, expected_encoded_size);
  EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
}


/*
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA 인코딩 동작을 확인한다.
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
TEST(dot3_ConstructWsa, WSA_MAX_SERV_INFO)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  /*
   * WSA에 수납될 수 있을 만큼의 PSR 등록 후 테스트 (31개 = _WSA_SERVICE_INFO_MAX_NUM_)
   */
  {
    uint8_t expected_output[] = {
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

    // 31개(_WSA_SERVICE_INFO_MAX_NUM_)의 PSR을 등록한다.
    struct Dot3Psr psrs1[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
    memset(psrs1, 0, sizeof(psrs1));
    for (int i = 0; i < _WSA_SERVICE_INFO_MAX_NUM_; i++) {
      psrs1[i].psid = i;
      psrs1[i].service_chan_num = 172 + (i % 13);
      ret = dot3_AddPsr(g_pinfo, &psrs1[i]);
      EXPECT_EQ(ret, i + 1);
    }
    // 정상적으로 등록되었는지 확인한다.
    memset(psrs2, 0, sizeof(psrs2));
    ret = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(ret, _WSA_SERVICE_INFO_MAX_NUM_);
    EXPECT_TRUE(!memcmp(psrs2, psrs1, sizeof(struct Dot3Psr) * ret));

    // 위에서 등록한 모든 PSR과 관련된 Service info와 channel info가 수납된 WSA가 정상적으로 생성되는 것을 확인한다.
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.repeat_rate = true;
    params.hdr.extensions.twod_location = true;
    params.hdr.extensions.threed_location = true;
    params.hdr.extensions.advertiser_id = true;
    params.present.wra = false;
    params.hdr.wsa_id = 0;
    params.hdr.content_count = 0;
    params.hdr.repeat_rate = 50;
    params.hdr.twod_location.latitude = 900000001;
    params.hdr.twod_location.longitude = 1800000001;
    params.hdr.threed_location.latitude = 900000001;
    params.hdr.threed_location.longitude = 1800000001;
    params.hdr.threed_location.elevation = 61439;
    params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
    memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
    int expected_encoded_size = sizeof(expected_output);
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, expected_encoded_size);
    EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
  }

 /*
  * PSR 최대개수(kDot3PsrNum_MaxNum=128) 등록 상태에서도, WSA에 수납될 수 있을 만큼(31=_WSA_SERVICE_INFO_MAX_NUM_)만 선택되어
  * WSA가 정상적으로 생성되는 것을 확인한다.
  *  - 테이블 상의 앞에 위치한 PSR 정보부터 우선적으로 수납된다.
  */
  dot3_DeleteAllPsrs(g_pinfo);
  {
    uint8_t expected_output[] = {
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

    // PSR을 최대로 등록한다. (kDot3PsrNum_MaxNum=128개)
    struct Dot3Psr psrs1[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
    memset(psrs1, 0, sizeof(psrs1));
    for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
      psrs1[i].psid = i;
      psrs1[i].service_chan_num = 172 + (i % 13);
      ret = dot3_AddPsr(g_pinfo, &psrs1[i]);
      EXPECT_EQ(ret, i + 1);
    }
    // 정상등록여부를 확인한다.
    memset(psrs2, 0, sizeof(psrs2));
    ret = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(ret, kDot3PsrNum_MaxNum);
    EXPECT_TRUE(!memcmp(psrs2, psrs1, sizeof(struct Dot3Psr) * ret));

    // WSA 생성 시, 테이블 내 PSR 중에서 WSA에 수납가능한 만큼(31=_WSA_SERVICE_INFO_MAX_NUM_)만 선택되어 수납되는 것을 확인한다.
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.repeat_rate = true;
    params.hdr.extensions.twod_location = true;
    params.hdr.extensions.threed_location = true;
    params.hdr.extensions.advertiser_id = true;
    params.present.wra = false;
    params.hdr.wsa_id = 0;
    params.hdr.content_count = 0;
    params.hdr.repeat_rate = 50;
    params.hdr.twod_location.latitude = 900000001;
    params.hdr.twod_location.longitude = 1800000001;
    params.hdr.threed_location.latitude = 900000001;
    params.hdr.threed_location.longitude = 1800000001;
    params.hdr.threed_location.elevation = 61439;
    params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
    memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
    int expected_encoded_size = sizeof(expected_output);
    ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, expected_encoded_size);
    EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
  }
}


/*
 *  - 수납 가능한 최대의 Service info 와 이에 관련된 Channel info 가 포함된 WSA 인코딩 동작을 확인하며,
 *    일부 Service info (0번, 30번)는 확장필드를 포함하도록 한다.
 *
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
TEST(dot3_ConstructWsa, WSA_MAX_SERV_INFO_ALL_EXT)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  uint8_t expected_output[] = {
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
   * PSR을 등록가능한 최대개수만큼 등록한다.
   *  - 이때 0번 PSR과 30번 PSR에는 확정정보를 포함시킨다.
   */
  struct Dot3Psr psrs1[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
  memset(psrs1, 0, sizeof(psrs1));
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    psrs1[i].psid = i;
    psrs1[i].service_chan_num = 172 + (i % 13);
    if (i == 0) { // 0번 PSR에 확장필드 모두 포함
      psrs1[i].present.psc = true;
      psrs1[i].present.provider_mac_addr = true;
      psrs1[i].present.rcpi_threshold = true;
      psrs1[i].present.wsa_cnt_threshold = true;
      psrs1[i].present.wsa_cnt_threshold_interval = true;
      psrs1[i].ip_service = true;
      psrs1[i].psc.len = kDot3PscLen_Max;
      memcpy(psrs1[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      memcpy(psrs1[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      psrs1[i].service_port = 200;
      memcpy(psrs1[i].provider_mac_addr, g_test_mac_address, MAC_ALEN);
      psrs1[i].rcpi_threshold = 200;
      psrs1[i].wsa_cnt_threshold = 200;
      psrs1[i].wsa_cnt_threshold_interval = 200;
    }
    if (i == 30) { // 30번 PSR에 확장필드 모두 포함
      psrs1[i].present.psc = true;
      psrs1[i].present.provider_mac_addr = true;
      psrs1[i].present.rcpi_threshold = true;
      psrs1[i].present.wsa_cnt_threshold = true;
      psrs1[i].present.wsa_cnt_threshold_interval = true;
      psrs1[i].ip_service = true;
      psrs1[i].psc.len = kDot3PscLen_Max;
      memcpy(psrs1[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      psrs1[i].psc.psc[0] = 0x01;
      memcpy(psrs1[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      psrs1[i].ipv6_address[0] = 0x01;
      psrs1[i].service_port = 100;
      memcpy(psrs1[i].provider_mac_addr, g_test_mac_address, MAC_ALEN);
      psrs1[i].provider_mac_addr[0] = 0x01;
      psrs1[i].rcpi_threshold = 100;
      psrs1[i].wsa_cnt_threshold = 100;
      psrs1[i].wsa_cnt_threshold_interval = 100;
    }
    ret = dot3_AddPsr(g_pinfo, &psrs1[i]);
    EXPECT_EQ(ret, i + 1);
  }
  // 정상등록 여부 체크
  memset(psrs2, 0, sizeof(psrs2));
  ret = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(ret, kDot3PsrNum_MaxNum);
  EXPECT_TRUE(!memcmp(psrs2, psrs1, sizeof(struct Dot3Psr) * ret));


  /*
   * 0번, 30번 Service info의 확장필드가 제대로 포함되어 정상적인 WSA가 생성된 것을 확인한다.
   */
  memset(&params, 0, sizeof(params));
  params.hdr.extensions.repeat_rate = true;
  params.hdr.extensions.twod_location = true;
  params.hdr.extensions.threed_location = true;
  params.hdr.extensions.advertiser_id = true;
  params.present.wra = false;
  params.hdr.wsa_id = 0;
  params.hdr.content_count = 0;
  params.hdr.repeat_rate = 50;
  params.hdr.twod_location.latitude = 900000001;
  params.hdr.twod_location.longitude = 1800000001;
  params.hdr.threed_location.latitude = 900000001;
  params.hdr.threed_location.longitude = 1800000001;
  params.hdr.threed_location.elevation = 61439;
  params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
  int expected_encoded_size = sizeof(expected_output);
  ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, expected_encoded_size);
  EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
}


/*
 *  - WRA 포함된 WSA를 확인한다.
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
TEST(dot3_ConstructWsa, WRA)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(kDot3LogLevel_none);

  uint8_t expected_output[] = {
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
   * PSR을 등록가능한 최대개수만큼 등록한다.
   *  - 이때 0번 PSR은 확장정보를 포함시킨다.
   *  - wsa_id를 최소값부터 최대값 사이에서 순차적으로 증가시키며 각 PSR에 대입한다.
   */
  struct Dot3Psr psrs1[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
  memset(psrs1, 0, sizeof(psrs1));
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    psrs1[i].psid = i;
    psrs1[i].wsa_id = (i % (kDot3WsaMaxId + 1));  // PSR 별로 wsa_id를 다르게 준다.
    psrs1[i].service_chan_num = 172 + (i % 13);
    if (i == 0) {
      psrs1[i].present.psc = true;
      psrs1[i].present.provider_mac_addr = true;
      psrs1[i].present.rcpi_threshold = true;
      psrs1[i].present.wsa_cnt_threshold = true;
      psrs1[i].present.wsa_cnt_threshold_interval = true;
      psrs1[i].ip_service = true;
      psrs1[i].psc.len = kDot3PscLen_Max;
      memcpy(psrs1[i].psc.psc, g_test_psc, kDot3PscLen_Max);
      memcpy(psrs1[i].ipv6_address, g_test_ipv6_address, IPV6_ALEN);
      psrs1[i].service_port = 200;
      memcpy(psrs1[i].provider_mac_addr, g_test_mac_address, MAC_ALEN);
      psrs1[i].rcpi_threshold = 200;
      psrs1[i].wsa_cnt_threshold = 200;
      psrs1[i].wsa_cnt_threshold_interval = 200;
    }
    ret = dot3_AddPsr(g_pinfo, &psrs1[i]);
    EXPECT_EQ(ret, i + 1);
  }
  // 등록된 PSR 정보 확인
  memset(psrs2, 0, sizeof(psrs2));
  ret = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(ret, kDot3PsrNum_MaxNum);
  EXPECT_TRUE(!memcmp(psrs2, psrs1, sizeof(struct Dot3Psr) * ret));


  /*
   * WSA 생성 시 명시된 wsa_id와 동일한 wsa_id를 갖는 PSR에 관련된 Service info, channel info 만이 WSA에 수납된 것을 확인한다.
   */
  memset(&params, 0, sizeof(params));
  params.hdr.extensions.repeat_rate = true;
  params.hdr.extensions.twod_location = true;
  params.hdr.extensions.threed_location = true;
  params.hdr.extensions.advertiser_id = true;
  params.present.wra = false;
  params.hdr.wsa_id = 0;    // wsa_id = 0 인 PSR 정보만 WSA에 수납되어야 한다.
  params.hdr.content_count = 0;
  params.hdr.repeat_rate = 50;
  params.hdr.twod_location.latitude = 900000001;
  params.hdr.twod_location.longitude = 1800000001;
  params.hdr.threed_location.latitude = 900000001;
  params.hdr.threed_location.longitude = 1800000001;
  params.hdr.threed_location.elevation = 61439;
  params.hdr.advertiser_id.len = strlen("Advertiser Identifier");
  memcpy(params.hdr.advertiser_id.id, "Advertiser Identifier", params.hdr.advertiser_id.len);
  params.present.wra = true;
  params.wra.router_lifetime = 100;
  memcpy(params.wra.ip_prefix, g_test_ipv6_address, IPV6_ALEN);
  params.wra.ip_prefix_len = 100;
  memcpy(params.wra.default_gw, g_test_ipv6_address, IPV6_ALEN);
  memcpy(params.wra.primary_dns, g_test_ipv6_address, IPV6_ALEN);
  int expected_encoded_size = sizeof(expected_output);
  ret = dot3_ConstructWsa(g_pinfo, &params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, expected_encoded_size);
  EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
}
