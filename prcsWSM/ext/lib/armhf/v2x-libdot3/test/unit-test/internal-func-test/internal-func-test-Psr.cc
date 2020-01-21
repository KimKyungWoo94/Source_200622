
/**
 * @file internal-func-test-Psr.cc
 * @date 2019-08-16
 * @author gyun
 * @brief PSR 관련함수들에 대한 단위테스트
 *
 * 본 파일은 dot3 라이브러리 내부 함수 중, PSR 관련 함수들에 대한 단위 테스트를 수행하는 테스트코드를 포함한다.
 * 파라미터 유효성에 대한 단위테스트는 수행하지 않는다. (이는 해당 함수를 호출하는 API 함수의 단위테스트에서 수행된다)
 *
 * 시험데이터 및 기대값은 https://asn1.io/asn1playground/ 에서 획득하였다.
 */

#include <stdio.h>
#include <string.h>

#include "gtest/gtest.h"

#include "dot3/dot3.h"
#include "dot3-internal.h"
#include "dot3-mib.h"


static uint8_t g_test_ipv6_address[16] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static uint8_t g_test_mac_address[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
static struct Dot3ProviderInfo *g_pinfo = &(g_dot3_mib.provider_info);

/*
 * Test case
 *  1) PSR을 순차적으로 최대치까지 등록해 가면서, 저장정보를 확인한다.
 *  2) PSR을 최대개수만큼 등록된 상태에서 하나씩 순차적으로 삭제해 가면서, 저장정보를 확인한다.
 *  3) PSR 개수별로 한번에 삭제 후 저장정보를 확인한다.
 */


/*
 * Sample PSR 데이터를 생성한다.
 */
static void PrepareSamplePsrs(struct Dot3Psr *const psrs, const uint8_t psr_num)
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
    psr->wsa_id = i % (kDot3WsaMaxId + 1);
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
    psr->wsa_id = i % (kDot3WsaMaxId + 1);
    psr->psid = i;
    psr->service_chan_num = kDot3Channel_KoreaV2XMin;
    psr->chan_access = kDot3ProviderChannelAccess_Any;
    psr->ip_service = false;
  }
}


/*
 * 1) PSR을 순차적으로 최대치까지 등록해 가면서, 저장정보를 확인한다.
 *  (1) dot3_AddPsr() 함수를 이용하여 128개까지 등록한다.
 *    - 1개씩 추가 등록할 때마다 다음을 수행하여 결과를 확인한다.
 *      - PSR의 개수를 확인한다 - dot3_GetPsrNum()
 *      - 모든 PSR의 정보를 확인한다 - dot3_GetAllPsrs()
 *  (2) (1)번 상태에서 dot3_AddPsr() 함수 호출 시 실패하는 것을 확인한다 (최대개수 초과)
 *  (3) 각 PSR의 정보를 확인한다 - dot3_GetPsrWithPsid()
 */
TEST(dot3_PSR, ADD_PSR)
{
  Dot3_Init(kDot3LogLevel_none);

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psr;
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 등록 동작 확인
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {

    // 등록
    psr_num = dot3_AddPsr(g_pinfo, &psrs[i]);
    EXPECT_EQ(psr_num, i + 1);

    // 개수 확인
    psr_num = dot3_GetPsrNum(g_pinfo);
    EXPECT_EQ(psr_num, i + 1);

    // 등록된 모든 PSR 정보 확인
    psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 최대 개수 초과
   */
  memcpy(&psr, &psrs[0], sizeof(struct Dot3Psr));
  psr_num = dot3_AddPsr(g_pinfo, &psr);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_PsrTableFull);

  /*
   * PSID 별 PSR 정보 확인
   */
  int ret;
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {

    // 저장된 PSID 에 대해 확인 요청
    memset(&psr, 0, sizeof(psr));
    ret = dot3_GetPsrWithPsid(g_pinfo, i, &psr);
    EXPECT_EQ(ret, kDot3Result_Success);
    EXPECT_TRUE(!memcmp(&psr, &psrs[i], sizeof(psr)));

    // 저장되어 있지 않은 PSID 에 대해 확인 요청
    memset(&psr, 0, sizeof(psr));
    ret = dot3_GetPsrWithPsid(g_pinfo, i + kDot3PsrNum_MaxNum, &psr);
    EXPECT_EQ(ret, -kDot3Result_Fail_NoSuchPsr);
  }

  /*
   * 중복된 PSID 등록
   */
  {
    dot3_DeleteAllPsrs(g_pinfo);

    // 등록
    memcpy(&psr, &psrs[0], sizeof(struct Dot3Psr));
    psr_num = dot3_AddPsr(g_pinfo, &psr);
    EXPECT_EQ(psr_num, 1);

    // 중복 등록
    memcpy(&psr, &psrs[0], sizeof(struct Dot3Psr));
    psr_num = dot3_AddPsr(g_pinfo, &psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_SamePsidPsr);
  }


}


/*
 * 2) PSR을 최대개수만큼 등록된 상태에서 하나씩 순차적으로 삭제해 가면서, 저장정보를 확인한다.
 *  (1) 최대 등록 상태에서 dot3_DeletePsr() 함수를 이용하여 0개가 될때까지 1개씩 삭제한다.
 *    - 1개씩 삭제할 때마다 다음을 수행하여 결과를 확인한다.
 *      - PSR의 개수를 확인한다 - dot3_GetPsrNum()
 *      - 모든 PSR의 정보를 확인한다 - dot3_GetAllPsrs()
 *      - 각 PSR의 정보를 확인한다 - dot3_GetPsrWithPsid()
 */
TEST(dot3_PSR, DELETE_PSR)
{
  Dot3_Init(kDot3LogLevel_none);

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psr;
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 최대치 등록
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    psr_num = dot3_AddPsr(g_pinfo, &psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
  }

  /*
   * 등록 상태 정상 여부 체크 - 개수, 내용
   */
  psr_num = dot3_GetPsrNum(g_pinfo);
  EXPECT_EQ(psr_num, kDot3PsrNum_MaxNum);
  psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(psr_num, kDot3PsrNum_MaxNum);
  EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * kDot3PsrNum_MaxNum));

  /*
   * 앞에서부터 하나씩 삭제하면서 저장정보를 확인한다.
   */
  int ret;
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++)
  {
    // 삭제
    psr_num = dot3_DeletePsr(g_pinfo, i);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);

    // 개수 확인
    psr_num = dot3_GetPsrNum(g_pinfo);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);

    // 등록된 모든 PSR 정보 확인
    psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);
    EXPECT_TRUE(!memcmp(psrs + 1 + i, psrs2, sizeof(struct Dot3Psr) * psr_num));

    // 각 PSID 별 PSR 정보 확인
    for (int j = 0; j < kDot3PsrNum_MaxNum; j++) {
      ret =  dot3_GetPsrWithPsid(g_pinfo, j, &psr);
      // 삭제된 PSR
      if (j <= i) {
        EXPECT_EQ(ret, -kDot3Result_Fail_NoSuchPsr);
      }
      // 남아있는 PSR
      else {
        EXPECT_EQ(ret, kDot3Result_Success);
        EXPECT_TRUE(!memcmp(&psr, &psrs[j], sizeof(psr)));
      }
    }
  }


  /*
   * 최대치 등록
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    psr_num = dot3_AddPsr(g_pinfo, &psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
  }

  /*
   * 등록 상태 정상 여부 체크 - 개수, 내용
   */
  psr_num = dot3_GetPsrNum(g_pinfo);
  EXPECT_EQ(psr_num, kDot3PsrNum_MaxNum);
  psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(psr_num, kDot3PsrNum_MaxNum);
  EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * kDot3PsrNum_MaxNum));

  /*
   * 뒤에서부터 하나씩 삭제하면서 저장정보를 확인한다.
   */
  for (int i = (kDot3PsrNum_MaxNum - 1); i >= 0; i--)
  {
    // 삭제
    psr_num = dot3_DeletePsr(g_pinfo, i);
    EXPECT_EQ(psr_num, i);

    // 개수 확인
    psr_num = dot3_GetPsrNum(g_pinfo);
    EXPECT_EQ(psr_num, i);

    // 등록된 모든 PSR 정보 확인
    psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * psr_num));

    // 각 PSID 별 PSR 정보 확인
    for (int j = 0; j < kDot3PsrNum_MaxNum; j++) {
      ret =  dot3_GetPsrWithPsid(g_pinfo, j, &psr);
      // 삭제된 PSR
      if (j >= i) {
        EXPECT_EQ(ret, -kDot3Result_Fail_NoSuchPsr);
      }
        // 남아있는 PSR
      else {
        EXPECT_EQ(ret, kDot3Result_Success);
        EXPECT_TRUE(!memcmp(&psr, &psrs[j], sizeof(psr)));
      }
    }
  }
}

/*
 * 3) PSR 개수별로 한번에 삭제 후 저장정보를 확인한다.
 *
 */
TEST(dot3_PSR, DELETE_ALL_PSR)
{
  Dot3_Init(kDot3LogLevel_none);

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psr;
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  int ret;
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {

    /*
     * 준비 단계 - 등록, 개수확인, 정보확인
     */
    // i 개만큼 등록 - 준비
    for (int j = 0; j <= i; j ++) {
      psr_num = dot3_AddPsr(g_pinfo, &psrs[j]);
      EXPECT_EQ(psr_num, j + 1);
    }
    // 등록된 개수 및 데이터 확인 - 준비완료 체크
    psr_num = dot3_GetPsrNum(g_pinfo);
    EXPECT_EQ(psr_num, i + 1);
    psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
    // PSR 탐색 - 준비완료 체크
    for (int j = 0; j < kDot3PsrNum_MaxNum; j++) {
      memset(&psr, 0, sizeof(psr));
      ret = dot3_GetPsrWithPsid(g_pinfo, j, &psr);
      if (j <= i) {
        EXPECT_EQ(ret, kDot3Result_Success);
        EXPECT_TRUE(!memcmp(&psr, &psrs[j], sizeof(psr)));
      } else {
        EXPECT_EQ(ret, -kDot3Result_Fail_NoSuchPsr);
      }
    }

    /*
     * 테스트 단계 - 모두 삭제 후 정보 확인
     *  - 개수가 0이어야 함, PSID 별 PSR 을 찾을 수 없어야 함.
     */
    dot3_DeleteAllPsrs(g_pinfo);
    psr_num = dot3_GetPsrNum(g_pinfo);
    EXPECT_EQ(psr_num, 0);
    psr_num = dot3_GetAllPsrs(g_pinfo, psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, 0);
    for (int j = 0; j < kDot3PsrNum_MaxNum; j++) {
      ret = dot3_GetPsrWithPsid(g_pinfo, j, &psr);
      EXPECT_EQ(ret, -kDot3Result_Fail_NoSuchPsr);
    }
  }
}

