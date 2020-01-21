/**
 * @file dot3-ffasn1c-wsa-encode.c
 * @date 2019-08-17
 * @author gyun
 * @brief ffasn1c 라이브러리를 이용하여 WSA를 인코딩하는 기능을 정의
 */

#include <pthread.h>
#include <string.h>

#include "sudo_queue.h"

#include "asn1defs_int.h"
#include "dot3-asn.h"

#include "dot3/dot3-types.h"
#include "dot3-ffasn1c.h"
#include "dot3-internal.h"


const int kSrvAdvMsgVersionNo = 3; ///< WSA version = 3


/**
 * WSA asn.1 정보구조체의 헤더정보를 채운다.
 *
 * @param params    @ref Dot3_ConstructWsa
 * @param wsa_msg   정보를 채울 정보구조체의 포인터
 * @return          성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
static int
dot3_FFAsn1c_FillWsaHeader(const struct Dot3ConstructWsaParams *const params, struct SrvAdvMsg *const wsa_msg)
{
  Log(kDot3LogLevel_event, "Filling WSA header\n");

  /*
   * WSA 헤더 필수 필드를 채운다.
   *  - messageID, version, wsa id, content count
   */
  wsa_msg->version.messageID = SrvAdvMessageType_saMessage;   // IEEE 1609.3 WSA
  wsa_msg->version.rsvAdvPrtVersion = kSrvAdvMsgVersionNo;
  wsa_msg->body.changeCount.saID = params->hdr.wsa_id;
  wsa_msg->body.changeCount.contentCount = params->hdr.content_count;

  /*
   * WSA 헤더 확장 필드 개수 카운트 (확장필드 메모리 할당을 위해)
   */
  wsa_msg->body.extensions.count = 0;
  if (params->hdr.extensions.repeat_rate) {
    wsa_msg->body.extensions.count++;
  }
  if (params->hdr.extensions.twod_location) {
    wsa_msg->body.extensions.count++;
  }
  if (params->hdr.extensions.threed_location) {
    wsa_msg->body.extensions.count++;
  }
  if (params->hdr.extensions.advertiser_id) {
    wsa_msg->body.extensions.count++;
  }

  /*
   * 확장필드를 채운다.
   */
  if (wsa_msg->body.extensions.count) {

    // 확장필드 저장 메모리 할당
    wsa_msg->body.extensions_option = true;
    wsa_msg->body.extensions.tab = (struct SrvAdvMsgHeaderExt *)asn1_mallocz(
      asn1_get_size(asn1_type_SrvAdvMsgHeaderExt) * wsa_msg->body.extensions.count);
    if (!wsa_msg->body.extensions.tab) {
      Err("Fail to fill WSA header - fail to asn1_malloc(SrvAdvMsgHeaderExts.tab)\n");
      return -kDot3Result_Fail_NoMemory;
    }
    uint8_t *ptr = (uint8_t *)wsa_msg->body.extensions.tab;

    // repeat rate 확장필드 추가
    if (params->hdr.extensions.repeat_rate) {
      struct SrvAdvMsgHeaderExt *ext = (struct SrvAdvMsgHeaderExt *)ptr;
      ptr += asn1_get_size(asn1_type_SrvAdvMsgHeaderExt);
      ext->extensionId = kDot3ExtensionId_RepeatRate;
      ext->value.type = (ASN1CType *)asn1_type_RepeatRate;
      ext->value.u.data = asn1_mallocz_value(asn1_type_RepeatRate);
      if (!ext->value.u.data) {
        Err("Fail to fill WSA header - fail to asn1_mallocz_value(asn1_type_RepeatRate)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(int *)(ext->value.u.data) = (int)params->hdr.repeat_rate;
    }

    // 2DLocation 확장필드 추가
    if (params->hdr.extensions.twod_location) {
      struct SrvAdvMsgHeaderExt *ext = (struct SrvAdvMsgHeaderExt *)ptr;
      ptr += asn1_get_size(asn1_type_SrvAdvMsgHeaderExt);
      ext->extensionId = kDot3ExtensionId_2DLocation;
      ext->value.type = (ASN1CType *)asn1_type_TwoDLocation;
      ext->value.u.data = asn1_mallocz_value(asn1_type_TwoDLocation);
      if (!ext->value.u.data) {
        Err("Fail to fill WSA header - fail to asn1_mallocz_value(asn1_type_TwoDLocation)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      struct TwoDLocation *location = (struct TwoDLocation *)ext->value.u.data;
      location->latitude.fill.len = 1;
      location->latitude.fill.buf = asn1_mallocz(1);
      if (!(location->latitude.fill.buf)) {
        Err("Fail to fill WSA header - fail to asn1_mallocz(latitude.fill.buf)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(location->latitude.fill.buf) = 0 << 7;
      location->latitude.lat = params->hdr.twod_location.latitude;
      location->longitude = params->hdr.twod_location.longitude;
    }

    // 3DLocation 확장필드 추가
    if (params->hdr.extensions.threed_location) {
      struct SrvAdvMsgHeaderExt *ext = (struct SrvAdvMsgHeaderExt *)ptr;
      ptr += asn1_get_size(asn1_type_SrvAdvMsgHeaderExt);
      ext->extensionId = kDot3ExtensionId_3DLocation;
      ext->value.type = (ASN1CType *)asn1_type_ThreeDLocation;
      ext->value.u.data = asn1_mallocz_value(asn1_type_ThreeDLocation);
      if (!ext->value.u.data) {
        Err("Fail to fill WSA header - fail to asn1_mallocz_value(asn1_type_ThreeDLocation)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      struct ThreeDLocation *location = (struct ThreeDLocation *)ext->value.u.data;
      location->latitude.fill.len = 1;
      location->latitude.fill.buf = asn1_mallocz(1);
      if (!(location->latitude.fill.buf)) {
        Err("Fail to fill WSA header - fail to asn1_mallocz(latitude.fill.buf)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(location->latitude.fill.buf) = 0 << 7;
      location->latitude.lat = params->hdr.threed_location.latitude;
      location->longitude = params->hdr.threed_location.longitude;
      location->elevation = params->hdr.threed_location.elevation;
    }

    // Advertiser ID 확장필드 추가
    if (params->hdr.extensions.advertiser_id) {
      struct SrvAdvMsgHeaderExt *ext = (struct SrvAdvMsgHeaderExt *)ptr;
      ptr += asn1_get_size(asn1_type_SrvAdvMsgHeaderExt);
      ext->extensionId = kDot3ExtensionId_AdvertiserId;
      ext->value.type = (ASN1CType *)asn1_type_AdvertiserIdentifier;
      ext->value.u.data = asn1_mallocz_value(asn1_type_AdvertiserIdentifier);
      if (!ext->value.u.data) {
        Err("Fail to fill WSA header - fail to asn1_mallocz_value(asn1_type_AdvertiserIdentifier)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      struct ASN1String *adv_id = (struct ASN1String *)ext->value.u.data;
      adv_id->len = params->hdr.advertiser_id.len;
      adv_id->buf = asn1_malloc(adv_id->len+1);
      if (!adv_id->buf) {
        Err("Fail to fill WSA header - fail to asn1_malloc(adv_id->len)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      memcpy(adv_id->buf, params->hdr.advertiser_id.id, adv_id->len);
      adv_id->buf[adv_id->len] = 0;
    }
  }

  Log(kDot3LogLevel_event, "Success to fill WSA header\n");
  return kDot3Result_Success;
}


/**
 * 하나의 Service info instance 의 내용을 채운다.
 *  - PSR 테이블 뮤텍스 락 내에서 호출된다.
 *
 * @param entry                     PSR 정보 테이블 엔트리
 * @param service_info_instance     정보를 채울 service info 정보 구조체
 * @return                          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_AddWsaServiceInfoInstance(
  const struct Dot3PsrTableEntry *const entry,
  struct ServiceInfo *const service_info_instance)
{
  Log(kDot3LogLevel_event, "Adding WSA service info instance for psid %u\n", entry->psr.psid);

  /*
   * PSID 필수필드를 채운다.
   */
  int ret = dot3_FFAsn1c_FillVarLengthNumber(entry->psr.psid, &(service_info_instance->serviceID));
  if (ret < 0) {
    return ret;
  }

  /*
   * Channel Index 필수필드는 나중에 채워진다.
   */

  /*
   * 옵션 필드를 채운다.
   *  - entry->option_cnt 는 Dot3_AddPsr()을 통해 PSR 등록 시에 저장된다.
   */
  if (entry->option_cnt)
  {
    Log(kDot3LogLevel_event, "Adding %u service info extensions\n", entry->option_cnt);
    service_info_instance->chOptions.extensions_option = true;
    struct ServiceInfoExts *exts = &(service_info_instance->chOptions.extensions);
    exts->count = entry->option_cnt;

    // 옵션필드를 채우기 위한 메모리 할당.
    exts->tab = (struct ServiceInfoExt *)asn1_mallocz(sizeof(struct ServiceInfoExt) * exts->count);
    if (!exts->tab) {
      Err("Fail to add WSA service info instance - fail to asn1_mallocz(ServiceInfoExt * %u)\n", exts->count);
      return -kDot3Result_Fail_NoMemory;
    }

    int cnt = 0;
    // PSC 옵션필드를 채운다.
    if (entry->psr.present.psc) {
      Log(kDot3LogLevel_event, "Adding service info extensions - %u bytes PSC\n", entry->psr.psc.len);
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_Psc;
      ext->value.type = (ASN1CType *)asn1_type_ProviderServiceContext;
      ext->value.u.data = asn1_mallocz_value(asn1_type_ProviderServiceContext);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_ProviderServiceContext)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      struct ProviderServiceContext *psc = (struct ProviderServiceContext *)ext->value.u.data;
      psc->fillBit.len = 3;
      psc->fillBit.buf = asn1_mallocz(psc->fillBit.len);
      if (!(psc->fillBit.buf)) {
        Err("Fail to fill WSA service info instance - fail to asn1_mallocz(psc->fillBit.buf)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(psc->fillBit.buf) = (0 << 7) | (0 << 6) | (0 << 5);
      psc->psc.len = entry->psr.psc.len;
      psc->psc.buf = asn1_mallocz(psc->psc.len);
      if (!psc->psc.buf) {
        Err("Fail to fill WSA service info instance - fail to asn1_mallocz(ProviderServiceContext)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      memcpy(psc->psc.buf, entry->psr.psc.psc, psc->psc.len);
    }
    // IPv6Address, ServicePort 옵션필드를 채운다.
    if (entry->psr.ip_service) {
      Log(kDot3LogLevel_event,
          "Adding service info extensions - ipv6address and service port(%u)\n", entry->psr.service_port);
      // IPv6Adddress
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_IPv6Address;
      ext->value.type = (ASN1CType *)asn1_type_IPv6Address;
      ext->value.u.data = asn1_mallocz_value(asn1_type_IPv6Address);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_IPv6Address)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      ASN1String *addr = (struct ASN1String *)ext->value.u.data;
      addr->len = IPV6_ALEN;
      addr->buf = asn1_mallocz(IPV6_ALEN);
      if (!addr->buf) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz(IPv6Address)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      memcpy(addr->buf, entry->psr.ipv6_address, IPV6_ALEN);
      // ServicePort
      ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_ServicePort;
      ext->value.type = (ASN1CType *)asn1_type_ServicePort;
      ext->value.u.data = asn1_mallocz_value(asn1_type_ServicePort);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_ServicePort)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(int *)(ext->value.u.data) = entry->psr.service_port;
    }
    // ProviderMacAddress 옵션필드를 채운다.
    if (entry->psr.present.provider_mac_addr) {
      Log(kDot3LogLevel_event, "Adding service info extensions - provider mac address\n");
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_ProviderMacAddress;
      ext->value.type = (ASN1CType *)asn1_type_ProviderMacAddress;
      ext->value.u.data = asn1_mallocz_value(asn1_type_ProviderMacAddress);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_ProviderMacAddress)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      ASN1String *addr = (struct ASN1String *)ext->value.u.data;
      addr->len = MAC_ALEN;
      addr->buf = asn1_mallocz(MAC_ALEN);
      if (!addr->buf) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz(ProviderMacAddress)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      memcpy(addr->buf, entry->psr.provider_mac_addr, MAC_ALEN);
    }
    // RcpiThreshold 옵션필드를 채운다.
    if (entry->psr.present.rcpi_threshold) {
      Log(kDot3LogLevel_event, "Adding service info extensions - rcpi threshold %u\n", entry->psr.rcpi_threshold);
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_RcpiThreshold;
      ext->value.type = (ASN1CType *)asn1_type_RcpiThreshold;
      ext->value.u.data = asn1_mallocz_value(asn1_type_RcpiThreshold);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_RcpiThreshold)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(int *)(ext->value.u.data) = entry->psr.rcpi_threshold;
    }
    // WsaCountThreshold 옵션필드를 채운다.
    if (entry->psr.present.wsa_cnt_threshold) {
      Log(kDot3LogLevel_event,
          "Adding service info extensions - wsa count threshold %u\n", entry->psr.wsa_cnt_threshold);
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_WsaCountThreshold;
      ext->value.type = (ASN1CType *)asn1_type_WsaCountThreshold;
      ext->value.u.data = asn1_mallocz_value(asn1_type_WsaCountThreshold);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_WsaCountThreshold)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(int *)(ext->value.u.data) = entry->psr.wsa_cnt_threshold;
    }
    // WsaCountThresholdInterval 옵션필드를 채운다.
    if (entry->psr.present.wsa_cnt_threshold_interval) {
      Log(kDot3LogLevel_event,
          "Adding service info extensions - wsa count threshold interval %u\n", entry->psr.wsa_cnt_threshold_interval);
      struct ServiceInfoExt *ext = exts->tab + cnt++;
      ext->extensionId = kDot3ExtensionId_WsaCountThresholdInterval;
      ext->value.type = (ASN1CType *)asn1_type_WsaCountThresholdInterval;
      ext->value.u.data = asn1_mallocz_value(asn1_type_WsaCountThresholdInterval);
      if (!ext->value.u.data) {
        Err("Fail to add WSA service info instance - fail to asn1_mallocz_value(asn1_type_WsaCountThresholdInterval)\n");
        return -kDot3Result_Fail_NoMemory;
      }
      *(int *)(ext->value.u.data) = entry->psr.wsa_cnt_threshold_interval;
    }
  }

  Log(kDot3LogLevel_event, "Success to add WSA service info instance\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체 내에 수납되어 있는 Channel info 중 원하는 채널을 갖는 channel info 의 index를 반환한다.
 *
 * @param infos         WSA asn.1 정보구조체의 Channel info 들의 리스트 포인터
 * @param info_count    현재 WSA asn.1 정보구조체에 저장되어 있는 Channel info 의 수
 * @param chan_num      찾고자 하는 채널번호
 * @return              Channel info의 인덱스(표준에 따라 1번부터 시작), 해당되는 정보가 없을 경우 0
 */
static int dot3_FFAsn1c_GetWsaChannelInfoIndexWithChannelNumber(
  const struct ChannelInfo *const infos,
  const uint32_t info_count,
  const Dot3ChannelNumber chan_num)
{
  struct ChannelInfo *cinfo = NULL;

  uint32_t count = (info_count > _WSA_CHAN_INFO_MAX_NUM_) ? _WSA_CHAN_INFO_MAX_NUM_ : info_count;

  for (int i = 0; i < count; i++) {
    cinfo = (struct ChannelInfo *)(infos + i);
    if (cinfo->channelNumber == chan_num) {
      return i + 1;
    }
  }
  return 0;
}


/**
 * WSA asn.1 정보구조체에 Channel info instance 를 추가한다.
 *
 * @param psr_entry             PSR 엔트리
 * @param chan_info_instance    추가할 Channel info instance 포인터
 * @return                      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_AddWsaChannelInfoInstance(
  const struct Dot3PsrTableEntry *const psr_entry,
  struct ChannelInfo *const chan_info_instance)
{
  Log(kDot3LogLevel_event, "Adding WSA channel info instance for channel %d\n", psr_entry->psr.service_chan_num);

  if (!(psr_entry->pci_entry)) {
    Err("Fail to add WSA channel info instance - there is no channel info referenced by PSR\n");
    return -kDot3Result_Fail_NoRelatedChannelInfo;
  }

  struct Dot3Pci *pci = &(psr_entry->pci_entry->pci);
  chan_info_instance->operatingClass = pci->operating_class;
  chan_info_instance->channelNumber = pci->chan_num;
  chan_info_instance->powerLevel = pci->transmit_power_level;
  chan_info_instance->dataRate.dataRate = pci->datarate;
  chan_info_instance->dataRate.adaptable.len = 1;
  chan_info_instance->dataRate.adaptable.buf = asn1_mallocz(1);
  if (!(chan_info_instance->dataRate.adaptable.buf)) {
    Err("Fail to add WSA channel info instance - fail to asn1_mallocz(adaptable)\n");
    return -kDot3Result_Fail_NoMemory;
  }
  *(chan_info_instance->dataRate.adaptable.buf) = (uint8_t)(pci->adaptable_datarate << 7);
  // TODO:: 확장 필드 (ChannelAccess, EdcaParameterSet) 추가

  Log(kDot3LogLevel_event, "Success to add WSA channel info instance\n");
  return kDot3Result_Success;
}



/**
 * WSA asn1. 정보 구조체의 WSA Service info segment 와 Channel info segment 정보를 채운다.
 * provider 뮤텍스 락 상태에서 호출되어야 한다.
 *
 * @param pinfo     provider info MIB
 * @param params    @ref Dot3_ConstructWsa
 * @param wsa_msg   정보를 채울 정보구조체의 포인터
 * @return          성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_FillWsaServiceInfoSegmentAndChannelInfoSegment(
  const struct Dot3ProviderInfo *const pinfo,
  const struct Dot3ConstructWsaParams *const params,
  struct SrvAdvMsg *const wsa_msg)
{
  Log(kDot3LogLevel_event, "Filling WSA service info segment and channel info segment\n");

  /*
   * 일단 PSR 개수 또는 WSA 최대수납가능수 만큼의 Service Info, Channel Info 메모리를 할당한다.
   */
  int max_num = (pinfo->psr_table.num > _WSA_SERVICE_INFO_MAX_NUM_) ? _WSA_SERVICE_INFO_MAX_NUM_:pinfo->psr_table.num;
  wsa_msg->body.serviceInfos.tab = (struct ServiceInfo *)asn1_mallocz(asn1_get_size(asn1_type_ServiceInfo) * max_num);
  if (!wsa_msg->body.serviceInfos.tab) {
    Err("Fail to fill WSA service info and channel info - fail to asn1_malloc(serviceInfos.tab)\n");
    return -kDot3Result_Fail_NoMemory;
  }
  wsa_msg->body.channelInfos.tab = (struct ChannelInfo *)asn1_mallocz(asn1_get_size(asn1_type_ChannelInfo) * max_num);
  if (!wsa_msg->body.channelInfos.tab) {
    Err("Fail to fill WSA service info and channel info - fail to asn1_malloc(channelInfos.tab)\n");
    return -kDot3Result_Fail_NoMemory;
  }

  /*
   * PSR 테이블을 탐색하며 wsa_id 가 동일한 PSR에 대한 정보를 WSA 정보구조체에 채운다.
   *  WSA 정보구조체 내에 Service Info 를 추가한다.
   *  WSA 정보구조체 내에 Channel Info 를 추가한다.
   */
  struct Dot3PsrTableEntry *psr_entry;
  struct ServiceInfo *service_info_instance;
  int ret, service_info_cnt = 0, chan_info_cnt = 0;
  TAILQ_FOREACH(psr_entry, &(pinfo->psr_table.head), entries)
  {
    if (psr_entry->psr.wsa_id != params->hdr.wsa_id) {
      continue;
    }

    // Service info instance 의 주요필드 및 옵션필드를 채운다.
    service_info_instance = (struct ServiceInfo *)(wsa_msg->body.serviceInfos.tab + service_info_cnt);
    ret = dot3_FFAsn1c_AddWsaServiceInfoInstance(psr_entry, service_info_instance);
    if (ret < 0) {
      return ret;
    }
    service_info_cnt++;

    // PSR 의 채널번호와 동일한 channel info instance가 이미 채워져 있는지 확인하여,
    //  - 채워져 있는 경우(!0), 해당 channel info instance의 index를 service info instance의 channel index 값으로 설정한다.
    //  - 채워져 있지 않은 경우(0), 새로운 channel info instance를 추가하고, service info instance의 channel index 값을 설정한다.
    int chan_index = dot3_FFAsn1c_GetWsaChannelInfoIndexWithChannelNumber(wsa_msg->body.channelInfos.tab,
                                                                          chan_info_cnt,
                                                                          psr_entry->psr.service_chan_num);
    if (chan_index) {
      service_info_instance->channelIndex = chan_index;
    }
    else {
      ret = dot3_FFAsn1c_AddWsaChannelInfoInstance(psr_entry, (wsa_msg->body.channelInfos.tab + chan_info_cnt));
      if (ret < 0) {
        return ret;
      }
      chan_info_cnt++;
      service_info_instance->channelIndex = chan_info_cnt;
    }

    // 각 instance가 WSA에 실을 수 있는 개수를 초과하면 중단한다.
    if ((service_info_cnt >= _WSA_SERVICE_INFO_MAX_NUM_) || (chan_info_cnt >= _WSA_CHAN_INFO_MAX_NUM_)) {
      break;
    }
  }

  if (service_info_cnt) {
    wsa_msg->body.serviceInfos_option = true;
    wsa_msg->body.serviceInfos.count = service_info_cnt;
  }
  if (chan_info_cnt) {
    wsa_msg->body.channelInfos_option = true;
    wsa_msg->body.channelInfos.count = chan_info_cnt;
  }

  Log(kDot3LogLevel_event, "Success to fill WSA %d service info segment and %d channel info segment\n",
      wsa_msg->body.serviceInfos.count, wsa_msg->body.channelInfos.count);
  return kDot3Result_Success;
}


/**
 * WRA asn.1 정보구조체의 헤더정보를 채운다.
 *
 * @param params    @ref Dot3_ConstructWsa
 * @param wsa_msg   정보를 채울 정보구조체의 포인터
 * @return          성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
static int
dot3_FFAsn1c_FillWra(const struct Dot3ConstructWsaParams *const params, struct SrvAdvMsg *const wsa_msg)
{
  Log(kDot3LogLevel_event, "Filling WRA\n");

  wsa_msg->body.routingAdvertisement_option = true;
  wsa_msg->body.routingAdvertisement.lifetime = params->wra.router_lifetime;
  wsa_msg->body.routingAdvertisement.ipPrefix.len = IPV6_ALEN;
  wsa_msg->body.routingAdvertisement.ipPrefix.buf = asn1_mallocz(IPV6_ALEN);
  if (NULL == wsa_msg->body.routingAdvertisement.ipPrefix.buf) {
    Err("Fail to fill WRA - No memory\n");
    return -kDot3Result_Fail_NoMemory;
  }
  memcpy(wsa_msg->body.routingAdvertisement.ipPrefix.buf, params->wra.ip_prefix, IPV6_ALEN);
  wsa_msg->body.routingAdvertisement.ipPrefixLength = params->wra.ip_prefix_len;
  wsa_msg->body.routingAdvertisement.defaultGateway.len = IPV6_ALEN;
  wsa_msg->body.routingAdvertisement.defaultGateway.buf = asn1_mallocz(IPV6_ALEN);
  if (NULL == wsa_msg->body.routingAdvertisement.defaultGateway.buf) {
    Err("Fail to fill WRA - No memory\n");
    return -kDot3Result_Fail_NoMemory;
  }
  memcpy(wsa_msg->body.routingAdvertisement.defaultGateway.buf, params->wra.default_gw, IPV6_ALEN);
  wsa_msg->body.routingAdvertisement.primaryDns.len = IPV6_ALEN;
  wsa_msg->body.routingAdvertisement.primaryDns.buf = asn1_mallocz(IPV6_ALEN);
  if (NULL == wsa_msg->body.routingAdvertisement.primaryDns.buf) {
    Err("Fail to fill WRA - No memory\n");
    return -kDot3Result_Fail_NoMemory;
  }
  memcpy(wsa_msg->body.routingAdvertisement.primaryDns.buf, params->wra.primary_dns, IPV6_ALEN);

  // TODO:: 확장 영역  (secondary dns, gateway mac address)

  Log(kDot3LogLevel_event, "Success to fill WRA\n");
  return kDot3Result_Success;
}


/**
 * ffasn1c 라이브러리를 이용하여 WSA를 인코딩한다.
 *
 * @param pinfo         provider info MIB
 * @param params        @ref Dot3_ConstructWsa
 * @param outbuf        @ref Dot3_ConstructWsa
 * @param outbuf_size   @ref Dot3_ConstructWsa
 * @return              성공시 인코딩된 WSA 길이, 실패시 음수(-Dot3ResultCode)
 */
int INTERNAL dot3_FFAsn1c_EncodeWsa(
  struct Dot3ProviderInfo *const pinfo,
  const struct Dot3ConstructWsaParams *const params,
  uint8_t *const outbuf,
  const Dot3PduSize outbuf_size)
{
  int ret;
  Log(kDot3LogLevel_event, "Encoding WSA\n");

  /*
   * 인코딩을 위한 WSA asn.1 정보구조체를 할당하고 초기화한다.
   */
  struct SrvAdvMsg *wsa_msg = (struct SrvAdvMsg *)asn1_mallocz_value(asn1_type_SrvAdvMsg);
  if (!wsa_msg) {
    Err("Fail to encode WSA - fail to asn1_mallocz_value(SrvAdvMsg)\n");
    return -kDot3Result_Fail_NoMemory;
  }

  /*
   * WSA asn.1 정보구조체의 헤더를 채운다.
   *  - version, change count, extensions 까지.
   */
  ret = dot3_FFAsn1c_FillWsaHeader(params, wsa_msg);
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * asn.1 정보 구조체의 Service info segment 와 Channel info segment 를 채운다.
   */
  pthread_mutex_lock(&(pinfo->mtx));
  ret = dot3_FFAsn1c_FillWsaServiceInfoSegmentAndChannelInfoSegment(pinfo, params, wsa_msg);
  pthread_mutex_unlock(&(pinfo->mtx));
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * asn.1 정보 구조체의 WRA 필드를 채운다.
   */
  if (params->present.wra) {
    ret = dot3_FFAsn1c_FillWra(params, wsa_msg);
    if (ret < 0) {
      asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
      return ret;
    }
  }

  /*
   * 인코딩하고 결과 유효성을 검증한다.
   */
  uint8_t *buf;
  asn1_ssize_t encoded_wsa_size = asn1_uper_encode(&buf, asn1_type_SrvAdvMsg, wsa_msg);
  // 인코딩 실패
  if ((encoded_wsa_size < 0) || (buf == NULL)) {
    Err("Fail to encode WSA - fail to asn1_uper_encode()\n");
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return -kDot3Result_Fail_Asn1Encode;
  }
  // 인코딩 길이가 허용되는 최대길이보다 크면 실패
  if (encoded_wsa_size > kWsmBodySafeMaxSize) {
    Err("Fail to encode WSA - Too long encoded WSA: %d\n", encoded_wsa_size);
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    asn1_free(buf);
    return -kDot3Result_Fail_TooLongWsa;
  }
  // 인코딩 길이가 outbuf의 크기보다 크면 실패
  if (encoded_wsa_size > outbuf_size) {
    Err("Fail to encode WSA - Insufficient buffer size than encoded: %d < %d\n", outbuf_size, encoded_wsa_size);
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    asn1_free(buf);
    return -kDot3Result_Fail_InsufficientBuf;
  }

  /*
   * 인코딩 데이터 반환
   */
  memcpy(outbuf, buf, encoded_wsa_size);

  /*
   * asn.1 정보구조체 및 인코딩 버퍼 메모리 해제
   */
  asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
  asn1_free(buf);

  Log(kDot3LogLevel_event, "Success to encode %d-bytes WSA\n", encoded_wsa_size);
  return encoded_wsa_size;
}
