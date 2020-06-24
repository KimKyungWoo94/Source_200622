/**
 * @file api-test-Dot3_Psr.cc
 * @date 2019-08-17
 * @author gyun
 * @brief PSR 관련 모든 Open API에 대한 단위테스트
 *
 * 본 파일은 모든 PSR 관련 Open API (Dot3_***Psr())에 대한 단위테스트를 수행한다.
 * 파라미터 유효성, 크기, 결과값 등에 대해 테스트한다.
 */


#include "gtest/gtest.h"

#include "dot3/dot3.h"


/*
 * 샘플데이터 생성을 위한 전역변수
 */
static uint8_t g_test_ipv6_address[16] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static uint8_t g_test_mac_address[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };


/*
 * Test case
 *  * Dot3_AddPsr() 동작 유효성
 *  * Dot3_AddPsr() 파라미터 유효성
 *  * Dot3_DeletePsr() 동작 및 파라미터 유효성
 *  * Dot3_ChangePsr() 동작 및 파라미터 유효성
 *  * Dot3_DeleteAllPsrs() 동작 및 파라미터 유효성
 *  * Dot3_GetPsrWithPsid() 동작 및 파라미터 유효성
 *  * Dot3_GetPsrNum() 동작 및 파라미터 유효성
 *  * Dot3_GetAllPsrs() 동작 및 파라미터 유효성
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
 * 1) Dot3_AddPsr() 동작 유효성
 */
TEST(Dot3_Psr, Dot3_AddPsr)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psr;
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 정상 동작 확인 - 등록 후 개수, 등록정보의 정확성을 확인한다.
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 모든 PSR 정보의 정확성 확인
    memset(psrs2, 0, sizeof(psrs2));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 테이블 오버플로우
   */
  memcpy(&psr, &psrs[0], sizeof(psr));
  psr_num = Dot3_AddPsr(&psr);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_PsrTableFull);

  /*
   * 중복된 PSID 등록
   */
  {
    Dot3_DeleteAllPsrs();

    // 등록
    memcpy(&psr, &psrs[0], sizeof(psr));
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, 1);

    // 중복 등록
    memcpy(&psr, &psrs[0], sizeof(psr));
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_SamePsidPsr);
  }
}


/*
 * * Dot3_AddPsr() 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_AddPsr_parameters)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs1[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psrs3[kDot3PsrNum_MaxNum], psr;
  memset(psrs1, 0, sizeof(psrs1));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs1, kDot3PsrNum_MaxNum);

  /*
   * null 파라미터
   */
  psr_num = Dot3_AddPsr(NULL);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_NullParameters);

  /*
   * wsa_id
   */
  {
    Dot3_DeleteAllPsrs();

    // 정상범위 등록
    int cnt = 0;
    for (int i = 0; i <= kDot3WsaMaxId; i++) {
      memcpy(&psrs2[i], &psrs1[i], sizeof(psr));
      psrs2[i].wsa_id = i;
      psr_num = Dot3_AddPsr(&psrs2[i]);
      EXPECT_EQ(psr_num, i + 1);
      cnt++;
    }
    // 등록정보 확인
    memset(psrs3, 0, sizeof(psrs3));
    psr_num = Dot3_GetAllPsrs(psrs3, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, cnt);
    EXPECT_TRUE(!memcmp(psrs3, psrs2, sizeof(struct Dot3Psr) * cnt));

    // 비정상범위 등록
    psrs2[0].wsa_id = kDot3WsaMaxId + 1;
    psr_num = Dot3_AddPsr(&psrs2[0]);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidWsaIdValue);
  }

  /*
   * 유효하지 않은 PSID
   */
  memcpy(&psr, &psrs1[0], sizeof(psr));
  psr.psid = kDot3Psid_Max + 1;
  psr_num = Dot3_AddPsr(&psr);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidPsidValue);

  /*
   * PSC 길이
   */
  {
    Dot3_DeleteAllPsrs();

    // 정상범위 등록
    int cnt = 0;
    for (int i = 0; i <= kDot3PscLen_Max; i++) {
      memcpy(&psrs2[i], &psrs1[i], sizeof(psr));
      psrs2[i].psc.len = i;
      psr_num = Dot3_AddPsr(&psrs2[i]);
      EXPECT_EQ(psr_num, i + 1);
      cnt++;
    }
    // 등록정보 확인
    memset(psrs3, 0, sizeof(psrs3));
    psr_num = Dot3_GetAllPsrs(psrs3, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, cnt);
    EXPECT_TRUE(!memcmp(psrs3, psrs2, sizeof(struct Dot3Psr) * cnt));

    // 비정상범위 등록
    memcpy(&psr, &psrs1[0], sizeof(psr));
    psr.psc.len = kDot3PscLen_Max + 1;
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_TooLongPsc);
  }

  /*
   * 채널 번호
   */
  {
    Dot3_DeleteAllPsrs();

    // 정상범위 등록
    int cnt = 0;
    for (int i = kDot3Channel_KoreaV2XMin; i <= kDot3Channel_KoreaV2XMax; i++) {
      memcpy(&psrs2[cnt], &psrs1[cnt], sizeof(psr));
      psrs2[cnt].service_chan_num = kDot3Channel_KoreaV2XMin;
      psr_num = Dot3_AddPsr(&psrs2[cnt]);
      EXPECT_EQ(psr_num, cnt + 1);
      cnt++;
    }
    // 등록정보 확인
    memset(psrs3, 0, sizeof(psrs3));
    psr_num = Dot3_GetAllPsrs(psrs3, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, cnt);
    EXPECT_TRUE(!memcmp(psrs3, psrs2, sizeof(struct Dot3Psr) * cnt));

    Dot3_DeleteAllPsrs();

    // 비정상범위 등록
    memcpy(&psr, &psrs1[0], sizeof(psr));
    psr.service_chan_num = kDot3Channel_KoreaV2XMin - 1;
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_NoRelatedChannelInfo);
    memcpy(&psr, &psrs1[0], sizeof(psr));
    psr.service_chan_num = kDot3Channel_KoreaV2XMax + 1;
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_NoRelatedChannelInfo);
  }

  /*
   * Channel access
   */
  {
    Dot3_DeleteAllPsrs();

    //정상범위 등록
    int cnt = 0;
    for (int i = kDot3ProviderChannelAccess_Min; i <= kDot3ProviderChannelAccess_Max; i++) {
      memcpy(&psrs2[i], &psrs1[i], sizeof(psr));
      psrs2[i].chan_access = i;
      psr_num = Dot3_AddPsr(&psrs2[i]);
      EXPECT_EQ(psr_num, i + 1);
      cnt++;
    }
    // 등록정보 확인
    memset(psrs3, 0, sizeof(psrs3));
    psr_num = Dot3_GetAllPsrs(psrs3, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, cnt);
    EXPECT_TRUE(!memcmp(psrs3, psrs2, sizeof(struct Dot3Psr) * cnt));

    Dot3_DeleteAllPsrs();

    // 비정상범위 등록
    memcpy(&psr, &psrs1[0], sizeof(psr));
    psr.chan_access = kDot3ProviderChannelAccess_Max + 1;
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidChannelAccess);
  }

  /*
   * WSA count threshold
   */
  {
    Dot3_DeleteAllPsrs();

    // 정상범위 등록 (WSA count threshold 값의 범위가 kDot3PsrNum_MaxNum 보다 크므로, 범위 내 모든 값이 아닌, 3씩 증가하는 값을 사용.
    int cnt = 0;
    for (int i = kDot3WsaCountThresholdInterval_Min; i <= kDot3WsaCountThresholdInterval_Max; i += 3) {
      memcpy(&psrs2[cnt], &psrs1[cnt], sizeof(psr));
      psrs2[cnt].wsa_cnt_threshold_interval = i;
      psr_num = Dot3_AddPsr(&psrs2[cnt]);
      EXPECT_EQ(psr_num, cnt + 1);
      cnt++;
    }
    // 등록정보 확인
    memset(psrs3, 0, sizeof(psrs3));
    psr_num = Dot3_GetAllPsrs(psrs3, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, cnt);
    EXPECT_TRUE(!memcmp(psrs3, psrs2, sizeof(struct Dot3Psr) * cnt));

    Dot3_DeleteAllPsrs();

    // 비정상범위 등록
    memcpy(&psr, &psrs1[0], sizeof(psr));
    psr.wsa_cnt_threshold_interval = kDot3WsaCountThresholdInterval_Min - 1;
    psr_num = Dot3_AddPsr(&psr);
    EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidWsaCountThresholdIntervalValue);
  }
}


/*
 * * Dot3_DeletePsr() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_DeletePsr)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 초기 상태 셋업 - PSR 최대개수만큼 등록한다.
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 모든 PSR 정보 정확성 확인
    memset(psrs2, 0, sizeof(psrs2));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 등록되지 않은 PSID에 대한 삭제 시도
   */
  psr_num = Dot3_DeletePsr(kDot3PsrNum_MaxNum);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_NoSuchPsr);

  /*
   * 유효하지 않은 PSID에 대한 삭제 시도
   */
  psr_num = Dot3_DeletePsr(kDot3Psid_Max + 1);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidPsidValue);

  /*
   * 정상 삭제 시도
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 삭제
    psr_num = Dot3_DeletePsr(i);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1 - i));
    // 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1 - i));
    // 남아 있는 모든 PSR 정보 정확성 확인
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1 - i));
    EXPECT_TRUE(!memcmp(psrs + 1 + i, psrs2, sizeof(struct Dot3Psr) * psr_num));
  }

  /*
   * 비어 있는 테이블에 삭제 시도
   */
  psr_num = Dot3_GetPsrNum();
  EXPECT_EQ(psr_num, 0);
  psr_num = Dot3_DeletePsr(0);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_NoSuchPsr);
}


/*
 * * Dot3_ChangePsr() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_ChangePsr)
{
  // TODO:: 구현 후 추가
}


/*
 * * Dot3_DeleteAllPsrs() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_DeleteAllPsrs)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 초기 상태 셋업 - PSR 최대개수만큼 등록한다.
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 모든 PSR 정보 정확성 확인
    memset(psrs2, 0, sizeof(psrs2));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 삭제 후 개수 확인
   */
  Dot3_DeleteAllPsrs();
  psr_num = Dot3_GetPsrNum();
  EXPECT_EQ(psr_num, 0);
}


/*
 * * Dot3_GetPsrWithPsid() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_GetPsrWithPsid)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum], psr;
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 초기 상태 셋업 - PSR 최대개수만큼 등록한다.
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 모든 PSR 정보 정확성 확인
    memset(psrs2, 0, sizeof(psrs2));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 등록되지 않은 PSID에 대한 확인 시도
   */
  psr_num = Dot3_GetPsrWithPsid(kDot3PsrNum_MaxNum, &psr);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_NoSuchPsr);

  /*
   * 유효하지 않은 PSID에 대한 삭제 시도
   */
  psr_num = Dot3_GetPsrWithPsid(kDot3Psid_Max + 1, &psr);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_InvalidPsidValue);

  /*
   * 널 파라미터
   */
  psr_num = Dot3_GetPsrWithPsid(0, NULL);
  EXPECT_EQ(psr_num, -kDot3Result_Fail_NullParameters);

  /*
   * 확인 정보 정상 여부 체크
   */
  int ret;
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    memset(&psr, 0, sizeof(psr));
    ret = Dot3_GetPsrWithPsid(i, &psr);
    EXPECT_EQ(ret, kDot3Result_Success);
    EXPECT_TRUE(!memcmp(&psr, &psrs[i], sizeof(psr)));
  }
}


/*
 * * Dot3_GetPsrNum() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_GetPsrNum)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum];
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 한 개도 없을 때 확인
   */
  psr_num = Dot3_GetPsrNum();
  EXPECT_EQ(psr_num, 0);

  /*
   * 하나씩 등록해 가면서 개수 확인
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, i + 1);
  }

  /*
   * 전체 개수 확인
   */
  psr_num = Dot3_GetPsrNum();
  EXPECT_EQ(psr_num, kDot3PsrNum_MaxNum);

  /*
   * 하나씩 삭제해 가면서 개수 확인
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 삭제
    psr_num = Dot3_DeletePsr(i);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);
    // 개수 확인
    psr_num = Dot3_GetPsrNum();
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);
  }

  /*
   * 다 삭제되었을 때 확인
   */
  psr_num = Dot3_GetPsrNum();
  EXPECT_EQ(psr_num, 0);
}


/*
 * * Dot3_GetAllPsrs() 동작 및 파라미터 유효성
 */
TEST(Dot3_Psr, Dot3_GetAllPsrs)
{
  Dot3_Init(0);  // 테스트 실패 원인 확인 시에는 6 으로 변경

  struct Dot3Psr psrs[kDot3PsrNum_MaxNum], psrs2[kDot3PsrNum_MaxNum];
  memset(psrs, 0, sizeof(psrs));
  int psr_num;

  /*
   * PSR 샘플 데이터 준비
   */
  PrepareSamplePsrs(psrs, kDot3PsrNum_MaxNum);

  /*
   * 한 개도 없을 때 확인
   */
  memset(psrs2, 0, sizeof(psrs));
  psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(psr_num, 0);

  /*
   * 하나씩 등록해 가면서 정보 확인
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 등록
    psr_num = Dot3_AddPsr(&psrs[i]);
    EXPECT_EQ(psr_num, i + 1);
    // 등록된 모든 정보 확인
    memset(psrs2, 0, sizeof(psrs));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, i + 1);
    EXPECT_TRUE(!memcmp(psrs, psrs2, sizeof(struct Dot3Psr) * i));
  }

  /*
   * 하나씩 삭제해 가면서 정보 확인
   */
  for (int i = 0; i < kDot3PsrNum_MaxNum; i++) {
    // 삭제
    psr_num = Dot3_DeletePsr(i);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);
    // 등록된 모든 정보 확인
    memset(psrs2, 0, sizeof(psrs));
    psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
    EXPECT_EQ(psr_num, (kDot3PsrNum_MaxNum - 1) - i);
    EXPECT_TRUE(!memcmp(psrs + 1 + i, psrs2, psr_num));
  }

  /*
   * 다 삭제되었을 때 확인
   */
  memset(psrs2, 0, sizeof(psrs));
  psr_num = Dot3_GetAllPsrs(psrs2, kDot3PsrNum_MaxNum);
  EXPECT_EQ(psr_num, 0);
}
