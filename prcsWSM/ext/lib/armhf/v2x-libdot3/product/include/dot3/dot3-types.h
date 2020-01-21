//
// Created by gyun on 2019-06-04.
//

#ifndef V2X_LIBDOT3_DOT3_TYPES_H
#define V2X_LIBDOT3_DOT3_TYPES_H

#include <stdbool.h>
#include <stdint.h>


/// 결과 코드
enum eDot3ResultCode {
  kDot3Result_Success = 0,  ///< 성공
  kDot3Result_Fail_NullParameters,  ///< 널 파라미터가 전달된 경우
  kDot3Result_Fail_TooLongPayload, ///< 너무 큰 페이로드가 전달된 경우
  kDot3Result_Fail_InvalidPsidValue,  ///< 유효하지 않은 PSID 값
  kDot3Result_Fail_InvalidPsidFormat,  ///< 유효하지 않은 PSID 형식
  kDot3Result_Fail_InvalidPriorityValue, ///< 유효하지 않은 priority 값
  kDot3Result_Fail_InvalidChannelNumberValue, ///< 유효하지 않은 채널번호 값
  kDot3Result_Fail_InvalidDataRate, ///< 유효하지 않은 DataRate
  kDot3Result_Fail_InvalidPowerValue, ///< 유효하지 않은 파워 값
  kDot3Result_Fail_InvalidWsaCountThresholdIntervalValue, ///< 유효하지 않은 WSA Count Threshold Interval
  kDot3Result_Fail_TooLongMpdu, ///< 너무 긴 MPDU
  kDot3Result_Fail_TooShortMpdu, ///< 너무 짧은 MPDU
  kDot3Result_Fail_InsufficientBuf, ///< 길이가 충분하지 않은 버퍼
  kDot3Result_Fail_Asn1Encode, ///< asn.1 인코딩 실패
  kDot3Result_Fail_Asn1Decode, ///< asn.1 디코딩 실패
  kDot3Result_Fail_Asn1AbnormalOp, ///< 비정상 asn.1 동작
  kDot3Result_Fail_TooLongWsm, ///< 너무 긴 WSM
  kDot3Result_Fail_TooShortWsm, ///< 너무 짧은 WSM
  kDot3Result_Fail_TooLongWsa, ///< 너무 긴 WSA

  kDot3Result_Fail_InvalidWsmpNHeaderSubType, /// < 유효하지 않은 subtype (WSMP-N-Header)
  kDot3Result_Fail_InvalidWsmpNHeaderExtensionId, ///< 유효하지 않은 ExtensionId (WSMP-N-Header)
  kDot3Result_Fail_InvalidWsmpNHeaderTpid, ///< 유효하지 않은 TPID (WSMP-N-Header)
  kDot3Result_Fail_InvalidWsmpNHeaderWsmpVersion, ///< 유효하지 않은 WSMP 버전 (WSMP-N-Header)

  kDot3Result_Fail_InvalidLowerLayerProtocolVersion, ///< 유효하지 않은 하위계층 프로토콜 버전
  kDot3Result_Fail_InvalidLowerLayerFrameType, ///< 유효하지 않은 하위계층 프레임 유형
  kDot3Result_Fail_NotWildcardBssid, ///< BSSID 가 wildcard BSSID 가 아님.
  kDot3Result_Fail_NotSupportedEtherType, ///< 지원되지 않는 EtherType
  kDot3Result_Fail_NoMemory, ///< 메모리 부족

  kDot3Result_Fail_NoSuchPsr, ///< 해당 PSR이 테이블에 존재하지 않음.
  kDot3Result_Fail_PsrTableFull, ///< PSR 테이블이 꽉 참.
  kDot3Result_Fail_SamePsidPsr, ///< 동일한 PSID를 갖는 PSR이 존재함.

  kDot3Result_Fail_NoRelatedChannelInfo, ///< PSR에 연관된 Channel info 가 없음.

  kDot3Result_Fail_InvalidWsaIdValue, ///< 유효하지 않은 WSA identifier
  kDot3Result_Fail_InvalidWsaContentCountValue, ///< 유효하지 않은 WSA content count
  kDot3Result_Fail_InvalidChannelAccess, ///< 유효하지 않은 Channel access
  kDot3Result_Fail_InvalidAdvertiserIdLen, ///< 유효하지 않은 Advertiser Id 길이
  kDot3Result_Fail_TooLongPsc, ///< 너무 긴 PSC
  kDot3Result_Fail_InvalidLatitudeValue, ///< 유효하지 않은 위도 값
  kDot3Result_Fail_InvalidLongitudeValue, ///< 유효하지 않은 경도 값
  kDot3Result_Fail_InvalidElevationValue, ///< 유효하지 않은 고도 값
  kDot3Result_Fail_InvalidWsaHdrExtensionIdValue, ///< 유효하지 않은 WSA 확장필드 ID
  kDot3Result_Fail_InvalidWciExtensionIdValue, ///< 유효하지 않은 WSA Channel Info 확장필드 ID
  kDot3Result_Fail_InvalidWsiExtensionIdValue, ///< 유효하지 않은 WSA Service Info 확장필드 ID
  kDot3Result_Fail_InvalidWsaMessageType, ///< 유효하지 않은 WSA 메시지 유형
  kDot3Result_Fail_InvalidWsaVersionValue, ///< 유효하지 않은 WSA 버전
  kDot3Result_Fail_InvalidIPv6PrefixLenValue, ///< 유효하지 않은 IPv6 prefix length

  kDot3Result_Fail_NotSupportedYet, ///< 아직 지원하지 않음

};
/// @copydoc eDot3ResultCode
typedef int Dot3ResultCode;

/// PDU 크기 정의
/// @note
/// 본 라이브러리에서 사용되는 MPDU라는 용어는 코드 상에 특별한 언급이 없는한 CRC 제외한 MPDU이다.
/// kWsmBodyMaxSize 계산시 최소길이 WSMP헤더의 길이에 1을 더한 값을 빼는 이유는 Body가 최대길이일때,
/// Length 필드가 2바이트이기 때문이다.
///   - 최소길이 WSMP헤더의 마지막 1 바이트가 Length 필드이므로, 2바이트 Length 필드가 필요한 경우에는 1바이트만 더 추가된다.
enum eDot3PduSize {
  kPduSize_Min = 0,
  kNonQosMacHdrSize = 24, ///< Non QoS MAC 헤더 크기
  kQoSMacHdrSize = 26,  ///< QoS MAC 헤더 크기
  kMacCrcSize = 4,  ///< MAC CRC 필드 크기
  kLLCHdrSize = 2,  ///< LLC 헤더 크기
  kWsmpHdrMinSize = 4, ///< WSMP 헤더 최소 크기
  kWsmpHdrMaxSize = 18, ///< WSMP 헤더 최대 크기 (확장필드 모두 포함, PSID/WsmLength 최대값 사용 시)
  kMsduMaxSize = 2304,  ///< MSDU 최대크기(per 802.11-2012~)
  kWsmMinSize = kWsmpHdrMinSize, // WSM 최소 크기 (=4)
  kWsmMaxSize = (kMsduMaxSize - kLLCHdrSize), ///< WSM 최대 크기 (=2302)
  kWsmBodyMaxSize = (kWsmMaxSize - (kWsmpHdrMinSize+1)), ///< WSM헤더 길이가 최소(=5)일 때 가능한 WSM body 최대 크기 (=2297)
  kWsmBodySafeMaxSize = (kWsmMaxSize - kWsmpHdrMaxSize), ///< WSM헤더 길이가 최대(=18)일 때 가능한 WSM body 최대 크기 (=2284)
  kMpduMaxSize = (kQoSMacHdrSize + kMsduMaxSize), ///< CRC 제외한 MPDU 최대크기(=2330)
  kMpduMaxSizeWithCrc = (kQoSMacHdrSize + kMsduMaxSize + kMacCrcSize), ///< CRC 포함한 MPDU 최대크기(=2334)
  kWsmMpduMinSize = (kNonQosMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize), ///< CRC 제외한 MPDU 최소크기(=32)
  kWsmMpduMinSizeWithCrc = (kNonQosMacHdrSize + kLLCHdrSize + kWsmpHdrMinSize + kMacCrcSize), ///< CRC 포함한 MPDU 최소크기(=36)
  kPduSize_Max = kMpduMaxSize
};
/// @copydoc ePduSize
typedef uint16_t Dot3PduSize;
// TODO:: kMpduMaxSize, kWsmMpduMinSize, 검색 -> 각 루틴에서 CRC 포함 여부를 어떻게 할지 결정하고, 길이 비교시 맞는 값으로 비교하도록 수정 필요
///       모든 WSM API에서 사용되는 MPDU는 CRC 없는 것으로 하자

/// PSID (Provider Service IDentifier)
enum eDot3Psid {
  kDot3Psid_Min = 0,  ///< PSID 최소값
  kDot3Psid_Max = 0x1020407F, ///< PSID 최대값
  kDot3Psid_Ipv6Routing = 0x1020407E, ///< IPv6 Routing 용 PSID
  kDot3Psid_Wsa = 0x87  ///< WSA용 PSID
};
/// @copydoc eDot3Psid
typedef uint32_t Dot3Psid;


/**
 * PSR 관련 수
 */
enum eDot3PsrNum
{
  kDot3PsrNum_MaxNum = _PSR_MAX_NUM_, ///< PSR 테이블 내 엔트리 최대 개수 (per 1609.3-2016)
  kDot3PsrNum_MinIndex = 0,
  kDot3PsrNum_MaxIndex = (kDot3PsrNum_MaxNum - 1),
};
typedef unsigned int Dot3PsrNum;  ///< @copydoc eDot3PsrNum


/**
 * Provider Channel Info 관련 수
 */
enum eDot3PciNum
{
  kDot3PciNum_MaxNum = _PSR_MAX_NUM_, ///< Channel info 테이블 내 엔트리 최대 개수 (per 1609.3-2016)
  kDot3PciNum_MinIndex = 0,
  kDot3PciNum_MaxIndex = (kDot3PciNum_MaxNum - 1),
};
typedef unsigned int Dot3PciNum;  ///< @copydoc eDot3PciNum

/**
 * WSA Service info 관련 수
 */
enum eDot3WsiNum
{
  kDot3WsiNum_MinNum = 0,
  kDot3WsiNum_MaxNum = _WSA_SERVICE_INFO_MAX_NUM_,
  kDot3WsiNum_MinIndex = 0,
  kDot3WsiNum_MaxIndex = (kDot3WsiNum_MaxNum - 1),
};
typedef unsigned int Dot3WsiNum; ///< @copydoc eDot3WsiNum

/**
 * WSA channel info 관련 수
 *  TODO:: WSA 내 service info에서는 channel info의 index를 1부터 사용한다. 여기서도 그럴 필요가 있는가?
 */
enum eDot3WciNum
{
  kDot3WciNum_MinNum = 0,
  kDot3WciNum_MaxNum = _WSA_CHAN_INFO_MAX_NUM_,
  kDot3WciNum_MinIndex = 0,
  kDot3WciNum_MaxIndex = (kDot3WciNum_MaxNum - 1)
};
typedef unsigned int Dot3WciNum; ///< @copydoc eDot3WciNum

/// TimeSlot
enum eDot3TimeSlot {
  kDot3TimeSlot_0 = 0,  ///< TimeSlot0
  kDot3TimeSlot_1 = 1,  ///< TimeSlot1
  kDot3TimeSlot_Any = 2,  ///< TimeSlot0 또는 TimeSlot1
  kDot3TimeSlot_Continuous = 2  ///< Continuous 접속 (TimeSlot0 & TimeSlot1)
};
/// @copydoc eDot3TimeSlot
typedef uint32_t Dot3TimeSlot;

/// DataRate (500kbps 단위)
// TODO:: 20Mhz 도 고려하여 추가해야 함.
enum eDot3DataRate {
  kDot3DataRate_3Mbps = 6,
  kDot3DataRate_4p5Mbps = 9,
  kDot3DataRate_6Mbps = 12,
  kDot3DataRate_9Mbps = 18,
  kDot3DataRate_12Mbps = 24,
  kDot3DataRate_18Mbps = 36,
  kDot3DataRate_24Mbps = 48,
  kDot3DataRate_27Mbps = 54,
  kDot3DataRate_Min = kDot3DataRate_3Mbps,
  kDot3DataRate_Max = kDot3DataRate_27Mbps,
  kDot3DataRate_TxDefault = kDot3DataRate_6Mbps,  ///< 기본 송신 데이터레이트
  kDot3DataRate_Unknown = -999, ///< 알 수 없는 데이터레이트
};
/// @copydoc DataRate
typedef int32_t Dot3DataRate;


// TODO:: libwlanaccess에서는 송신파워/수신파워가 0.5dBm 단위이다. 통일성을 위해 libdot3에서도 0.5dBm 단위로 변경하자.
/// Power (1dBm 단위)
/**
 * Power 값 (dBm 단위)
 */
enum eDot3Power
{
  kDot3Power_Min = -128,      ///< 최소 파워
  kDot3Power_Max = 127,       ///< 최대 파워
  kDot3Power_MaxInClassC = 20,              ///< 클래스 C 최대 파워 (포트출력)
  kDot3Power_MaxEirpInClassC = 33,          ///< 클래스 C 최대 EIRP
  kDot3Power_TxDefault = kDot3Power_MaxInClassC,   ///< 기본 송신파워
  kDot3Power_Unknown = -999 ///< 알 수 없는 파워값
};
typedef int32_t Dot3Power;  /// @copydoc eDot3Power

/// Priority
enum eDot3Priority {
  kDot3Priority_Min = 0,  ///< 우선순위 최소값
  kDot3Priority_Max = 7,  ///< 우선순위 최대값
  kDot3Priority_Wsa = kDot3Priority_Max,  ///< WSA용 우선순위
  kDot3Priority_Unknown = -999  ///< 알 수 없는 우선순위(수신 시에만 사용)
};
/// @copydoc eDot3Priority
typedef int32_t Dot3Priority;

/// 주소 길이 (바이트 단위)
enum eDot3AddrSize {
  kDot3MacAddrSize = 6, ///< MAC 주소 길이
  kDot3Ipv6AddrSize = 16  ///< IPv6 주소 길이
};
/// @copydoc eDot3AddrSize
typedef uint32_t Dot3AddrSize;

/*
 * 채널번호
 */
enum eDot3ChannelNumber
{
  kDot3Channel_Min = 0, ///< 접속 가능한 최소 채널번호(=5.00GHz 중심주파수)
  kDot3Channel_Max = 200, ///< 접속 가능한 최대 채널번호(=6.00GHz 중심주파수)
  kDot3Channel_Unknown = -999,  ///< 알 수 없는 채널
  kDot3Channel_Any = -999,  ///< 임의의 채널

  // 한국 V2X 주파수 채널 대역
  kDot3Channel_KoreaV2XMin = 172,
  kDot3Channel_KoreaV2XCch = 180,
  kDot3Channel_KoreaV2XMax = 184,

  // 미국 V2X 주파수 채널 대역
  kDot3Channel_UsaV2XMin = 172,
  kDot3Channel_UsaV2X20MhzLow = 175,
  kDot3Channel_UsaV2X20MhzHigh = 181,
  kDot3Channel_UsaV2XCch = 178,
  kDot3Channel_UsaV2XMax = 184,
};
typedef int32_t Dot3ChannelNumber;  /// @copydoc eDot3ChannelNumber

/**
 * Operating class
 */
enum eDot3OperatingClass
{
  kDot3OperatingClass_Min = 0,
  kDot3OperatingClass_5G_10mhz = 17,		/* 미국 기준. IEEE 802.11-2012 */
  kDot3OperatingClass_5G_20mhz = 18,		/* 미국 기준. IEEE 802.11-2012 */
  kDot3OperatingClass_Max = 255,
  kDot3OperatingClass_Default = kDot3OperatingClass_5G_10mhz
};
typedef uint8_t Dot3OperatingClass; ///< @copydoc eDot3OperatingClass

/// RCPI(Received Channel Power Indicator)
enum eDot3Rcpi {
  kDot3Rcpi_Min = 0,/*=-110dBm*/ ///< 최소 RCPI 값
  kDot3Rcpi_Max = 220,/*=0dBm*/ ///< 최대 RCPI 값
  kDot3Rcpi_Unknown = -999 ///< 알 수 없는 RCPI
};
typedef int32_t Dot3Rcpi; /// @copydoc eDot3Rcpi;

/**
 * WSA 유형 (dot3WsaType)
 */
enum eDot3WsaType {
  kDot3SecuredWsa = 1,    ///< Unsecured WSA
  kDot3UnsecuredWsa = 2,  ///< Secured WSA
};
typedef uint32_t Dot3WsaType; ///< @copydoc eDot3WsaType

/**
 * WSA 식별번호 (Wsa Identifier)
 */
enum eDot3WsaIdentifier
{
  kDot3WsaMinId = 0,
  kDot3WsaMaxId = 15,
};
typedef uint8_t Dot3WsaIdentifier;  ///< @copydoc eDot3WsaIdentifier

/**
 * WSA content count (동일 WSA identifier에 대해 WSA가 변경될 때마다 1씩 증가)
 */
enum eDot3WsaContentCount
{
  kDot3WsaMinContentCount = 0,
  kDot3WsaMaxContentCount = 15,
};
typedef uint8_t Dot3WsaContentCount;  ///< @copydoc eDot3WsaContentCount

/**
 * WSA Repeat Rate
 *  - 5초당 전송되는 WSA의 개수
 */
enum eDot3WsaRepeatRate
{
  kDot3WsaRepeatRate_Min = 0,
  kDot3WsaRepeatRate_Max = 255,
};
typedef uint8_t Dot3WsaRepeatRate; ///< @copydoc eDot3WsaRepeatRate

/**
 * 채널 접속 유형 (dot3ProviderChannelAccess)
 */
enum eDot3ProviderChannelAccess {
  kDot3ProviderChannelAccess_Continuous = 0,          ///< TimeSlot0 과 TimeSlot1 에서 서비스를 제공 (=continuous)
  kDot3ProviderChannelAccess_AlternatingTimeSlot0Only = 1,  ///< TimeSlot0 에서 서비스 제공 (=alternating)
  kDot3ProviderChannelAccess_AlternatingTimeSlot1Only = 2,  ///< TimeSlot1 에서 서비스 제공 (=alternating)
  kDot3ProviderChannelAccess_Any = 3,                       ///< 어떤 형태든 가능
  kDot3ProviderChannelAccess_Default = kDot3ProviderChannelAccess_AlternatingTimeSlot1Only,
  kDot3ProviderChannelAccess_Min = kDot3ProviderChannelAccess_Continuous,
  kDot3ProviderChannelAccess_Max = kDot3ProviderChannelAccess_Any
};
typedef uint32_t Dot3ProviderChannelAccess; ///< @copydoc eDot3ProviderChannelAccess

/**
 * MAC 주소 형식
 */
#define MAC_ALEN 6
typedef uint8_t Dot3MacAddress[MAC_ALEN];

/**
 * IPv6 주소 형식
 */
#define IPV6_ALEN 16
#define IPV6_ADDR_STR_MAX_LEN 128
typedef uint8_t Dot3IPv6Address[IPV6_ALEN];

/**
 * PSR 상태 (dot3ProviderServiceStatus in MIB)
 */
enum eDot3ProviderServiceStatus {
  kDot3ProviderServiceStatus_Pending = 0,
  kDot3ProviderServiceStatus_Satisified = 1,
  kDot3ProviderServiceStatus_PartiallySatisfied = 2
};
typedef uint32_t Dot3ProviderServiceStatus; ///< @copydoc eDot3ProviderServiceStatus

/**
 * WSA count threshold
 *  - 유효한 WSA로 판단되기 위한 수신 개수
 *  - 이 값 이상 수신된 WSA 만 유효한 것으로 판단한다.
 */
enum eDot3WsaCountThreshold
{
  kDot3WsaCountThreshold_Min = 0,
  kDot3WsaCountThreshold_Max = 255,
};
typedef uint8_t Dot3WsaCountThreshold;  ///< @copydoc eDot3WsaCountThreshold

/**
 * WSA count threshold interval
 *  - wsa_cnt_threshold 를 카운트하기 위한 100ms 단위 인터벌
 *  - WSA count threshold interval 기간 동안 WSA count threshold 이상 수신된 WSA 만 유효한 것으로 판단한다.
 */
enum eDot3WsaCountThresholdInterval
{
  kDot3WsaCountThresholdInterval_Min = 1,
  kDot3WsaCountThresholdInterval_Max = 255
};
typedef uint8_t Dot3WsaCountThresholdInterval; ///< @copydoc kDot3WsaCountThresholdInterval


/**
 * Latitude (위도, 0.1 마이크로도 단위)
 */
enum eDot3Latitude
{
  kDot3Latitude_Min = -900000000,
  kDot3Latitude_Max = 900000001,
  kDot3Latitude_Unavailable = 900000001
};
typedef int32_t Dot3Latitude; ///< @copydoc eDot3Latitude


/**
 * Longitude (경도, 0.1 마이크로도 단위)
 */
enum eDot3Longitude
{
  kDot3Longitude_Min = -1800000000,
  kDot3Longitude_Max = 1800000001,
  kDot3Longitude_Unavailable = 1800000001
};
typedef int32_t Dot3Longitude; ///< @copydoc eDot3Longitude


/**
 * Elevation (고도, 10cm 단위)
 */
enum eDot3Elevation
{
  kDot3Elevation_Min = -4096,
  kDot3Elevation_Max = 61439, ///< 6143.9 m
  kDot3Elevation_Unavailable = -4096
};
typedef int32_t Dot3Elevation; ///< @copydoc eDot3Elevation


/**
 * WSA Advertiser id 길이
 */
enum eDot3WsaAdvertiserIdLen
{
  kDot3WsaAdvertiserIdLen_Min = 1,
  kDot3WsaAdvertiserIdLen_Max = 32,
};
typedef uint8_t Dot3WsaAdvertiserIdLen; ///< @copydoc eDot3WsaAdvertiserIdLen

/**
 * WSA 헤더에 수납되는 버전 값
 */
enum eDot3WsaVersion
{
  kDot3WsaVersion_Min = 0,
  kDot3WsaVersion_Max = 15,
  kDot3WsaVersion_Current = 3,
};
typedef unsigned int Dot3WsaVersion; ///< @copydoc eDot3WsaVersion

/**
 * WSA 헤더에 수납되는 Change count 값
 */
enum eDot3WsaChangeCount
{
  kDot3WsaChangeCount_Min = 0,
  kDot3WsaChangeCount_Max = 15,
};
typedef unsigned int Dot3WsaChangeCount; ///< @copydoc eDot3WsaChangeCount

/**
 * WSA 의 service info 필드의 Channel index 값
 *  - 1부터 시작한다.
 */
enum eDot3WsaChannelIndex
{
  kDot3WsaChannelIndex_NotUsed = 0,
  kDot3WsaChannelIndex_Min = 1,
  kDot3WsaChannelIndex_Max = _WSA_CHAN_INFO_MAX_NUM_
};
typedef unsigned int Dot3WsaChannelIndex; ///< @copydoc eDot3WsaChannelIndex

/**
 * Provider Service Context 길이
 */
enum eDot3PscLen
{
  kDot3PscLen_Min = 0,
  kDot3PscLen_Max = 31,
};
typedef unsigned int Dot3PscLen; ///< @copydoc eDot3PscLen

/**
 * WRA 에 수납되는 Router Life Time 값
 */
enum eDot3WraRouterLifeTime
{
  kDot3WraRouterLifeTime_Min = 0,
  kDot3WraRouterLifeTime_Max = 65535
};
typedef uint16_t Dot3WraRouterLifeTime; ///< @copydoc eDot3WraRouterLifeTime

/**
 * IPv6 프리픽스 길이를 나타내는 값
 */
enum eDot3IPv6PrefixLen
{
  kDot3IPv6PrefixLen_Min = 1,
  kDot3IPv6PrefixLen_Max = 128
};
typedef uint8_t Dot3IPv6PrefixLen; ///< @copydoc eDot3IPv6PrefixLen

/**
 * Provider Service Context 형식
 */
struct Dot3Psc
{
  Dot3PscLen len;
  uint8_t psc[kDot3PscLen_Max + 1];
};

/**
 * WSA Advertiser id
 */
struct Dot3WsaAdvertiserId
{
  Dot3WsaAdvertiserIdLen len;
  uint8_t id[kDot3WsaAdvertiserIdLen_Max + 1];
};

/**
 * WSA 2DLocation
 *  - WSA 헤더 확장필드에 수납되는 2DLocation
 */
struct Dot3WsaTwoDLocation
{
  Dot3Latitude latitude;
  Dot3Longitude longitude;
};

/**
 * WSA 3DLocation
 *  - WSA 헤더 확장필드에 수납되는 3DLocation
 */
struct Dot3WsaThreeDLocation
{
  Dot3Latitude latitude;
  Dot3Longitude longitude;
  Dot3Elevation elevation;
};



/**
 * WSA 헤더
 */
struct Dot3WsaHdr
{
  // 필수필드
  Dot3WsaVersion version;
  Dot3WsaIdentifier wsa_id;
  Dot3WsaContentCount content_count;

  // 확장필드
  struct {
    bool repeat_rate;
    bool twod_location;
    bool threed_location;
    bool advertiser_id;
  } extensions;
  Dot3WsaRepeatRate repeat_rate;
  struct Dot3WsaTwoDLocation twod_location;
  struct Dot3WsaThreeDLocation threed_location;
  struct Dot3WsaAdvertiserId advertiser_id;
};


/// 송신 WSM MPDU 생성을 위한 정보 구조체.
/// WSM 헤더와 MAC 헤더를 생성하는데 사용된다.
struct Dot3WsmMpduTxParams
{
  struct {
    bool chan_num;  ///< WSM-N 헤더에 ChannelNumber 확장필드 존재 여부
    bool datarate;  ///< WSM-N 헤더에 DataRate 확장필드 존재 여부
    bool transmit_power;  ///< WSM-N 헤더에 TransmitPowerUsed 확장필드 존재 여부
    bool chan_load;  ///< WSM-N 헤더에 ChannelLoad 확장필드 존재 여부
  } hdr_extensions; ///< WSM-N 헤더의 각 확장필드 존재 여부

  uint32_t ifindex; ///< WSM을 전송할 네트워크인터페이스 식별번호
  Dot3ChannelNumber chan_num; ///< WSM을 전송할 채널번호
  Dot3TimeSlot timeslot; ///< WSM을 전송할 TimeSlot
  Dot3DataRate datarate; ///< WSM을 전송할 DataRate
  Dot3Power transmit_power; ///< WSM을 전송할 파워
  uint32_t chan_load; ///<  사용되지 않음. (표준에 기능 및 사용방법에 대한 언급이 없음)
  Dot3Priority priority; ///< WSM을 전송할 우선 순위
  uint64_t expiry; ///< WSM 만기시각
  uint8_t dst_mac_addr[kDot3MacAddrSize]; ///< 목적지 MAC 주소
  Dot3Psid psid;  ///< PSID
  uint8_t src_mac_addr[kDot3MacAddrSize]; ///< 송신지(자신) MAC 주소
};

/// 수신된 WSM MPDU의 파싱된 파라미터 정보가 저장되는 구조체
/// 수신된 WSM 헤더와 MAC 헤더 필드 등으로부터 정보가 저장된다.
struct Dot3WsmMpduRxParams
{
  uint32_t ifindex; ///< WSM이 수신된 네트워크인터페이스 식별번호
  uint32_t version; ///< WSMP version
  Dot3ChannelNumber tx_chan_num; ///< WSM-N 헤더의 ChannelNumber 확장필드 값 (필드가 없을 경우 Unknown으로 설정됨)
  Dot3ChannelNumber rx_chan_num; ///< 실제 수신된 채널번호 (=수신인터페이스가 접속 중인 채널번호)
  Dot3DataRate tx_datarate; ///< WSM-N 헤더의 DataRate 확장필드 값 (필드가 없을 경우 Unknown으로 설정됨)
  Dot3DataRate rx_datarate; ///< 실제 수신된 데이터레이트
  Dot3Power tx_power; ///< WSM-N 헤더의 TransmitPowerUsed 확장필드 값 (필드가 없을 경우 Unknown으로 설정됨)
  Dot3Power rx_power; ///< 실제 수신된 파워
  Dot3Rcpi rcpi; ///< RCPI
  uint32_t chan_load; ///< 사용되지 않음. (표준에 기능 및 사용방법에 대한 언급이 없음)
  Dot3Priority priority; ///< 수신된 WSM의 우선순위
  uint8_t src_mac_addr[kDot3MacAddrSize]; ///< 송신지 MAC 주소
  uint8_t dst_mac_addr[kDot3MacAddrSize]; ///< 목적지 MAC 주소 (내 MAC주소 또는 멀티캐스트, 브로드캐스트)
  Dot3Psid psid; ///< PSID
};

/// WSR 정보
struct Dot3Wsr
{
  Dot3Psid psid;  ///< PSID
};


/**
 * PSR 정보 (=dot3ProviderServiceRequestTableEntry in MIB)
 * Dot3_AddPsr() API 파라미터
 */
struct Dot3Psr
{
  // 필수필드
  Dot3WsaIdentifier wsa_id;             ///< WSA 헤더에 수납되는 WSA identifier
  Dot3Psid psid;                        ///< PSID
  Dot3ChannelNumber service_chan_num;   ///< 서비스가 제공되는 채널
  Dot3ProviderChannelAccess chan_access;  ///< 채널접속 유형

  // 옵션필드
  struct {
    bool psc;
    bool provider_mac_addr;
    bool rcpi_threshold;
    bool wsa_cnt_threshold;
    bool wsa_cnt_threshold_interval;
  } present;  ///< 각 옵션필드의 존재 여부
  struct Dot3Psc psc;                   ///< Provider Service Context
  bool ip_service;                      ///< IP 서비스인지 여부
  Dot3IPv6Address ipv6_address;         ///< IP 서비스 제공자의 IPv6 주소
  uint16_t service_port;                ///< IP 서비스인 경우, IP 서비스 제공자의 포트번호
  Dot3MacAddress provider_mac_addr;     ///< (서비스 제공자와 WSA 전송자가 다를 경우) 서비스 제공자의 MAC 주소
  Dot3Rcpi rcpi_threshold;              ///< 유효한 WSA로 판단되기 위한 RCPI 임계값
  Dot3WsaCountThreshold wsa_cnt_threshold;                    ///< 유효한 WSA로 판단되기 위한 수신 개수
  Dot3WsaCountThresholdInterval wsa_cnt_threshold_interval;   ///< wsa_cnt_threshold 를 카운트하기 위한 100ms 단위 인터벌(1~)

  // (현재) 미사용필드
  Dot3MacAddress dst_mac_addr; ///< (미사용 항목) WSA 목적지 MAC 주소. PSR 단위가 아닌 통합 프로세스 단위에서 제어한다.
  Dot3WsaType wsa_type;        ///< (미사용 항목) WSA 유형. PSR 단위가 아닌 통합 프로세스 단위에서 제어한다.
  bool best_available_chan;    ///< (미사용 항목) dot3가 적절한 채널을 선택한다. PSR 단위가 아닌 통합 프로세스 단위에서 제어한다.
  Dot3ChannelNumber wsa_chan_num; ///< (미사용 항목) WSA가 전송되는 채널. PSR 단위가 아닌 통합 프로세스 단위에서 제어한다.
  Dot3WsaRepeatRate repeat_rate;  ///< (미사용 항목) 5초당 WSA 전송 회수. PSR 단위가 아닌 통합 프로세스 단위에서 제어한다.
  // Info Elements Indicator - WSA 헤더 확장필드 포함 여부 -> Dot3_ConstructWsa()에서 사용.
  // Signature Lifetime - 서명 유효기간 -> PSR 단위가 아닌 시스템 단위에서 제어한다.
};

/**
 * Provider Channel Info 정보 (=dot3ProviderChannelInfoTable in MIB)
 */
struct Dot3Pci
{
  Dot3OperatingClass operating_class;     ///< operating class
  Dot3ChannelNumber chan_num;             ///< 채널번호
  Dot3Power transmit_power_level;         ///< 이 채널에서 사용할 수 있는 최대 EIRP(dBm)
  Dot3DataRate datarate;                  ///< 이 채널에서 사용할 수 있는 데이터레이트 또는 최소 데이터레이트
  bool adaptable_datarate;                ///< true: 본 채널에서 datarate 만 사용 가능. false: datarate 이상 사용 가능
  //Dot3EdcaParameterSet edca_param_set;  // TODO::
};

/**
 * WSA Service info 정보 (WSA에 수납되는 Service info)
 */
struct Dot3Wsi
{
  // 필수필드
  Dot3Psid psid;
  Dot3WsaChannelIndex channel_index;

  // 확장필드
  struct {
    bool psc;
    bool ipv6_address;
    bool service_port;
    bool provider_mac_address;
    bool rcpi_threshold;
    bool wsa_cnt_threshold;
    bool wsa_cnt_threshold_interval;
  } extensions;
  struct Dot3Psc psc;
  Dot3IPv6Address ipv6_address;
  uint16_t service_port;
  Dot3MacAddress provider_mac_address;
  Dot3Rcpi rcpi_threshold;
  Dot3WsaCountThreshold wsa_cnt_threshold;
  Dot3WsaCountThresholdInterval wsa_cnt_threshold_interval;
};

/**
 * WSA Channel Info 정보 (WSA에 수납되는 Channel info)
 */
struct Dot3Wci
{
  // 필수 필드
  Dot3OperatingClass operating_class;     ///< operating class
  Dot3ChannelNumber chan_num;             ///< 채널번호
  Dot3Power transmit_power_level;         ///< 이 채널에서 사용할 수 있는 최대 EIRP(dBm)
  Dot3DataRate datarate;                  ///< 이 채널에서 사용할 수 있는 데이터레이트 또는 최소 데이터레이트
  bool adaptable_datarate;                ///< true: 본 채널에서 datarate 만 사용 가능. false: datarate 이상 사용 가능

  // 확장필드
  struct {
    bool edca_param_set;
    bool chan_access;
  } extension;
  //Dot3EdcaParameterSet edca_param_set;  // TODO::
  Dot3ProviderChannelAccess chan_access;  ///< 본 채널에 적용되는 채널접속 유형
};

/**
 * WAVE Routing Advertisement 정보 (WSA에 수납되는 WRA)
 */
struct Dot3Wra
{
  // 필수 필드
  Dot3WraRouterLifeTime router_lifetime;  ///< Router lifetime
  Dot3IPv6Address ip_prefix;  ///< IP prefix
  Dot3IPv6PrefixLen ip_prefix_len; ///< IP prefix length
  Dot3IPv6Address default_gw; ///< Default gateway IP 주소
  Dot3IPv6Address primary_dns;  ///< Primary DNS

  // 확장 필드
  struct {
    bool secondary_dns;
    bool gatewya_mac_addr;
  } present;
  Dot3IPv6Address secondary_dns;  ///< Secondary DNS
  Dot3MacAddress gateway_mac_addr;  ///< Default gateway MAC 주소
};

/**
 * Dot3_ConstructWsa() 파라미터
 */
struct Dot3ConstructWsaParams
{
  struct {
    bool wra;
  } present;

  struct Dot3WsaHdr hdr;
  struct Dot3Wra wra;
};


/**
 * Dot3_ParseWsa() 파라미터
 */
struct Dot3ParseWsaParams
{
  struct {
    bool wra;
  } present;

  struct Dot3WsaHdr hdr;
  Dot3WsiNum wsi_num;
  struct Dot3Wsi wsis[kDot3WsiNum_MaxNum];
  Dot3WciNum wci_num;
  struct Dot3Wci wcis[kDot3WciNum_MaxNum];
  struct Dot3Wra wra;
};

#endif //V2X_LIBDOT3_DOT3_TYPES_H
