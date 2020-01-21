/**
 * @file v2x-obu-libdot3.c
 * @date 2019-08-19
 * @author gyun
 * @brief dot3 라이브러리 사용 기능 구현
 */

#include <stdio.h>
#include <syslog.h>

#include "dot3/dot3.h"

/**
 * dot3 라이브러리를 초기화한다.
 *
 * @param log_level 로그메시지 출력 레벨
 * @return          성공 시 0, 실패 시 -1
 */
int V2X_OBU_InitDot3Library(int log_level)
{
    //printf("Initializing dot3 library - log_level: %d\n", log_level);
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Initializing dot3 library - log_level: %d\n", log_level);

    /*
     * 라이브러리 초기화
     */
    int ret = Dot3_Init(log_level);
    if (ret < 0) {
        //printf("Fail to Dot3_Init() - %d\n", ret);
        syslog(LOG_ERR | LOG_LOCAL1, "[prcsWSM] Fail to Dot3_Init() - %d\n", ret);
        return -1;
    }

    //printf("Success to initialize dot3 library\n");
    syslog(LOG_INFO | LOG_LOCAL0, "[prcsWSM] Success to initialize dot3 library\n");
    return 0;
}
