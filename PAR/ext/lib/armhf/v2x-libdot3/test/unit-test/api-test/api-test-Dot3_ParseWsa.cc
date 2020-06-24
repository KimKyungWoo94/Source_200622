/**
 * @file api-test-Dot3_ParseWsa.cc
 * @date 2019-08-19
 * @author gyun
 * @brief Dot3_ParseWsa() Open API에 대한 단위테스트
 *
 * 본 파일은 Dot3_ParseWsa() Open API에 대한 단위테스트를 수행한다.
 * 파라미터 유효성, 크기, 결과값 등에 대해 테스트한다.
 * WSA 데이터 자체의 유효성은 내부함수 단위테스트에서 수행하므로, 여기서는 수행하지 않는다.
 */

#include <stdio.h>

#include "gtest/gtest.h"

#include "dot3/dot3.h"


/*
 * Test case
 *
 * - 널 파라미터에 따른 동작을 테스트한다.
 */


/*
 * - null 파라미터에 따른 동작을 테스트한다.
 *  - 유효한 값에 대한 정상 동작은 내부함수 단위테스트에서 수행된다.
 */
TEST(Dot3_ParseWsa, null)
{
  Dot3_Init(0);

  int ret;
  struct Dot3ParseWsaParams params;
  uint8_t encoded_wsa[] = {0x30, 0x33};

  /*
   * 정상 동작 확인
   */
  memset(&params, 0, sizeof(params));
  ret = Dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, kDot3Result_Success);

  /*
   * encoded_wsa = NULL
   */
  memset(&params, 0, sizeof(params));
  ret = Dot3_ParseWsa(NULL, sizeof(encoded_wsa), &params);
  EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);

  /*
   * encoded_wsa_size = 0
   */
  memset(&params, 0, sizeof(params));
  ret = Dot3_ParseWsa(encoded_wsa, 0, &params);
  EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);

  /*
   * params = NULL
   */
  memset(&params, 0, sizeof(params));
  ret = Dot3_ParseWsa(encoded_wsa, sizeof(encoded_wsa), NULL);
  EXPECT_EQ(ret, -kDot3Result_Fail_NullParameters);
}
