/**
 * @file dot3-chaninfo.c
 * @date 2019-08-17
 * @author gyun
 * @brief Provider Channel Info 테이블에 관련된 기능 구현 파일
 */


#include <stdio.h>
#include <stdlib.h>

#include "dot3-internal.h"
#include "dot3-mib.h"


/**
 * Provider Channel Info 테이블 엔트리를 기본값으로 설정한다.
 * 초기화 루틴을 제외하고는 provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param entry     설정할 Provider Channel Info 엔트리
 * @param chan_num  채널번호
 */
static void
dot3_SetDefaultChannelInfoTableEntry(struct Dot3PciTableEntry *const entry, const Dot3ChannelNumber chan_num)
{
  if ((chan_num % 2) == 0) {
    entry->pci.operating_class = kDot3OperatingClass_5G_10mhz;
  } else {
    entry->pci.operating_class = kDot3OperatingClass_5G_20mhz;
  }
  entry->pci.chan_num = chan_num;
  entry->pci.transmit_power_level = kDot3Power_MaxEirpInClassC;
  entry->pci.datarate = kDot3DataRate_TxDefault;
  entry->pci.adaptable_datarate = true;
}


/**
 * Provider Channel Info 테이블을 초기화한다.
 *
 * @param pinfo     provider info MIB
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
int INTERNAL dot3_InitPciTable(struct Dot3ProviderInfo *const pinfo)
{
  Log(kDot3LogLevel_init, "Initializing channel info table\n");

  /*
   * 테이블 정보 구조체 초기화
   */
  TAILQ_INIT(&(pinfo->pci_table.head));
  pinfo->pci_table.num = 0;

  /*
   * 각 채널에 대한 기본 Channel info 정보들을 테이블에 추가
   *  10MHz 채널에 대한 정보를 테이블 앞에 둔다.
   */
    // 10MHz/20MHz 채널들에 대한 기본 Channel info 정보 생성
  struct Dot3PciTableEntry *entry;
  for (int i = kDot3Channel_KoreaV2XMin; i <= kDot3Channel_KoreaV2XMax; i++) {
    entry = (struct Dot3PciTableEntry *)calloc(1, sizeof(struct Dot3PciTableEntry));
    if (!entry) {
      Err("Fail to initialize channel info table - no memory\n");
      return -kDot3Result_Fail_NoMemory;
    }
    dot3_SetDefaultChannelInfoTableEntry(entry, i);
    TAILQ_INSERT_TAIL(&(pinfo->pci_table.head), entry, entries);
    (pinfo->pci_table.num)++;
  }

  Log(kDot3LogLevel_event, "Success to initialize channel info table\n");
  TAILQ_FOREACH(entry, &(pinfo->pci_table.head), entries) {
    dot3_PrintPciTableEntry(kDot3LogLevel_init, entry);
  }

  return kDot3Result_Success;
}


/**
 * Provider Channel Info 테이블을 비운다.
 * provider_mtx 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 */
void INTERNAL dot3_FlushPciTable(struct Dot3ProviderInfo *const pinfo)
{
  struct Dot3PciTableEntry *entry, *tmp;
  TAILQ_FOREACH_SAFE(entry, &(pinfo->pci_table.head), entries, tmp) {
    TAILQ_REMOVE(&(pinfo->pci_table.head), entry, entries);
    free(entry);
  }
}


/**
 * Provider Channel Info 테이블 엔트리의 내용을 화면에 출력한다.
 * Provider Channel Info 테이블에 속한 엔트리 정보 출력 시에는 provider_mtx 락 상태에서 호출되어야 한다.
 *
 * @param log_level     출력 기준 로그레벨
 * @param entry         출력할 엔트리의 포인터
 */
void INTERNAL
dot3_PrintPciTableEntry(const Dot3LogLevel log_level, const struct Dot3PciTableEntry *const entry)
{
  if (g_dot3_log >= log_level) {
    const struct Dot3Pci *const pci = &(entry->pci);
    printf("  Chan: %u, OpClass: %u, TxPowerLevel(EIRP): %ddBm, DataRate: %d(*500kbps), adpatable: %u\n",
           pci->chan_num, pci->operating_class, pci->transmit_power_level, pci->datarate, pci->adaptable_datarate);
  }
}


/**
 * Provider Channel Info 의 내용을 화면에 출력한다.
 * Provider Channel Info 테이블에 속한 엔트리 정보 출력 시에는 provider_mtx 락 상태에서 호출되어야 한다.
 *
 * @param log_level     출력 기준 로그레벨
 * @param chan_info     출력할 Channel info
 */
void INTERNAL dot3_PrintPciContents(const Dot3LogLevel log_level, const struct Dot3Pci *const pci)
{
  if (g_dot3_log >= log_level) {
    printf("  Chan: %u, OpClass: %u, TxPowerLevel(EIRP): %ddBm, DataRate: %d(*500kbps), adpatable: %u\n",
           pci->chan_num, pci->operating_class, pci->transmit_power_level, pci->datarate, pci->adaptable_datarate);
  }
}


/**
 * Provider Channel Info 테이블의 내용을 화면에 출력한다.
 * provider_mtx 락 상태에서 호출되어야 한다.
 *
 * @param log_level     출력 기준 로그레벨
 * @param pinfo         provider info MIB
 */
void INTERNAL dot3_PrintPciTable(const Dot3LogLevel log_level, const struct Dot3ProviderInfo *const pinfo)
{
  if (g_dot3_log >= log_level) {
    struct Dot3PciTableEntry *entry;
    TAILQ_FOREACH(entry, &(pinfo->pci_table.head), entries) {
      dot3_PrintPciTableEntry(log_level, entry);
    }
  }
}
