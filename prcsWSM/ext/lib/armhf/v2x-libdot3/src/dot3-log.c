//
// Created by gyun on 2019-06-06.
//

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "dot3-internal.h"

Dot3LogLevel g_dot3_log = kDot3LogLevel_config;  /// 로그레벨 변수 (Dot3_Init() 함수에서 업데이트 된다)


/**
 * @brief 로그메시지를 출력한다.
 * @param func 로그 출력을 수행하는 함수 이름
 * @param format 출력 라인
 * @param ... 출력 라인
 *
 * 본 함수는 직접 호출되지 않으며, 항상 Log() 및 Err() 매크로를 통해 간접 호출된다.
 * 전달된 출력문 앞에 라이브러리명(dot3)과 함수명이 추가되어 표준에러(stderr)로 출력된다.
 */
void dot3_PrintLog(const char *func, const char *format, ...)
{
  va_list arg;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

#if defined(WIN32) | defined(_WIN32) | defined(WIN64) | defined(_WIN64)
  struct tm *tm_now;
  tm_now = localtime(&ts.tv_sec);
  fprintf(stderr, "[%04u%02u%02u.%02u%02u%02u.%06ld]", tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday,
          tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, ts.tv_nsec / 1000);
#else
  struct tm tm_now;
  localtime_r((time_t *)&ts.tv_sec, &tm_now);
  fprintf(stderr, "[%04u%02u%02u.%02u%02u%02u.%06ld]", tm_now.tm_year+1900, tm_now.tm_mon+1, tm_now.tm_mday,
          tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ts.tv_nsec / 1000);
#endif

  fprintf(stderr, "[%s][%s] ", "dot3", func);
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
}


/**
 * @brief 패킷 덤프를 출력한다.
 * @param pkt 출력할 패킷 데이터
 * @param pkt_size 패킷 데이터 크기
 */
void dot3_PrintPacketDump(const uint8_t *const pkt, const uint32_t pkt_size)
{
  for (int i = 0; i < pkt_size; i++) {
    if ((i!=0) && (i%16==0)) {
      printf("\n");
    }
    printf("%02X ", pkt[i]);
  }
  printf("\n");
}
