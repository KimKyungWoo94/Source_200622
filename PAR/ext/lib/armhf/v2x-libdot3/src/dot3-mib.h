/**
 * @file dot3-mib.h
 * @date 2019-08-16
 * @author gyun
 * @brief Management Information Base (MIB) 정의 헤더 파일
 */


#ifndef LIBDOT3_DOT3_MIB_H
#define LIBDOT3_DOT3_MIB_H

#include <pthread.h>

#include "sudo_queue.h"

#include "dot3/dot3-types.h"


/**
 * Provider Channel Info 테이블 엔트리
 *  - dot3 라이브러리 초기화 시, V2X 주파수 대역의 각 채널에 대한 초기 테이블이 생성되며, WSA 의 channel info 에 수납된다.
 *  - chan_access 정보는 초기화 시 설정되지 않고, WSA 생성 시에 업데이트 된다.
 */
struct Dot3PciTableEntry
{
  struct Dot3Pci pci;   ///< Provider Channel Info 정보
  /// (현재 미사용) Dot3ProviderChannelAccess chan_access;  ///< 채널접속 방식 (continous, alternating) TODO:: 삭제 검토
  TAILQ_ENTRY(Dot3PciTableEntry) entries;
};
TAILQ_HEAD(Dot3PciTableEntryHead, Dot3PciTableEntry);


/**
 * Provider Service Request 테이블 엔트리
 */
struct Dot3PsrTableEntry/*type*/
{
  struct Dot3Psr psr;               ///< Provider Service Request 정보
  unsigned int option_cnt;          ///< Provider Service Request 내 옵션필드 존재 개수
  struct Dot3PciTableEntry *pci_entry;  ///< 서비스채널과 연관된 Provider Channel Info 참조
  TAILQ_ENTRY(Dot3PsrTableEntry/*type*/)	entries/*field*/;
};
TAILQ_HEAD(Dot3PsrTableEntryHead/*headname*/, Dot3PsrTableEntry/*type*/);


/**
 * Provider 관련 정보
 */
struct Dot3ProviderInfo
{
  /// Provider 관련정보 동기화를 위한 뮤텍스
  pthread_mutex_t mtx;

  /// Provider Service Request 테이블
  struct {
    Dot3PsrNum num;
    struct Dot3PsrTableEntryHead head;
  } psr_table;

  /// Provider Channel Info 테이블
  struct {
    Dot3PciNum num;
    struct Dot3PciTableEntryHead head;
  } pci_table;
};


/**
 * Management Information Base (MIB)
 */
struct Dot3Mib
{
  struct Dot3ProviderInfo provider_info;  ///< Provider 관련 정보
};


#endif //LIBDOT3_DOT3_MIB_H
