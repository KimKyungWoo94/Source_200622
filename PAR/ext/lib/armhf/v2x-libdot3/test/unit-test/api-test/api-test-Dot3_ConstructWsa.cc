
/**
 * @file api-test-Dot3_ConstructWsa.cc
 * @date 2019-08-18
 * @author gyun
 * @brief Dot3_ConstructWsa() Open API에 대한 단위테스트
 *
 * 본 파일은 Dot3_ConstructWsa() Open API에 대한 단위테스트를 수행한다.
 * 파라미터 유효성, 크기, 결과값 등에 대해 테스트한다.
 * WSA 데이터 자체의 유효성은 내부함수 단위테스트에서 수행하므로, 여기서는 수행하지 않는다.
 */

#include <stdio.h>
#include <dot3/dot3-types.h>

#include "gtest/gtest.h"

#include "dot3/dot3.h"


/*
 * Test case
 *
 * - 기본 테스트 (정해진 asn.1 값에 따라 원하는 대로 생성되는지 확인)
 * - wsa_id 파라미터 유효성에 따른 동작을 테스트한다.
 * - content_count 파라미터 유효성에 따른 동작을 테스트한다.
 * - 2DLocation.latitude/longitude 파라미터 유효성에 따른 동작을 테스트한다.
 * - 3DLocation.latitude/longitude/elevation 파라미터 유효성에 따른 동작을 테스트한다.
 * - adveritser_id 파라미터 유효성에 따른 동작을 테스트한다.
 * - 널 파라미터 유효성에 따른 동작을 테스트한다.
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
 * - 기본 테스트 (정해진 asn.1 값에 따라 원하는 대로 생성되는지 확인)
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
TEST(Dot3_ConstructWsa, Basic)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];

  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  uint8_t expected_output[] = {
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
    ret = Dot3_AddPsr(&psrs1[i]);
    EXPECT_EQ(ret, i + 1);
  }
  // 등록된 PSR 정보 확인
  memset(psrs2, 0, sizeof(psrs2));
  ret = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
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
  int expected_encoded_size = sizeof(expected_output);
  ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, expected_encoded_size);
  EXPECT_TRUE(!(memcmp(outbuf, expected_output, expected_encoded_size)));
}


/*
 * - wsa_id 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, wsa_id)
{
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 wsa_id 를 명시하면 실패하는 것을 확인한다.
   */
  struct Dot3ConstructWsaParams params;
  memset(&params, 0, sizeof(params));
  params.hdr.wsa_id = kDot3WsaMaxId + 1;
  int ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, -kDot3Result_Fail_InvalidWsaIdValue);
}


/*
 * - content_count 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, content_count)
{
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 content_count 를 명시하면 실패하는 것을 확인한다.
   */
  struct Dot3ConstructWsaParams params;
  memset(&params, 0, sizeof(params));
  params.hdr.content_count = kDot3WsaMaxContentCount + 1;
  int ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
  EXPECT_EQ(ret, -kDot3Result_Fail_InvalidWsaContentCountValue);
}


/*
 * - 2DLocation.latitude/longitude 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, twod_location)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 latitude 를 명시하면 실패하는 것을 확인한다.
   */
  {
    // 범위 밖 작은 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.twod_location = true;
    params.hdr.twod_location.latitude = kDot3Latitude_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLatitudeValue);

    // 범위 밖 큰 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.twod_location = true;
    params.hdr.twod_location.latitude = kDot3Latitude_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLatitudeValue);
  }

  /*
   * 유효하지 않은 longitude 를 명시하면 실패하는 것을 확인한다.
   */
  {
    // 범위 밖 작은 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.twod_location = true;
    params.hdr.twod_location.longitude = kDot3Longitude_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLongitudeValue);

    // 범위 밖 큰 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.twod_location = true;
    params.hdr.twod_location.longitude = kDot3Longitude_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLongitudeValue);
  }
}


/*
 * - 3DLocation.latitude/longitude/elevation 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, threed_location)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 latitude 를 명시하면 실패하는 것을 확인한다.
   */
  {
    // 범위 밖 작은 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.latitude = kDot3Latitude_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLatitudeValue);

    // 범위 밖 큰 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.latitude = kDot3Latitude_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLatitudeValue);
  }

  /*
   * 유효하지 않은 longitude 를 명시하면 실패하는 것을 확인한다.
   */
  {
    // 범위 밖 작은 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.longitude = kDot3Longitude_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLongitudeValue);

    // 범위 밖 큰 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.longitude = kDot3Longitude_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidLongitudeValue);
  }

  /*
   * 유효하지 않은 elevation 를 명시하면 실패하는 것을 확인한다.
   */
  {
    // 범위 밖 작은 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.elevation = kDot3Elevation_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidElevationValue);

    // 범위 밖 큰 값
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.threed_location = true;
    params.hdr.threed_location.elevation = kDot3Elevation_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidElevationValue);
  }
}


/*
 * - adveritser_id 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, advertiser_id)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 길이를 가지면 실패하는 것을 확인한다.
   */
  {
    // 길이 초과
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.advertiser_id = true;
    params.hdr.advertiser_id.len = kDot3WsaAdvertiserIdLen_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidAdvertiserIdLen);

    // 0
    memset(&params, 0, sizeof(params));
    params.hdr.extensions.advertiser_id = true;
    params.hdr.advertiser_id.len = 0;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidAdvertiserIdLen);
  }
}


/*
 * - WRA의 IP prefix len 파라미터 유효성에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, ip_prefix_len)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 유효하지 않은 길이를 가지면 실패하는 것을 확인한다.
   */
  {
    // 길이 초과
    memset(&params, 0, sizeof(params));
    params.present.wra = true;
    params.wra.ip_prefix_len = kDot3IPv6PrefixLen_Max + 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidIPv6PrefixLenValue);

    // 작은 길이
    memset(&params, 0, sizeof(params));
    params.present.wra = true;
    params.wra.ip_prefix_len = kDot3IPv6PrefixLen_Min - 1;
    ret = Dot3_ConstructWsa(&params, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_InvalidIPv6PrefixLenValue);
  }
}

/*
 * - 널 파라미터에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ConstructWsa, null)
{
  int ret;
  struct Dot3ConstructWsaParams params;
  uint8_t outbuf[kMpduMaxSize];
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  /*
   * 널 파라미터일 경우 실패하는 것을 확인한다.
   */
  {
    // params = NULL
    ret = Dot3_ConstructWsa(NULL, outbuf, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);

    // outbuf = NULL
    memset(&params, 0, sizeof(params));
    ret = Dot3_ConstructWsa(&params, NULL, sizeof(outbuf));
    EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);

    // outbuf_size = 0
    memset(&params, 0, sizeof(params));
    ret = Dot3_ConstructWsa(&params, outbuf, 0);
    EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);
  }
}
