//
// Created by gyun on 2019-06-27.
//

#include "dot3-internal.h"
#if defined(OBJASN1C_)
  #include "dot3-objasn1c.h"
#elif defined(FFASN1C_)
  #include "dot3-ffasn1c.h"
#endif

/**
 * @brief 전달된 송신파라미터들과 페이로드를 이용하여 UPER 인코딩된 WSM을 생성 후 반환한다.
 * @param params        WSM 송신파라미터정보 구조체의 포인터를 전달한다(Null 불가).
 *                      WSM 헤더를 생성하는데 사용되는 정보.
 * @param payload       상위계층 페이로드가 저장된 버퍼 포인터를 전달한다 -> WSM body에 수납된다.
 *                      payload가 없는 경우, NULL을 전달할 수 있다.
 * @param payload_size  payload 버퍼에 담긴 실제 페이로드의 길이.
 *                      payload가 없는 경우 0을 전달한다.
 *                      (kMsduMaxSize - (kLLCHdrSize + kWsmpHdrMinSize)) 값을 초과할 수 없다.
 * @param outbuf        생성된 WSM이 저장될 버퍼 포인터를 전달한다(Null 불가).
 *                      UPER 인코딩된 WSM header + body가 저장된다.
 * @param outbuf_size   outbuf 버퍼의 크기를 전달한다.
 *                      payload_size 인자보다 최소 4~18바이트(=모든 옵션필드 비활성화/활성화시 WSM 헤더 길이) 이상 커야 한다.
 * @return              성공시 생성된 WSM의 길이, 실패시 음수(-Dot3ResultCode)
 *
 * UPER 인코딩된 WSM을 생성한다. 빌드 옵션(CMakeLists.txt 참조)에 따라 ASN.1 컴파일러/라이브러리가 다르게 적용된다.
 */
int dot3_ConstructWsm(
  struct Dot3WsmMpduTxParams *const params,
  const uint8_t *const payload,
  const Dot3PduSize payload_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
#if defined(OBJASN1C_)
  return dot3_ObjAsn1c_EncodeWsm(params, payload, payload_size, outbuf, outbuf_size);
#elif defined(FFASN1C_)
  return dot3_FFAsn1c_EncodeWsm(params, payload, payload_size, outbuf, outbuf_size);
#else
  #error "3rd party asn.1 library is not defined"
#endif
}

/**
 * @brief MSDU(=WSM) 를 UPER 디코딩한 후, WSM 헤더 정보 등을 수신파라미터정보에 저장하여 반환한다.
 * @param msdu          파싱할 MSDU(=WSM) 가 저장된 버퍼의 주소를 전달한다. NULL 은 사용할 수 없다.
 * @param msdu_size     파싱할 MSDU 의 크기
 * @param outbuf        파싱된 상위계층 페이로드(=WSM body) 가 저장될 버퍼의 주소를 전달한다. NULL 은 사용할 수 없다.
 * @param outbuf_size   outbuf 버퍼의 크기를 전달한다.
 * @param params        수신파라미터정보가 저장될 구조체의 주소를 전달한다. NULL 은 사용할 수 없다.
 * @return 성공시 상위계층 페이로드의 크기, 실패시 음수(-Dot3ResultCode)
 */
int dot3_ParseWsm(
  const uint8_t *const msdu,
  const Dot3PduSize msdu_size,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size,
  struct Dot3WsmMpduRxParams *const params)
{
#if defined(OBJASN1C_)
  //return dot3_ObjAsn1c_DecodeWsm(msdu, msdu_size, outbuf, outbuf_size, params);
#error "WSM decoding function using ObjAsn1c is not implemented yet"
#elif defined(FFASN1C_)
  return dot3_FFAsn1c_DecodeWsm(msdu, msdu_size, outbuf, outbuf_size, params);
#else
#error "3rd party asn.1 library is not defined"
#endif
}
