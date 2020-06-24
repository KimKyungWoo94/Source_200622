/**
 * @file dot3-api-psr.c
 * @date 2019-08-16
 * @author gyun
 * @brief PSR 관련 API들을 구현한 파일
 */

#include "dot3/dot3-types.h"

#include "dot3-internal.h"


/**
 * @copydoc Dot3_AddPsr
 */
int OPEN_API Dot3_AddPsr(const struct Dot3Psr *const psr)
{
  Log(kDot3LogLevel_config, "Adding PSR\n");

  /*
   * 파라미터 유효성 체크
   */
  if (!psr) {
    Err("Fail to add PSR - null parameter\n");
    return -kDot3Result_Fail_NullParameters;
  }
  if (false == dot3_IsValidWsaIdValue(psr->wsa_id)) {
    Err("Fail to add PSR - invalid wsa id %u\n", psr->wsa_id);
    return -kDot3Result_Fail_InvalidWsaIdValue;
  }
  if(false == dot3_IsValidPsidValue(psr->psid)) {
    Err("Fail to add PSR - invalid psid %u\n", psr->psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }
  if (psr->present.psc && (false == dot3_IsValidPscLen(psr->psc.len))) {
    Err("Fail to add PSR - too long psc %u\n", psr->psc.len);
    return -kDot3Result_Fail_TooLongPsc;
  }
  if (false == dot3_IsValidChannelNumberValue(psr->service_chan_num)) {
    Err("Fail to add PSR - invalid service channel %d\n", psr->service_chan_num);
    return -kDot3Result_Fail_InvalidChannelNumberValue;
  }
  if (false == dot3_IsValidProviderChannelAccess(psr->chan_access)) {
    Err("Fail to add PSR - invalid channel access %d\n", psr->service_chan_num);
    return -kDot3Result_Fail_InvalidChannelAccess;
  }
  if (psr->present.wsa_cnt_threshold_interval &&
    (false == dot3_IsValidWsaCountThresholdIntervalValue(psr->wsa_cnt_threshold_interval))) {
    Err("Fail to add PSR - invalid wsa_cnt_threshold_interval %u\n", psr->wsa_cnt_threshold_interval);
    return -kDot3Result_Fail_InvalidWsaCountThresholdIntervalValue;
  }

  /*
   * PSR 추가
   */
  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  pthread_mutex_lock(&(pinfo->mtx));
  int ret = dot3_AddPsr(pinfo, psr);
  pthread_mutex_unlock(&(pinfo->mtx));
  return ret;
}


/**
 * @copydoc Dot3_DeletePsr
 */
int OPEN_API Dot3_DeletePsr(const Dot3Psid psid)
{
  Log(kDot3LogLevel_config, "Deleting PSR\n");

  /*
   * 파라미터 유효성 체크
   */
  if(false == dot3_IsValidPsidValue(psid)) {
    Err("Fail to delete PSR - invalid psid %u\n", psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }

  /*
   * PSR 삭제
   */
  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  pthread_mutex_lock(&(pinfo->mtx));
  int ret = dot3_DeletePsr(pinfo, psid);
  pthread_mutex_unlock(&(pinfo->mtx));
  return ret;
}


/**
 * @copydoc Dot3_ChangePsr
 */
int OPEN_API Dot3_ChangePsr(const Dot3Psid psid, const char *psc)
{
  // TODO:: 구현 필요
  return -kDot3Result_Fail_NotSupportedYet;
}


/**
 * @copydoc Dot3_DeleteAllPsrs
 */
void OPEN_API Dot3_DeleteAllPsrs(void)
{
  Log(kDot3LogLevel_config, "Deleting all PSRs\n");
  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  pthread_mutex_lock(&(pinfo->mtx));
  dot3_DeleteAllPsrs(pinfo);
  pthread_mutex_unlock(&(pinfo->mtx));
}


/**
 * @copydoc Dot3_GetPsrForPsid
 */
int OPEN_API Dot3_GetPsrWithPsid(const Dot3Psid psid, struct Dot3Psr *const psr)
{
  Log(kDot3LogLevel_config, "Get PSR for psid %u\n", psid);

  /*
   * 파라미터 유효성 체크
   */
  if (!psr) {
    Err("Fail to get PSR - null parameters\n");
    return -kDot3Result_Fail_NullParameters;
  }
  if(false == dot3_IsValidPsidValue(psid)) {
    Err("Fail to get PSR - invalid psid %u\n", psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }

  /*
   * PSR 확인
   */
  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  pthread_mutex_lock(&(pinfo->mtx));
  int ret = dot3_GetPsrWithPsid(pinfo, psid, psr);
  pthread_mutex_unlock(&(pinfo->mtx));
  return ret;
}


/**
 * @copydoc Dot3_GetPsrNum
 */
int OPEN_API Dot3_GetPsrNum(void)
{
  Log(kDot3LogLevel_config, "Get the number of PSRs\n");
  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  return dot3_GetPsrNum(pinfo);
}


/**
 * @copydoc Dot3_GetAllPsrs
 */
int OPEN_API Dot3_GetAllPsrs(struct Dot3Psr *psrs_array, const Dot3PsrNum psrs_array_size)
{
  Log(kDot3LogLevel_config, "Get all PSRs\n");

  /*
   * 파라미터 유효성 체크
   */
  if(!psrs_array || !psrs_array_size) {
    Err("Fail to get all PSRs - null parameters\n");
    return -kDot3Result_Fail_NullParameters;
  }

  struct Dot3ProviderInfo *pinfo = &(g_dot3_mib.provider_info);
  pthread_mutex_lock(&(pinfo->mtx));
  int ret = dot3_GetAllPsrs(pinfo, psrs_array, psrs_array_size);
  pthread_mutex_unlock(&(pinfo->mtx));
  return ret;
}
