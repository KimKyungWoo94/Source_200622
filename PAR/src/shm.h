#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/shm.h>

#define	GPSD_CHECK_KEY	1617

/* 함수 선언 */
int32_t InitShm(int* shmid, char **shmPtr);
int32_t ReleaseShm(char *shmPtr);

