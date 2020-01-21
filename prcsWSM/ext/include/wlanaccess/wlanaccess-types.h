/**
 * @file wlanaccess-types.h
 * @date 2019-05-08
 * @author gyun
 * @brief wlanaccess 라이브러리의 Open API 함수에서 사용되는 자료 유형을 정의한 헤더파일
 */


#ifndef LIBWLANACCESS_WLANACCESS_TYPES_H
#define LIBWLANACCESS_WLANACCESS_TYPES_H


#include <stdint.h>


/// @brief 처리결과 코드
enum eAlResultCode {
  kAlResult_Success = 0,      ///< 성공
  kAlResult_InvalidIfNum,     ///< 유효하지 않은 인터페이스 개수
  kAlResult_InvalidIfIndex,   ///< 유효하지 않은 인터페이스 식별번호
  kAlResult_InvalidChannel,   ///< 유효하지 않은 채널
  kAlResult_InvalidTimeSlot,  ///< 유효하지 않은 TimeSlot
  kAlResult_PollingFailed,    ///< 이벤트 폴링 실패
  kAlResult_NoMemory,         ///< 메모리 할당 실패
  kAlResult_DevSpecificError, ///< 디바이스(하드웨어) 관련 에러
  kAlResult_NotSupported,     ///< 지원하지 않음
  kAlResult_NotSupportedYet,  ///< 아직 지원하지 않음 (구현 예정)
  kAlResult_NullParameters,   ///< 널 파라미터
  kAlResult_InvalidMpduSize,  ///< 유효하지 않은 MPDU 길이
  kAlResult_InvalidMacAddress, ///< 유효하지 않은 MAC 주소
};
/// @copydoc eAlResultCode
typedef int AlResultCode;

/**
 * @brief 로그메시지 출력 레벨
 *
 * 높은 레벨은 낮은 레벨의 범위를 포함한다.
 * 즉, 로그변수가 높은 레벨로 설정되어 있으면, 그 하위레벨에 해당되는 로그는 함께 출력된다.
 */
enum eAlLogLevel {
  kAlLogLevel_none = 0, ///< 아무 로그도 출력하지 않는다.
  kAlLogLevel_err,      ///< 에러 로그
  kAlLogLevel_init,     ///< 초기화 절차에 관련된 로그
  kAlLogLevel_config,   ///< 설정에 관련된 로그 (API 엔트리 포함)
  kAlLogLevel_event,    ///< 각종 이벤트(패킷 송수신 포함)에 관련된 로그
  kAlLogLevel_dump,     ///< 상세내용 로그(송수신 패킷 덤프 데이터 등)
  kAlLogLevel_all,      ///< 모든 로그
};
/// @copydoc eLogLevel
typedef int AlLogLevel;

/// @brief 인터페이스 식별번호
enum eAlIfIndex {
  kAlIfIndex_min = 0,
  kAlIfIndex_max = 255,
};
/// @copydoc eAlIfIndex
typedef uint8_t AlIfIndex;

/// @brief MPDU 길이 (MAC 헤더, MSDU, MAC CRC 포함)
enum eAlMpduSize {
  kAlMacNonQoSHeadrSize = 24,  /// Non-QoS MSDU에 대한 MAC 헤더 길이 (OCB통신(=V2X통신)에서는 사용되지 않음)
  kAlMacQoSHeaderSize = 26,    /// QoS MSDU에 대한 MAC 헤더 길이 (OCB(=V2X통신)통신에 사용됨)
  kAlMacCrcSize = 4,           /// MAC CRC 길이
  kAlMsduMinSize = 0,         /// MSDU 최소길이
  kAlMsduMaxSize = 2304,      /// MSDU 최대길이 (IEEE 802.11)
  kAlMpduMinSize = (kAlMacNonQoSHeadrSize + kAlMsduMinSize), /// MPDU 최소길이 (데이터 또는 관리프레임)
  kAlMpduMinSizeWithCrc = (kAlMpduMinSize + kAlMacCrcSize), /// MPDU 최소길이 (데이터 또는 관리프레임)
  kAlMpduMaxSize = (kAlMacQoSHeaderSize + kAlMsduMaxSize),   /// MPDU 최대길이
  kAlMpduMaxSizeWithCrc = (kAlMpduMaxSize + kAlMacCrcSize),   /// MPDU 최대길이
};
/// @copydoc eAlMpduSize
typedef uint16_t AlMpduSize;

/// @brief 채널번호
enum eAlChannel {
  kAlChannel_min = 0,
  kAlChannel_max = 255,
};
/// @copydoc eAlChannel
typedef uint8_t AlChannel;

/// @brief TimeSlot 식별자
enum eAlTimeSlot {
  kAlTimeSlot_0,    /// TimeSlot0
  kAlTimeSlot_1,    /// TimeSlot1
  kAlTimeSlot_both, /// Both timeslot(ex: continuous)
  kAlTimeSlot_any = kAlTimeSlot_both, /// Any timeslot (예: 패킷 전송 시, 시간슬롯 상관없이 전송할 때)
  kAlTimeSlot_min = kAlTimeSlot_0,
  kAlTimeSlot_max = kAlTimeSlot_both
};
/// @copydoc eAlTimeSlot
typedef uint8_t AlTimeSlot;

/// @brief 액세스계층 MPDU 전송 파라미터
struct AlMpduTxParams {
  AlChannel channel;    /// 전송 채널
  AlTimeSlot timeslot;  /// 전송 TimeSlot
  int8_t txpower;       /// 송신파워 (0.5dBm 단위)
  uint8_t datarate;     /// 송신데이터레이트 (500kbps 단위)
  uint64_t expiry;      /// (현재시간으로부터의)유효기간 (마이크로초 단위)
};

/// @brief 액세스계층 MPDU 수신 파라미터
struct AlMpduRxParams {
  AlIfIndex ifindex;    /// MPDU가 수신된 인터페이스 식별번호
  AlTimeSlot timeslot;  /// MPDU가 수신된 TimeSlot
  AlChannel channel;    /// MPDU가 수신된 채널번호
  int16_t rxpower;       /// MPDU 수신 파워 (0.5dBm 단위). -32768=Unknown
  uint8_t rcpi;         /// MPDU RCPI
  uint8_t datarate;     /// MPDU 수신 데이터레이트
};

/// @brief MAC 주소 형식
typedef uint8_t AlMacAddress[6];

/// @brief OUI 형식 (MAC 주소의 상위 24비트)
typedef uint8_t AlOui[3];

/// @brief 전송요청 처리 결과 코드
enum eAlTxResultCode {
  kAlTxResult_Success = 0,      /// 성공
  kAlTxResult_DevSpecificErr,   /// 디바이스 의존적 실패
};
/// @copydoc eAlTxResultCode
typedef int AlTxResultCode;

/// @brief 액세스계층에서 발생하는 에러이벤트 코드
enum eAlErrorCode {
  kAlError_unspecified,
};
/// @copydoc eAlErrorCode
typedef int AlErrorCode;

/// @brief 송신통계정보
struct AlTxStatstics {

};

/// @brief 수신통계정보
struct AlRxStatstics {

};

#endif //LIBWLANACCESS_WLANACCESS_TYPES_H
