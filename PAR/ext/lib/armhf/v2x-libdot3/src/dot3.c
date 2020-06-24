//
// Created by gyun on 2019-06-04.
//

#include <string.h>

#include "dot3-internal.h"
#include "dot3-mib.h"


struct Dot3Mib INTERNAL g_dot3_mib;


/**
 * Provider info MIB 를 초기화한다.
 *
 * @param pinfo 초기화할 provider info MIB
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_InitProviderInfo(struct Dot3ProviderInfo *const pinfo)
{
  Log(kDot3LogLevel_init, "Initializing provider info\n");
  pthread_mutex_init(&(pinfo->mtx), NULL);

  /*
   * PSR 테이블 초기화
   */
  dot3_InitPsrTable(pinfo);

  /*
   * Channel info 테이블 초기화
   */
  int ret = dot3_InitPciTable(pinfo);
  if (ret < 0) {
    dot3_FlushPciTable(pinfo);
    return ret;
  }

  Log(kDot3LogLevel_init, "Success to initialize provider info\n");
  return kDot3Result_Success;
}


/**
 * dot3 라이브러리 내부를 초기화한다.
 *
 * @return  성공시 0, 실패시 음수(-Dot3ResultCode)
 */
int INTERNAL dot3_InitDot3(void)
{
  Log(kDot3LogLevel_init, "Initializing dot3\n");
  memset(&g_dot3_mib, 0, sizeof(g_dot3_mib));

  /*
   * Provider 정보 초기화
   */
  int ret = dot3_InitProviderInfo(&g_dot3_mib.provider_info);
  if (ret < 0) {
    return ret;
  }

  Log(kDot3LogLevel_init, "Success to initialize dot3\n");
  return kDot3Result_Success;
}
