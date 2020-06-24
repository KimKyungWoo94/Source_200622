//
// Created by gyun on 2019-06-04.
//

#ifndef V2X_LIBDOT3_DOT3_API_H
#define V2X_LIBDOT3_DOT3_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dot3-types.h"

/**
 * @brief dot3 라이브러리를 초기화한다. 다른 모든 dot3 API 호출 전에 호출되어야 한다.
 * @param log_level 로그출력레벨
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 *
 * 로그출력레벨
 *  0 : 아무 로그도 출력하지 않는다.
 *  1 : 에러 로그
 *  2 : 초기화 절차에 관련된 로그
 *  3 : 설정에 관련된 로그 (API 엔트리 포함)
 *  4 : 각종 이벤트(패킷 송수신 포함)에 관련된 로그
 *  5 : 상세내용 로그(송수신 패킷 덤프 데이터 등)
 *  6 : 모든 로그를 출력한다.
 */
int Dot3_Init(int log_level);

/**
 * @brief 전송하고자 하는 WSM(WAVE Short Message) MPDU(MAC Protocol Data Unit)의 생성을 요청한다.
 * @param params        WSM 송신파라미터정보(WSM 헤더 및 MAC 헤더를 생성하는데 사용되는 정보) 구조체의 포인터를 전달한다.
 *                      NULL 은 전달할 수 없다.
 * @param payload       상위계층 페이로드(Ieee1609Dot2Data, WSA 등)가 저장된 버퍼 포인터를 전달하며 이는 WSM body 에 수납된다.
 *                      NULL 을 전달할 경우, WSM body 가 없는 WSM MPDU 가 생성된다.
 * @param payload_size  payload 버퍼에 담긴 실제 페이로드의 길이를 전달한다.
 *                      페이로드가 없는 경우 0을 전달한다.
 *                      이 값은 kWsmBodyMaxSize(2297) 또는 (WSMP 헤더가 최소길이가 될 경우)
 *                      kWsmBodySafeMaxSize(2284)을 초과할 수 없다. (WSMP 헤더가 최대길이가 될 경우)
 *                      되도록 kWsmBodySafeMaxSize 기준을 사용하라.
 * @param outbuf        생성된 MPDU(MAC CRC 필드 제외)가 저장될 버퍼 포인터를 전달한다.
 *                      NULL 은 전달할 수 없다.
 * @param outbuf_size   outbuf 버퍼의 크기를 전달한다.
 *                      이 값은 payload_size 인자보다 최소
 *                      32바이트(kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize) 또는
 *                      46바이트(kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMaxSize) 이상 커야 한다.
 *                      되도록 kMpduMaxSize(2334) 또는 46바이트 기준값을 사용하라.
 * @return              성공시 생성된 MPDU의 길이, 실패시 음수(-Dot3ResultCode)
 *
 * 본 API 호출 시, 페이로드를 WSM body 에 수납하거나 수납하지 않은 WSM MPDU 가 반환된다.
 * payload 인자가 NULL 이거나 payload_size 인자가 0 이면, WSM body 없는 WSM MPDU 가 생성된다.
 */
int Dot3_ConstructWsmMpdu(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);

/**
 * @brief 수신된 WSM(WAVE Short Message) MPDU(MAC Protocol Data Unit)에 대한 파싱을 요청한다.
 * @param mpdu              WSM MPDU(MAC CRC 필드 포함)가 저장된 버퍼 포인터를 전달한다.
 *                          NULL 은 사용할 수 없다.
 * @param mpdu_size         mpdu 버퍼에 담긴 실제 MPDU 의 길이 (MAC CRC 필드 불포함)
 * @param outbuf            파싱된 페이로드(=WSM body)가 저장될 버퍼 포인터를 전달한다.
 *                          파싱된 페이로드가 저장되어 반환된다.
 *                          NULL 은 사용할 수 없다.
 * @param outbuf_size       outbuf 버퍼의 크기를 전달한다.
 *                          이 값은 되도록 mpdu_size 인자값 - (kQoSMacHdrSize+kLLCHdrSize+kWsmpHdrMinSize) 보다 크게 한다.
 * @param params            WSM 수신파라미터정보 구조체의 포인터를 전달한다
 *                          WSM 관련 수신파라미터정보가 업데이트되어 반환된다.
 *                          NULL 은 사용할 수 없다.
 * @param wsr_registered    WSM 의 PSID 가 WSR 테이블에 등록되어 있는지 여부를 저장할 변수 포인터를 전달한다.
 *                          해당 PSID 가 WSR 테이블에 등록되어 있는 경우 true, 등록되어 있지 않을 경우 false 가 저장되어 반환된다.
 *                          NULL 은 사용할 수 없다.
 * @return                  성공시 outbuf 에 저장된 페이로드의 길이, 실패시 음수(-Dot3ResultCode)
 *
 * 본 API 호출 시, 파싱된 페이로드(=WSM body)와 수신파라미터정보가 반환된다.
 * 호출자는 outbuf 에 반환되는 페이로드를 상위계층으로 전달하여 처리할 수 있다(예: 1609.2, SAE J2735 등)
 * 호출자는 반환된 정보 중 wsr_registered 값을 통해 해당 WSM 이 WSR 에 등록되어 있는지 여부를 확인할 수 있다.
 */
int Dot3_ParseWsmMpdu(
  const uint8_t *const mpdu,
  const Dot3PduSize mpdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params,
  bool *const wsr_registered);

/**
 * @brief WSR(WAVE Service Request = 수신하고자 하는 WSM의 PSID)를 등록한다.
 * @param psid 관심 있는 PSID
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 *
 * 등록 요청된 PSID는 dot3 라이브러리 내부에서 관리되는 WSR 테이블에 저장된다.
 */
int Dot3_AddWsr(const Dot3Psid psid);

/**
 * @brief WSR(WAVE Service Request = 수신하고자 하는 WSM의 PSID)를 삭제한다.
 * @param psid 더이상 관심이 없는 PSID
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 *
 * 삭제 요청된 PSID는 dot3 라이브러리 내부에서 관리되는 WSR 테이블에서 삭제된다.
 */
int Dot3_DeleteWsr(const Dot3Psid psid);

/**
 * @brief 모든 WSR(WAVE Service Request = 수신하고자 하는 WSM의 PSID)들을 삭제한다.
 * @return 성공시 0, 실패시 음수(-Dot3ResultCode)
 *
 * dot3 라이브러리 내부에서 관리되는 WSR 테이블 내 모든 정보가 삭제된다.
 */
int Dot3_DeleteAllWsrs(void);

/**
 * @brief 등록된 WSR의 개수를 반환한다.
 * @return 성공시 등록된 WSR의 개수(0 이상), 실패시 음수(-Dot3ResultCode)
 */
int Dot3_GetWsrNum(void);

/**
 * @brief 등록되어 있는 모든 WSR들을 반환한다.
 * @param wsrs WSR들이 반환된다.
 * @return 성공시 반환된 WSR의 개수(0 이상), 실패시 음수(-Dot3ResultCode)
 */
int Dot3_GetAllWsrs(struct Dot3Wsr wsrs[]);

/**
 * PSR(Provider Service Request)을 등록한다.
 * 등록 요청된 PSR은 dot3 라이브러리 내부에서 관리되는 PSR 테이블에 저장된다.
 *
 * @param psr   등록할 PSR 정보
 * @return      성공시 현재 등록되어 있는 PSR의 개수, 실패시 음수(-Dot3ResultCode)
 */
int Dot3_AddPsr(const struct Dot3Psr *const psr);

/**
 * 특정 PSR(Provider Service Request)을 삭제한다.
 * 삭제 요청된 PSR은 dot3 라이브러리 내부에서 관리되는 PSR 테이블에서 삭제된다.
 *
 * @param psid  삭제하고자 하는 PSR의 PSID
 * @return      성공시 현재 등록되어 있는 PSR의 개수, 실패시 음수(-Dot3ResultCode)
 */
int Dot3_DeletePsr(const Dot3Psid psid);

/**
 * 특정 PSR(Provider Service Request)의 내용을 변경한다.
 * dot3 라이브러리 내부에서 관리되는 PSR 정보의 내용이 변경된다.
 *
 * @param psid  변경하고자 하는 PSR의 PSID
 * @param psc   변경된 PSC
 * @return      성공시 현재 등록되어 있는 PSR의 개수, 실패시 음수(-Dot3ResultCode)
 */
int Dot3_ChangePsr(const Dot3Psid psid, const char *psc);

/**
 * 모든 PSR(Provider Service Request)들을 삭제한다.
 * dot3 라이브러리 내부에서 관리되는 PSR 테이블 내 모든 PSR 정보가 삭제된다.
 */
void Dot3_DeleteAllPsrs(void);

/**
 * 특정 PSID를 갖는 PSR 정보를 확인한다.
 * dot3 라이브러리 내부에서 관리되는 PSR 테이블 내 해당 PSR 정보가 반환된다.
 *
 * @param psid  확인하고자 하는 PSR의 PSID
 * @param psr   PSR 정보가 반환될 변수의 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
int Dot3_GetPsrWithPsid(const Dot3Psid psid, struct Dot3Psr *const psr);

/**
 * 등록되어 있는 PSR의 개수를 확인한다.
 * dot3 라이브러리 내부에서 관리되는 PSR 테이블에 등록되어 있는 PSR의 개수가 반환된다.
 *
 * @return 등록되어 있는 PSR의 개수(0 이상)
 */
int Dot3_GetPsrNum(void);

/**
 * 등록되어 있는 모든 PSR들의 정보를 확인한다.
 * dot3 라이브러리 내부에서 관리되는 PSR 테이블에 등록되어 있는 모든 PSR의 정보가 반환된다.
 *
 * @param psrs_array        PSR 정보들이 반환될 배열의 포인터
 * @param psrs_array_size   psrs_array 의 크기
 * @return                  성공시 반환된 PSR의 개수(0 이상), 실패시 음수(-Dot3ResultCode)
 */
int Dot3_GetAllPsrs(struct Dot3Psr *psrs_array, const Dot3PsrNum psrs_array_size);


/**
 * WSA 생성을 요청한다.
 *
 * @param params        WSA 생성을 위해 전달되는 정보
 * @param outbuf        생성된 WSA가 저장될 버퍼의 포인터
 * @param outbuf_size   outbuf 버퍼의 크기
 * @return              성공 시 WSA의 길이(0 이상), 실패시 음수(-Dot3ResultCode)
 */
int Dot3_ConstructWsa(
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size);

/**
 * WSA 파싱을 요청한다.
 *
 * @param encoded_wsa           파싱할 WSA (UPER 인코딩 된 상태)
 * @param encoded_wsa_size      inbuf 버퍼의 길이
 * @param params                파싱된 정보가 저장될 정보구조체의 포인터
 * @return                      성공 시 0, 실패시 음수(-Dot3ResultCode)
 */
int Dot3_ParseWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params);


#ifdef __cplusplus
}
#endif

#endif //V2X_LIBDOT3_DOT3_API_H
