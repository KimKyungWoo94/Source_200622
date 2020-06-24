/**
 * @file wlanaccess-api.h
 * @date 2019-05-08
 * @author gyun
 * @brief wlanaccess 라이브러리의 Open API 함수를 정의한 헤더파일
 */

#ifndef LIBWLANACCESS_WLANACCESS_API_H
#define LIBWLANACCESS_WLANACCESS_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wlanaccess-types.h"

/**
 * @brief 액세스계층 라이브러리를 초기화한다.
 *
 * @param log_level 로그메시지 출력 레벨
 * @return          성공시 액세스계층 라이브러리가 지원하는 인터페이스의 수, 실패시 음수(-AlResultCode)
 *
 * 어플리케이션은 액세스계층라이브러리의 다른 모든 API를 호출하기 전에 본 API를 호출해야 한다.
 *
 * 본 API 호출 시, 액세스계층 디바이스와 드라이버도 기본 설정으로 초기화된다.
 * 따라서, 액세스계층라이브러리를 이용하여 액세스계층 디바이스에 접근하는 다른 프로세스의 동작에 영향을 줄 수 있다.
 * 시스템 부팅 후 최초 1회만 호출하는 것이 권장된다. (전용 유틸리티 사용)
 */
int Al_Init(const AlLogLevel log_level);


/**
 * @brief 액세스계층 라이브러리를 오픈한다.
 *
 * @param log_level @ref Al_Init
 * @return          @ref Al_Init
 *
 * Al_Init() API와 동일한 기능을 수행한다.
 * 다만 Al_Init() API 와 다르게, 액세스계층 디바이스와 드라이버를 기본 설정으로 초기화하지 않는다.
 * 액세스계층 라이브러리를 사용하는 일반 어플리케이션에서는 Al_Init()이 아닌 본 API를 호출하는 것이 좋다.
 */
int Al_Open(const AlLogLevel log_level);


/**
 * @brief 특정 인터페이스를 통해 MPDU를 전송한다.
 * @param ifindex MPDU를 전송할 인터페이스 식별번호
 * @param mpdu 전송할 MPDU. MAC CRC 필드는 채우지 않아도 된다.
 * @param mpdu_size mpdu의 크기 (MAC 헤더 + MSDU. CRC는 불포함)
 * @param txparams MPDU 전송 파라미터들
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_TransmitMpdu(
  const AlIfIndex ifindex,
  const uint8_t *const mpdu,
  const AlMpduSize mpdu_size,
  const struct AlMpduTxParams *const txparams);

/**
 * @brief 특정 인터페이스에 대해 채널 접속을 요청한다.
 * @param ifindex 채널 접속할 인터페이스 식별번호
 * @param ts0_chan TimeSlot0에 접속할 채널번호
 * @param ts1_chan TimeSlot1에 접속할 채널번호
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_AccessChannel(const AlIfIndex ifindex, const AlChannel ts0_chan, const AlChannel ts1_chan);

/**
 * @brief 특정 인터페이스에 대해 현재 접속 중인 채널을 확인한다.
 * @param ifindex 인터페이스 식별번호
 * @param ts0_chan TimeSlot0에 접속 중인 채널번호가 반환된다.
 * @param ts1_chan TimeSlot1에 접속 중인 채널번호가 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_GetCurrentChannel(const AlIfIndex ifindex, AlChannel *const ts0_chan, AlChannel *const ts1_chan);

/**
 * @brief 특정 인터페이스에 대해 채널접속 해제를 요청한다.
 * @param ifindex 채널접속을 해제할 인터페이스 식별번호
 * @param timeslot 채널접속을 해제할 TimeSlot (0,1,both)
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_ReleaseChannel(const AlIfIndex ifindex, const AlTimeSlot timeslot);

/**
 * @brief 특정 인터페이스의 MAC 주소를 설정한다.
 * @param ifindex MAC 주소를 설정할 인터페이스 식별번호
 * @param addr 설정할 MAC 주소
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_SetIfMacAddress(const AlIfIndex ifindex, const AlMacAddress addr);

/**
 * @brief 특정 인터페이스의 MAC 주소를 랜덤하게 설정한다. (OUI는 유지된다)
 * @param ifindex MAC 주소를 설정할 인터페이스 식별번호
 * @param oui OUI를 전달한다.
 * @param addr 랜덤하게 설정된 MAC 주소가 저장되어 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_SetIfMacAddressRandom(const AlIfIndex ifindex, const AlOui oui, AlMacAddress addr);

/**
 * @brief 특정 인터페이스의 MAC 주소를 랜덤하게 설정한다. (OUI도 랜덤하게 설정된다)
 * @param ifindex MAC 주소를 설정할 인터페이스 식별번호
 * @param addr 랜덤하게 설정된 MAC 주소가 저장되어 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_SetIfMacAddressFullRandom(const AlIfIndex ifindex, AlMacAddress addr);

/**
 * @brief 특정 인터페이스의 MAC 주소를 확인한다.
 * @param ifindex MAC 주소를 확인할 인터페이스 식별번호
 * @param addr_index MAC 주소 설정번호
 *                   - 단일 인터페이스에 다수의 MAC 주소 설정이 가능한 경우, 설정번호를 전달한다.
 *                     사용 가능한 설정번호의 범위는 하드웨어에 따라 다를 수 있다.
 *                   - 단일 MAC 주소만 설정 가능할 경우에는 0을 전달한다.
 * @param addr MAC 주소가 저장되어 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_GetIfMacAddress(const AlIfIndex ifindex, AlMacAddress addr);

/**
 * @brief 특정 인터페이스에 대한 송신통계정보를 확인한다.
 * @param ifindex 인터페이스 식별번호
 * @param stats 송신통계정보가 저장되어 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_GetTxStatistics(const AlIfIndex ifindex, struct AlTxStatstics *const stats);

/**
 * @brief 특정 인터페이스의 송신통계정보를 초기화한다.
 * @param ifindex 인터페이스 식별번호
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_ClearTxStatistics(const AlIfIndex ifindex);

/**
 * @brief 특정 인터페이스에 대한 수신통계정보를 확인한다.
 * @param ifindex 인터페이스 식별번호
 * @param stats 수신통계정보가 저장되어 반환된다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_GetRxStatistics(const AlIfIndex ifindex, struct AlRxStatstics *const stats);

/**
 * @brief 특정 인터페이스의 수신통계정보를 초기화한다.
 * @param ifindex 인터페이스 식별번호
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 */
int Al_ClearRxStatistics(const AlIfIndex ifindex);

/**
 * @brief 채널접속요청(Al_AccessChannel())에 대한 결과처리 콜백함수를 등록한다.
 * @param ProcessAccessChannelResultCallback 콜백함수 포인터
 *
 * 어플리케이션은 액세스계층으로부터 채널접속요청처리에 대한 결과를 수신받기 위해 본 함수를 통해 콜백함수를 등록한다.
 * 액세스계층은 채널접속요청에 대한 처리가 완료되면 등록된 콜백함수를 호출한다.
 * 등록된 콜백함수가 호출되기 위해서는, 어플리케이션 쓰레드에서 Al_PollEvent()를 호출해야 한다.
 *
 * 콜백함수의 각 파라미터의 의미는 다음과 같다.
 *  - ifindex: 채널접속이 수행된 인터페이스 식별번호
 */
void Al_RegisterCallbackAccessChannelResult(
  void (*ProcessAccessChannelResultCallback)(const AlIfIndex ifindex));

/**
 * @brief MAC주소 설정요청(Al_SetIfMacAddress())에 대한 결과처리 콜백함수를 등록한다.
 * @param ProcessSetIfMacAddressResultCallback 콜백함수 포인터
 *
 * 어플리케이션은 액세스계층으로부터 MAC주소설정요청처리에 대한 결과를 수신받기 위해 본 함수를 통해 콜백함수를 등록한다.
 * 액세스계층은 MAC주소설정요청에 대한 처리가 완료되면 등록된 콜백함수를 호출한다.
 * 등록된 콜백함수가 호출되기 위해서는, 어플리케이션 쓰레드에서 Al_PollEvent()를 호출해야 한다.
 *
 * 콜백함수의 각 파라미터의 의미는 다음과 같다.
 *  - ifindex: MAC주소가 설정된 인터페이스 식별번호
 */
void Al_RegisterCallbackSetIfMacAddressResult(
  void (*ProcessSetIfMacAddressResultCallback)(const AlIfIndex ifindex));

/**
 * @brief 송신요청(Al_TransmitMpdu())에 대한 결과처리 콜백함수를 등록한다.
 * @param ProcessTransmitResultCallback 콜백함수 포인터
 *
 * 어플리케이션은 액세스계층으로부터 전송요청처리에 대한 결과를 수신받기 위해 본 함수를 통해 콜백함수를 등록한다.
 * 액세스계층은 송신요청 처리가 완료되면 등록된 콜백함수를 호출한다.
 * 등록된 콜백함수가 호출되기 위해서는, 어플리케이션 쓰레드에서 Al_PollEvent()를 호출해야 한다.
 *
 * 콜백함수의 각 파라미터의 의미는 다음과 같다.
 *   - result : 송신 처리 결과
 *   - dev_specific_err_code: 디바이스별로 별도로 정의된 에러코드. result가 음수(=실패)일 경우에만 의미가 있다.
 *                            본 코드 값은 디바이스 별로 상이하므로, 각 디바이스별 에러코드를 확인해야 한다.
 */
void Al_RegisterCallbackTransmitResult(
  void (*ProcessTransmitResultCallback)(const AlTxResultCode result, const int dev_specific_errcode));

/**
 * @brief MPDU 수신처리 콜백함수를 등록한다.
 * @param ProcessRxMpduCallback 콜백함수 포인터
 *
 * 어플리케이션은 수신된 MPDU를 액세스계층으로부터 전달 받기 위해 본 함수를 통해 콜백함수를 등록한다.
 * 액세스계층은 MPDU가 수신되면 등록된 콜백함수를 호출한다.
 * 등록된 콜백함수가 호출되기 위해서는, 어플리케이션 쓰레드에서 Al_PollEvent()를 호출해야 한다.
 *
 * 콜백함수의 각 파라미터의 의미는 다음과 같다.
 *   - mpdu : 수신된 MPDU
 *   - mpdu_size : 수신된 MPDU의 길이
 *   - rxparams : 수신패킷정보
 *
 * 본 콜백함수를 통해 어플리케이션으로 전달된 인자(MPDU 및 수신패킷정보)들은 콜백함수가 종료되면 삭제되므로,
 * 어플리케이션에서 해당 데이터를 나중에 사용하고자 할 경우에는 해당 데이터를 복사해 두어야 한다.
 * (콜백함수 내 지역변수는 사용 불가. 전역변수 또는 동적할당메모리 사용)
 */
void Al_RegisterCallbackRxMpdu(
  void (*ProcessRxMpduCallback)(
    const uint8_t *const mpdu,
    const AlMpduSize mpdu_size,
    const struct AlMpduRxParams *const rxparams));

/**
 * @brief TimeSlot1 -> TimeSlot0 시점에 호출될 콜백함수를 등록한다.
 * @param ProcessTimeSlot0EventCallback 콜백함수 포인터
 */
void Al_RegisterCallbackTimeSlot0Event(void (*ProcessTimeSlot0EventCallback)(void));

/**
 * @brief TimeSlot0 -> TimeSlot1 시점에 호출될 콜백함수를 등록한다.
 * @param ProcessTimeSlot0EventCallback 콜백함수 포인터
 */
void Al_RegisterCallbackTimeSlot1Event(void (*ProcessTimeSlot1EventCallback)(void));

/**
 * @brief 에러 발생 시 처리할 콜백함수를 등록한다.
 * @param ProcessErrorEventCallback 콜백함수 포인터
 */
void Al_RegisterCallbackErrorEvent(void (*ProcessErrorEventCallback)(const AlErrorCode err));

/**
 * @brief 액세스계층으로부터 콜백함수가 호출되도록 하기 위해 호출한다.
 * @return 성공시 0, 실패시 음수(-AlResultCode)
 *
 * 어플리케이션에서는 쓰레드 등을 생성하여 루프를 돌며 본 API를 호출함으로써, 액세스계층의 콜백함수 호출이 가능하게 한다.
 * 콜백함수 호출 시마다 본 함수도 리턴된다.
 * 주의사항: 콜백이 있는 요청 API 호출 전에 폴링 쓰레드를 먼저 생성해야 한다.
 */
int Al_PollEvent(void);

#ifdef __cplusplus
}
#endif

#endif //LIBWLANACCESS_WLANACCESS_API_H
