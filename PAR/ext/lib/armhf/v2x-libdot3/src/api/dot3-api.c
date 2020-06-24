//
// Created by gyun on 2019-06-06.
//

#include "dot3/dot3.h"
#include "dot3-internal.h"

/**
 * dot3 라이브러리르 초기화한다.
 */
int OPEN_API Dot3_Init(int log_level)
{
  /*
   * 로그레벨 설정
   */
  g_dot3_log = log_level;
  if (g_dot3_log < kDot3LogLevel_min) {
    g_dot3_log = kDot3LogLevel_min;
  }
  else if (g_dot3_log > kDot3LogLevel_max) {
    g_dot3_log = kDot3LogLevel_max;
  }
  else {}

  Log(kDot3LogLevel_init, "Initializing dot3 library - log level: %d\n", g_dot3_log);

  /*
   * dot3 초기화
   */
  dot3_InitDot3();

  return kDot3Result_Success;
}
