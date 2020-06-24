/**
 * @file dot3-wsa.c
 * @date 2019-08-17
 * @author gyun
 * @brief WSA 관련 함수들을 구현한 파일
 */

#include "dot3-internal.h"
#if defined(OBJASN1C_)
  #include "dot3-objasn1c.h"
#elif defined(FFASN1C_)
  #include "dot3-ffasn1c.h"
#endif


/**
 * @copydoc Dot3_ConstructWsa
 */
int INTERNAL dot3_ConstructWsa(
  struct Dot3ProviderInfo *const pinfo,
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
#if defined(OBJASN1C_)
  return dot3_ObjAsn1c_EncodeWsa(pinfo, params, outbuf, outbuf_size);
  #error "WSA encoding function using ObjAsn1c is not implemented yet"
#elif defined(FFASN1C_)
  return dot3_FFAsn1c_EncodeWsa(pinfo, params, outbuf, outbuf_size);
#else
  #error "3rd party asn.1 library is not defined"
#endif
}

/**
 * @copydoc Dot3_ParseWsa
 */
int INTERNAL dot3_ParseWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params)
{
#if defined(OBJASN1C_)
  return dot3_ObjAsn1c_DecodeWsa(inbuf, inbuf_size, params);
  #error "WSA decoding function using ObjAsn1c is not implemented yet"
#elif defined(FFASN1C_)
  return dot3_FFAsn1c_DecodeWsa(encoded_wsa, encoded_wsa_size, params);
#else
  #error "3rd party asn.1 library is not defined"
#endif
}
