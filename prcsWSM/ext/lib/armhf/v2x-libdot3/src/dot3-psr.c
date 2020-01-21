/**
 * @file dot3-psr.c
 * @date 2019-08-16
 * @author gyun
 * @brief Provider Service Request 관련 기능 구현 파일
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dot3/dot3.h>

#include "sudo_queue.h"

#include "dot3/dot3-types.h"
#include "dot3-internal.h"
#include "dot3-mib.h"


/**
 * PSR 테이블을 초기화한다.
 *
 * @param pinfo     provider info MIB
 */
void INTERNAL dot3_InitPsrTable(struct Dot3ProviderInfo *const pinfo)
{
  TAILQ_INIT(&(pinfo->psr_table.head));
  pinfo->psr_table.num = 0;
}


/**
 * PSR 테이블에서 특정 PSID를 갖는 PSR을 찾아 반환한다.
 *
 * @param pinfo     provider info MIB
 * @param psid      찾고자 하는 PSID
 * @return          성공 시 해당 엔트리의 포인터, 실패 시 NULL
 */
static struct Dot3PsrTableEntry* dot3_FindPsrWithPsid(const struct Dot3ProviderInfo *const pinfo, const Dot3Psid psid)
{
  struct Dot3PsrTableEntry *psr_entry;
  TAILQ_FOREACH(psr_entry, &(pinfo->psr_table.head), entries) {
    if (psr_entry->psr.psid == psid) {
      return psr_entry;
    }
  }
  return NULL;
}


/**
 * PSR을 테이블에 추가한다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 * @param psr       @ref Dot3_AddPsr
 */
int INTERNAL dot3_AddPsr(struct Dot3ProviderInfo *const pinfo, const struct Dot3Psr *const psr)
{
  Log(kDot3LogLevel_config, "Adding PSR with psid: %u\n", psr->psid);

  /*
   * 최대 개수 초과 확인
   */
  if (pinfo->psr_table.num == kDot3PsrNum_MaxNum) {
    Err("Fail to add PSR - table is full (%u)\n", pinfo->psr_table.num);
    return -kDot3Result_Fail_PsrTableFull;
  }

  /*
   * 중복 PSR 여부 확인
   */
  if(dot3_FindPsrWithPsid(pinfo, psr->psid)) {
    Err("Fail to add PSR - PSR with same psid %u exists in table\n", psr->psid);
    return -kDot3Result_Fail_SamePsidPsr;
  }

  /*
   * PSR 엔트리 추가
   */
  int ret;
  {
    // PSR 엔트리 할당 및 값 저장
    struct Dot3PsrTableEntry *psr_entry = (struct Dot3PsrTableEntry *)calloc(1, sizeof(struct Dot3PsrTableEntry));
    if (!psr_entry) {
      Err("Fail to add PSR - %s\n", strerror(errno));
      return -kDot3Result_Fail_NoMemory;
    }
    memcpy(&psr_entry->psr, psr, sizeof(struct Dot3Psr));
    if (psr->ip_service) { psr_entry->option_cnt += 2; }
    if (psr->present.psc) { psr_entry->option_cnt++; }
    if (psr->present.provider_mac_addr) { psr_entry->option_cnt++; }
    if (psr->present.rcpi_threshold) { psr_entry->option_cnt++; }
    if (psr->present.wsa_cnt_threshold) { psr_entry->option_cnt++; }
    if (psr->present.wsa_cnt_threshold_interval) { psr_entry->option_cnt++; }

    // Channel info 정보 참조
    bool found = false;
    struct Dot3PciTableEntry *pci_entry;
    TAILQ_FOREACH(pci_entry, &(pinfo->pci_table.head), entries) {
      if (psr->service_chan_num == pci_entry->pci.chan_num) {
        psr_entry->pci_entry = pci_entry;
        Log(kDot3LogLevel_config, "Channel info for channel %d is referenced - %p\n",
            psr->service_chan_num, psr_entry->pci_entry);
        found = true;
        break;
      }
    }
    if (!found) {
      Err("Fail to add PSR - cannot find channel info for service channel %d\n", psr->service_chan_num);
      free(psr_entry);
      return -kDot3Result_Fail_NoRelatedChannelInfo;
    }

    // 테이블에 추가
    TAILQ_INSERT_TAIL(&(pinfo->psr_table.head), psr_entry, entries);
    ret = ++(pinfo->psr_table.num);
  }

  /*
   * PSR 엔트리 개수 반환
   */
  Log(kDot3LogLevel_config, "Success to add PSR - %d entries present\n", ret);
  return ret;
}


/**
 * PSR을 테이블에서 삭제한다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 * @param psid      @ref Dot3_DeletePsr
 */
int INTERNAL dot3_DeletePsr(struct Dot3ProviderInfo *const pinfo, const Dot3Psid psid)
{
  int ret;
  Log(kDot3LogLevel_config, "Deleting PSR with psid %u\n", psid);

  /*
   * PSR 엔트리를 탐색하여 삭제
   */
  struct Dot3PsrTableEntry *entry, *tmp, *del = NULL;
  TAILQ_FOREACH_SAFE(entry, &(pinfo->psr_table.head), entries, tmp) {
    if (entry->psr.psid == psid) {
      TAILQ_REMOVE(&(pinfo->psr_table.head), entry, entries);
      ret = --(pinfo->psr_table.num);
      del = entry;
      break;
    }
  }

  /*
   * 찾으면 삭제, 못 찾으면 실패
   */
  if (del) {
    free(del);
  } else {
    Err("Fail to delete PSR - no such PSR with psid %u\n", psid);
    return -kDot3Result_Fail_NoSuchPsr;
  }

  Log(kDot3LogLevel_config, "Success to delete PSR - %d entries present\n", ret);
  return ret;
}


/**
 * 테이블 내 모든 PSR을 삭제한다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 */
void INTERNAL dot3_DeleteAllPsrs(struct Dot3ProviderInfo *const pinfo)
{
  Log(kDot3LogLevel_config, "Deleting all PSRs\n");

  /*
   * 테이블 내 모든 PSR들을 삭제한다.
   */
  struct Dot3PsrTableEntry *entry, *tmp;
  TAILQ_FOREACH_SAFE(entry, &(pinfo->psr_table.head), entries, tmp) {
    TAILQ_REMOVE(&(pinfo->psr_table.head), entry, entries);
    free(entry);
  }
  pinfo->psr_table.num = 0;
}


/**
 * 특정 PSID를 갖는 PSR 정보를 반환한다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 * @param psid      @ref Dot3_GetPsrWithPsid
 * @param psr       @ref Dot3_GetPsrWithPsid
 */
int INTERNAL dot3_GetPsrWithPsid(const struct Dot3ProviderInfo *const pinfo, const Dot3Psid psid, struct Dot3Psr *const psr)
{
  Log(kDot3LogLevel_config, "Get PSR with psid %u\n", psid);

  /*
   * 테이블을 탐색한다.
   */
  bool found = false;
  struct Dot3PsrTableEntry *entry;
  TAILQ_FOREACH(entry, &(pinfo->psr_table.head), entries) {
    if (entry->psr.psid == psid) {
      found = true;
      break;
    }
  }

  /*
   * 찾으면 반환, 못 찾으면 실패
   */
  if (found) {
    memcpy(psr, &entry->psr, sizeof(struct Dot3Psr));
  } else {
    Err("Fail to get PSR - no such PSR with psid %u\n", psid);
    return -kDot3Result_Fail_NoSuchPsr;
  }

  Log(kDot3LogLevel_config, "Success to get PSR\n");
  return kDot3Result_Success;
}


/**
 * 현재 테이블에 저장되어 있는 PSR의 개수를 반환한다.
 *
 * @param pinfo     provider info MIB
 */
int INTERNAL dot3_GetPsrNum(const struct Dot3ProviderInfo *const pinfo)
{
  int ret = pinfo->psr_table.num;
  Log(kDot3LogLevel_config, "Get the number of PSR - %d\n", ret);
  return ret;
}


/**
 * 테이블 내 모든 PSR의 정보를 반환한다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo             provider info MIB
 * @param psrs_array        @ref Dot3_GetAllPsrs
 * @param psrs_array_size   @ref Dot3_GetAllPsrs
 */
int INTERNAL dot3_GetAllPsrs(
  const struct Dot3ProviderInfo *const pinfo,
  struct Dot3Psr *psrs_array,
  const Dot3PsrNum psrs_array_size)
{
  /*
   * 테이블 내 모든 PSR 정보를 반환 배열에 복사한다.
   *  - 배열 크기와 엔트리 개수 중 작은 값만큼만 반환한다. (오버플로우 방지)
   */
  uint32_t min_cnt = (psrs_array_size > pinfo->psr_table.num) ? pinfo->psr_table.num : psrs_array_size;
  struct Dot3Psr *ptr;
  struct Dot3PsrTableEntry *entry;
  uint32_t copied = 0;
  TAILQ_FOREACH(entry, &(pinfo->psr_table.head), entries) {
    ptr = psrs_array + copied++;
    memcpy(ptr, &entry->psr, sizeof(struct Dot3Psr));
    if (copied == min_cnt) {
      break;
    }
  }

  /*
   * 복사된 개수를 반환한다.
   */
  Log(kDot3LogLevel_config, "Get all PSRS - there are %d entries\n", copied);
  return copied;
}


/**
 * PSR의 내용을 화면에 출력한다.
 * PSR 테이블 내 엔트리에 속한 PSR 출력 시에는 provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param log_level     출력 기준 로그레벨
 * @param psr   화면에 출력할 PSR
 */
void INTERNAL dot3_PrintPsrContents(const Dot3LogLevel log_level, const struct Dot3Psr *const psr)
{
  if (g_dot3_log >= log_level) {
    printf("wsa_id: %u, psid: %u, service_chan: %d, chan_access: %u\n",
      psr->wsa_id, psr->psid, psr->service_chan_num, psr->chan_access);
    if (psr->present.psc) {
      printf("psc(%u): 0x", psr->psc.len);
      for (int i = 0; i < psr->psc.len; i++) {
        printf("%02X", psr->psc.psc[i]);
      }
      printf("\n");
    }
    if (psr->present.provider_mac_addr) {
      printf("provider_mac_addr: %02X:%02X:%02X:%02X:%02X:%02X, ",
             psr->provider_mac_addr[0], psr->provider_mac_addr[1], psr->provider_mac_addr[2],
             psr->provider_mac_addr[3], psr->provider_mac_addr[4], psr->provider_mac_addr[5]);
    }
    if (psr->present.rcpi_threshold) {
      printf("rcpi_threshold: %d, ", psr->rcpi_threshold);
    }
    if (psr->present.wsa_cnt_threshold) {
      printf("wsa_cnt_threshold: %u, ", psr->wsa_cnt_threshold);
    }
    if (psr->present.wsa_cnt_threshold_interval) {
      printf("wsa_cnt_threshold_interval: %u, ", psr->wsa_cnt_threshold_interval);
    }
    if (psr->ip_service) {
      char ipv6_addr_str[IPV6_ADDR_STR_MAX_LEN];
      inet_ntop(AF_INET6, psr->ipv6_address, ipv6_addr_str, sizeof(ipv6_addr_str));
      printf("ipv6_addr : %s, port: %u, ", ipv6_addr_str, psr->service_port);
    }
    printf("\n");
  }
}
