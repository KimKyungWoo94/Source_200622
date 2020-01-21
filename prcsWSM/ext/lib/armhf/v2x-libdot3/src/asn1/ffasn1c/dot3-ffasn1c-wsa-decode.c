/**
 * @file dot3-ffasn1c-wsa-decode.c
 * @date 2019-08-19
 * @author gyun
 * @brief ffasn1c 라이브러리를 이용하여 WSA를 디코딩하는 기능을 구현 파일
 */

#include <string.h>

#include "asn1defs.h"

#include "dot3/dot3-types.h"
#include "dot3-asn.h"
#include "dot3-ffasn1c.h"
#include "dot3-internal.h"


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 필수필드를 파싱하여 저장한다.
 *
 * @param wsa_msg   디코딩된 WSA asn.1 정보구조체
 * @param hdr       파싱된 헤더 정보가 저장될 구조체 포인터
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrMandatory(const struct SrvAdvMsg *const wsa_msg, struct Dot3WsaHdr *const hdr)
{
  Log(kDot3LogLevel_event, "Parsing WSA header mandatory fields\n");

  if (SrvAdvMessageType_saMessage != wsa_msg->version.messageID) {
    Err("Fail to parse WSA header - invalid message type %d\n", wsa_msg->version.messageID);
    return -kDot3Result_Fail_InvalidWsaMessageType;
  }
  hdr->version = wsa_msg->version.rsvAdvPrtVersion;
  if (kDot3WsaVersion_Current != hdr->version) {
    Err("Fail to parse WSA header - invalid version %d\n", hdr->version);
    return -kDot3Result_Fail_InvalidWsaVersionValue;
  }
  hdr->wsa_id = wsa_msg->body.changeCount.saID;
  if (hdr->wsa_id > kDot3WsaMaxId) {
    Err("Fail to parse WSA header - invalid wsa_id %u\n", hdr->wsa_id);
    return -kDot3Result_Fail_InvalidWsaIdValue;
  }
  hdr->content_count = wsa_msg->body.changeCount.contentCount;
  if (hdr->content_count > kDot3WsaMaxContentCount) {
    Err("Fail to parse WSA header - invalid content_count %u\n", hdr->content_count);
    return -kDot3Result_Fail_InvalidWsaContentCountValue;
  }

  Log(kDot3LogLevel_event, "Success to parse WSA header mandatory fields\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 RepeatRate 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 RepeatRate extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA 헤더 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrRepeatRateExtension(const void *const ext, struct Dot3WsaHdr *const hdr)
{
  Log(kDot3LogLevel_event, "Parsing WSA header RepeatRate extension\n");

  if (NULL == ext) {
    Err("Fail to parse WSA header RepeatRate extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  hdr->repeat_rate = *(uint8_t *)ext;
  hdr->extensions.repeat_rate = true;

  Log(kDot3LogLevel_event, "Success to parse WSA header RepeatRate extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 2DLocation 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 2DLocation extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA 헤더 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrTwoDLocationExtension(const void *const ext, struct Dot3WsaHdr *const hdr)
{
  Log(kDot3LogLevel_event, "Parsing WSA header 2DLocation extension\n");

  if (NULL == ext) {
    Err("Fail to parse WSA header 2DLocation extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  struct TwoDLocation *location = (struct TwoDLocation *)ext;
  hdr->twod_location.latitude = location->latitude.lat;
  hdr->twod_location.longitude = location->longitude;
  hdr->extensions.twod_location = true;

  Log(kDot3LogLevel_event, "Success to parse WSA header 2DLocation extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 3DLocation 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 3DLocation extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA 헤더 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrThreeDLocationExtension(const void *const ext, struct Dot3WsaHdr *const hdr)
{
  Log(kDot3LogLevel_event, "Parsing WSA header 3DLocation extension\n");

  if (NULL == ext) {
    Err("Fail to parse WSA header 3DLocation extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  struct ThreeDLocation *location = (struct ThreeDLocation *)ext;
  hdr->threed_location.latitude = location->latitude.lat;
  hdr->threed_location.longitude = location->longitude;
  hdr->threed_location.elevation = location->elevation;
  hdr->extensions.threed_location = true;

  Log(kDot3LogLevel_event, "Success to parse WSA header 3DLocation extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 AdvertiserId 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 AdvertiserId extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA 헤더 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrAdvertiserIdExtension(const void *const ext, struct Dot3WsaHdr *const hdr)
{
  Log(kDot3LogLevel_event, "Parsing WSA header AdvertiserId extension\n");

  if (NULL == ext) {
    Err("Fail to parse WSA header AdvertiserId extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  ASN1String *addr = (struct ASN1String *)ext;
  if ((addr->len < kDot3WsaAdvertiserIdLen_Min) || (addr->len > kDot3WsaAdvertiserIdLen_Max)) {
    Err("Fail to parse WSA header AdvertiserId extension - invalid length %d\n", addr->len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == addr->buf) {
    Err("Fail to parse WSA header AdvertiserId extension - null data\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  hdr->advertiser_id.len = addr->len;
  memcpy(hdr->advertiser_id.id, addr->buf, addr->len);
  hdr->extensions.advertiser_id = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info AdvertiserId extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더의 확장필드를 파싱하여 저장한다.
 *
 * @param wsa_msg   디코딩된 WSA asn.1 정보구조체
 * @param hdr       파싱된 헤더 정보가 저장될 구조체 포인터
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdrExtensions(const struct SrvAdvMsg *const wsa_msg, struct Dot3WsaHdr *const hdr)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA header extensions\n");

  const struct SrvAdvMsgHeaderExts *const exts = &(wsa_msg->body.extensions);
  const struct SrvAdvMsgHeaderExt *ext;

  for (int j = 0; j < exts->count; j++)
  {
    ext = exts->tab + j;
    if (NULL == ext) {
      Err("Fail to parse WSA header extensions - nul extensions\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }
    switch (ext->extensionId)
    {
      case kDot3ExtensionId_RepeatRate:
        ret = dot3_FFAsn1c_ParseWsaHdrRepeatRateExtension(ext->value.u.data, hdr);
        if (ret < 0) {
          return ret;
        }
        break;
      case kDot3ExtensionId_2DLocation: {
        ret = dot3_FFAsn1c_ParseWsaHdrTwoDLocationExtension(ext->value.u.data, hdr);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_3DLocation: {
        ret = dot3_FFAsn1c_ParseWsaHdrThreeDLocationExtension(ext->value.u.data, hdr);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_AdvertiserId: {
        ret = dot3_FFAsn1c_ParseWsaHdrAdvertiserIdExtension(ext->value.u.data, hdr);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      default: {
        Err("Fail to parse WSA header extensions - invalid extensionId %d\n", ext->extensionId);
        return -kDot3Result_Fail_InvalidWsaHdrExtensionIdValue;
      }
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA header extensions\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA 헤더를 파싱하여 저장한다.
 *
 * @param wsa_msg   디코딩된 WSA asn.1 정보구조체
 * @param hdr       파싱된 헤더 정보가 저장될 구조체 포인터
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsaHdr(const struct SrvAdvMsg *const wsa_msg, struct Dot3WsaHdr *const hdr)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA header\n");

  /*
   * 필수필드 파싱
   */
  ret = dot3_FFAsn1c_ParseWsaHdrMandatory(wsa_msg, hdr);
  if (ret < 0) {
    return ret;
  }

  /*
   * (존재하는 경우) 확장필드 파싱
   */
  if (wsa_msg->body.extensions_option) {
    ret = dot3_FFAsn1c_ParseWsaHdrExtensions(wsa_msg, hdr);
    if (ret < 0) {
      return ret;
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA header\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 필수필드를 파싱하여 저장한다.
 *
 * @param service_info  파싱할 WSA Service info asn.1 정보구조체
 * @param wsi           파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiMandatory(
  const struct ServiceInfo *const service_info,
  struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info mandatory fields\n");

  int psid = dot3_FFAsn1c_ParseVarLengthNumber(&(service_info->serviceID));
  if (psid < 0) {
    return psid;
  }
  if (psid > kDot3Psid_Max) {
    Err("Fail to parse WSA Service Info mandatory fields - invalid psid value %d\n", psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }
  wsi->psid = psid;
  wsi->channel_index = service_info->channelIndex;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info mandatory fields\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 PSC 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 PSC extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiPscExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info PSC extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info PSC extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  struct ProviderServiceContext *psc = (struct ProviderServiceContext *)ext;
  if (psc->psc.len > kDot3PscLen_Max) {
    Err("Fail to parse WSA Service Info PSC extension - too long psc %d\n", psc->psc.len);
    return -kDot3Result_Fail_TooLongPsc;
  }
  if (NULL == psc->psc.buf) {
    Err("Fail to parse WSA Service Info PSC extension - null data\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  wsi->psc.len = psc->psc.len;
  memcpy(wsi->psc.psc, psc->psc.buf, wsi->psc.len);
  wsi->psc.psc[wsi->psc.len] = 0;
  wsi->extensions.psc = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info PSC extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 IPv6Address 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 IPv6Address extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiIPv6AddressExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info IPv6Address extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info IPv6Address extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  ASN1String *addr = (struct ASN1String *)ext;
  if (IPV6_ALEN != addr->len) {
    Err("Fail to parse WSA Service Info IPv6Address extension - invalid length %d\n", addr->len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == addr->buf) {
    Err("Fail to parse WSA Service Info IPv6Address extension - null data\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  memcpy(wsi->ipv6_address, addr->buf, IPV6_ALEN);
  wsi->extensions.ipv6_address = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info IPv6Address extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 ServicePort 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 ServicePort extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiServicePortExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info ServicePort extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info ServicePort extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  wsi->service_port = *(uint16_t *)ext;
  wsi->extensions.service_port = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info ServicePort extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 ProviderMacAddress 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 ProviderMacAddress extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiProviderMacAddressExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info ProviderMacAddress extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info ProviderMacAddress extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  ASN1String *addr = (struct ASN1String *)ext;
  if (MAC_ALEN != addr->len) {
    Err("Fail to parse WSA Service Info ProviderMacAddress extension - invalid length %d\n", addr->len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == addr->buf) {
    Err("Fail to parse WSA Service Info ProviderMacAddress extension - null data\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  memcpy(wsi->provider_mac_address, addr->buf, MAC_ALEN);
  wsi->extensions.provider_mac_address = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info ProviderMacAddress extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 RcpiThreshold 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 RcpiThreshold extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiRcpiThresholdExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info RcpiThreshold extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info RcpiThreshold extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  wsi->rcpi_threshold = *(uint8_t *)ext;
  wsi->extensions.rcpi_threshold = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info RcpiThreshold extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 WsaCountThreshold 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 WsaCountThreshold extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiWsaCountThresholdExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info WsaCountThreshold extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info WsaCountThreshold extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  wsi->wsa_cnt_threshold = *(uint8_t *)ext;
  wsi->extensions.wsa_cnt_threshold = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info WsaCountThreshold extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 WsaCountThresholdInterval 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 WsaCountThresholdInterval extension 정보구조체
 * @param wsi   파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiWsaCountThresholdIntervalExtension(const void *const ext, struct Dot3Wsi *const wsi)
{
  Log(kDot3LogLevel_event, "Parsing WSA Service Info WsaCountThresholdInterval extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Service Info WsaCountThresholdInterval extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  wsi->wsa_cnt_threshold_interval = *(uint8_t *)ext;
  wsi->extensions.wsa_cnt_threshold_interval = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info WsaCountThresholdInterval extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 의 확장필드를 파싱하여 저장한다.
 *
 * @param service_info  파싱할 WSA Service info asn.1 정보구조체
 * @param wsi           파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsiExtensions(
  const struct ServiceInfo *const service_info,
  struct Dot3Wsi *const wsi)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Service Info extensions\n");

  const struct ServiceInfoExts *const exts = &(service_info->chOptions.extensions);
  const struct ServiceInfoExt *ext;

  for (int j = 0; j < exts->count; j++)
  {
    ext = exts->tab + j;
    if (NULL == ext) {
      Err("Fail to parse WSA Service Info extensions - null extensions\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }

    switch (ext->extensionId)
    {
      case kDot3ExtensionId_Psc:
        ret = dot3_FFAsn1c_ParseWsiPscExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      case kDot3ExtensionId_IPv6Address: {
        ret = dot3_FFAsn1c_ParseWsiIPv6AddressExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_ServicePort: {
        ret = dot3_FFAsn1c_ParseWsiServicePortExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_ProviderMacAddress: {
        ret = dot3_FFAsn1c_ParseWsiProviderMacAddressExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_RcpiThreshold: {
        ret = dot3_FFAsn1c_ParseWsiRcpiThresholdExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_WsaCountThreshold: {
        ret = dot3_FFAsn1c_ParseWsiWsaCountThresholdExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      case kDot3ExtensionId_WsaCountThresholdInterval: {
        ret = dot3_FFAsn1c_ParseWsiWsaCountThresholdIntervalExtension(ext->value.u.data, wsi);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      default: {
        Err("Fail to parse WSA Service Info extension - invalid extensionId %d\n", ext->extensionId);
        return -kDot3Result_Fail_InvalidWsiExtensionIdValue;
      }
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info extensions\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info를 파싱하여 저장한다.
 *
 * @param service_info  파싱할 WSA Service info asn.1 정보구조체
 * @param wsi           파싱된 정보가 저장될 WSA Service Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsi(const struct ServiceInfo *const service_info, struct Dot3Wsi *const wsi)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Service Info\n");

  /*
   * 필수필드 파싱
   */
  ret = dot3_FFAsn1c_ParseWsiMandatory(service_info, wsi);
  if (ret < 0) {
    return ret;
  }

  /*
   * (존재하는 경우) 확장필드 파싱
   */
  if (service_info->chOptions.extensions_option) {
    ret = dot3_FFAsn1c_ParseWsiExtensions(service_info, wsi);
    if (ret < 0) {
      return ret;
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Service Info\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Service Info 들을 파싱하여 저장한다.
 *
 * @param wsa_msg   디코딩된 WSA asn.1 정보구조체
 * @param params    @ref Dot3_ParseWsa
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWsis(const struct SrvAdvMsg *const wsa_msg, struct Dot3ParseWsaParams *const params)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Service Infos\n");

  /*
   * WSA 내에 WSA Service Info 가 하나도 없으면 그대로 종료한다.
   */
  if (FALSE == wsa_msg->body.serviceInfos_option) {
    Log(kDot3LogLevel_event, "Success to parse WSA Service Infos - no WSA Service Info\n");
    return kDot3Result_Success;
  }

  const struct ServiceInfos *const service_infos = &(wsa_msg->body.serviceInfos);

  /*
   * WSA 내에 수납되어 있는 WSA Service info 개수를 확인한다.
   *  - WSA 내에 수납되어 있는 WSA Service Info의 수가 시스템이 지원하는 수보다 클 경우, 시스템이 지원하는 수까지만 파싱한다.
   */
  params->wsi_num = service_infos->count;
  if (params->wsi_num > kDot3WsiNum_MaxNum) {
    params->wsi_num = kDot3WsiNum_MaxNum;
    Log(kDot3LogLevel_event, "WAVE Service Info count is adjusted from %u to %u\n",
        service_infos->count, params->wsi_num);
  } else {
    Log(kDot3LogLevel_event, "%d service info exists\n", params->wsi_num);
  }


  /*
   * 각 WSA Service Info 를 파싱하여 저장한다.
   */
  struct ServiceInfo *service_info;
  for (int i = 0; i < params->wsi_num; i++)
  {
    service_info = service_infos->tab + i;
    if (!service_info) {
      Err("Fail to parse WSA Service Infos - null service info\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }

    ret = dot3_FFAsn1c_ParseWsi(service_info, &(params->wsis[i]));
    if (ret < 0) {
      return ret;
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Service Infos - %u WSA Service Info are parsed\n", params->wsi_num);
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info 의 필수필드를 파싱하여 저장한다.
 *
 * @param chan_info     파싱할 WSA Channel info asn.1 정보구조체
 * @param wsi           파싱된 정보가 저장될 WSA Channel Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWciMandatory(
  const struct ChannelInfo *const chan_info,
  struct Dot3Wci *const wci)
{
  Log(kDot3LogLevel_event, "Parsing WSA Channel Info mandatory fields\n");

  wci->operating_class = chan_info->operatingClass;
  wci->chan_num = chan_info->channelNumber;
  wci->transmit_power_level = chan_info->powerLevel;
  if (NULL == chan_info->dataRate.adaptable.buf) {
    Err("Fail to parse WSA Channel Info datarate adpatable - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  wci->adaptable_datarate = *(chan_info->dataRate.adaptable.buf);
  wci->datarate = chan_info->dataRate.dataRate;

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Info mandatory fields\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info 의 EdcaParameterSet 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 EdcaParameterSet extension 정보구조체
 * @param wci   파싱된 정보가 저장될 WSA Channel Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWciEdcaParameterSetExtension(const void *const ext, struct Dot3Wci *const wci)
{
  Log(kDot3LogLevel_event, "Parsing WSA Channel Info EdcaParameterSet extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Channel Info EdcaParameterSet extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }

  // TODO::

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Info EdcaParameterSet extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info 의 ChannelAccess 확장필드를 파싱하여 저장한다.
 *
 * @param ext   파싱할 ChannelAccess extension 정보구조체
 * @param wci   파싱된 정보가 저장될 WSA Channel Info 구조체 포인터
 * @return      성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWciChannelAccessExtension(const void *const ext, struct Dot3Wci *const wci)
{
  Log(kDot3LogLevel_event, "Parsing WSA Channel Info ChannelAccess extension\n");
  if (NULL == ext) {
    Err("Fail to parse WSA Channel Info ChannelAccess extension - null extension\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  wci->chan_access = *(uint8_t *)ext;
  wci->extension.chan_access = true;

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Info ChannelAccess extension\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info 의 확장필드를 파싱하여 저장한다.
 *
 * @param chan_info     파싱할 WSA Channel info asn.1 정보구조체
 * @param wci           파싱된 정보가 저장될 WSA Channel Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWciExtensions(
  const struct ChannelInfo *const chan_info,
  struct Dot3Wci *const wci)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Channel Info extensions\n");

  const struct ChannelInfoExts *const exts = &(chan_info->extensions.extensions);
  const struct ChannelInfoExt *ext;

  for (int j = 0; j < exts->count; j++)
  {
    ext = exts->tab + j;
    if (NULL == ext) {
      Err("Fail to parse Channel Info extensions - null extension\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }

    switch (ext->extensionId)
    {
      case kDot3ExtensionId_EdcaParameterSet:
        ret = dot3_FFAsn1c_ParseWciEdcaParameterSetExtension(ext->value.u.data, wci);
        if (ret < 0) {
          return ret;
        }
        break;
      case kDot3ExtensionId_ChannelAccess: {
        ret = dot3_FFAsn1c_ParseWciChannelAccessExtension(ext->value.u.data, wci);
        if (ret < 0) {
          return ret;
        }
        break;
      }
      default: {
        Err("Fail to parse WSA Channel Info extension - invalid extensionId %d\n", ext->extensionId);
        return -kDot3Result_Fail_InvalidWciExtensionIdValue;
      }
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Info extensions\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info를 파싱하여 저장한다.
 *
 * @param chan_info     파싱할 WSA Channel info asn.1 정보구조체
 * @param wci           파싱된 정보가 저장될 WSA Channel Info 구조체 포인터
 * @return              성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWci(const struct ChannelInfo *const chan_info, struct Dot3Wci *const wci)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Channel Info\n");

  /*
   * 필수필드 파싱
   */
  ret = dot3_FFAsn1c_ParseWciMandatory(chan_info, wci);
  if (ret < 0) {
    return ret;
  }

  // (존재하는 경우) 확장필드 파싱
  if (chan_info->extensions.extensions_option) {
    ret = dot3_FFAsn1c_ParseWciExtensions(chan_info, wci);
    if (ret < 0) {
      return ret;
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Info\n");
  return kDot3Result_Success;
}


/**
 * WSA asn.1 정보구조체로부터 WSA Channel Info 들을 파싱하여 저장한다.
 *
 * @param wsa_msg   WSA asn.1 정보구조체
 * @param params    @ref Dot3_ParseWsa
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWcis(const struct SrvAdvMsg *const wsa_msg, struct Dot3ParseWsaParams *const params)
{
  int ret;
  Log(kDot3LogLevel_event, "Parsing WSA Channel Infos\n");

  /*
   * WSA 내에 WSA Channel Info 가 하나도 없으면 그대로 종료한다.
   */
  if (FALSE == wsa_msg->body.channelInfos_option) {
    Log(kDot3LogLevel_event, "There is no WSA Channel Info\n");
    return kDot3Result_Success;
  }

  const struct ChannelInfos *const chan_infos = &(wsa_msg->body.channelInfos);

  /*
   * WSA 내에 수납되어 있는 개수를 확인한다.
   *  - WSA 내에 수납되어 있는 WSA Channel Info의 수가 시스템이 지원하는 수보다 클 경우, 시스템이 지원하는 수까지만 파싱한다.
   */
  params->wci_num = chan_infos->count;
  if (params->wci_num > kDot3WciNum_MaxNum) {
    params->wci_num = kDot3WciNum_MaxNum;
    Log(kDot3LogLevel_event, "Wci number is adjusted from %u to %u\n", chan_infos->count, params->wci_num);
  } else {
    Log(kDot3LogLevel_event, "%d channel info exists\n", params->wsi_num);
  }

  /*
   * 각 WSA Channel Info 를 파싱하여 저장한다.
   */
  struct ChannelInfo *chan_info;
  for (int i = 0; i < params->wci_num; i++)
  {
    chan_info = chan_infos->tab + i;
    if (NULL == chan_info) {
      Err("Fail to parse WSA Channel Info - null extensions\n");
      return -kDot3Result_Fail_Asn1AbnormalOp;
    }

    ret = dot3_FFAsn1c_ParseWci(chan_info, &(params->wcis[i]));
    if (ret < 0) {
      return ret;
    }
  }

  Log(kDot3LogLevel_event, "Success to parse WSA Channel Infos - %u WSA Channel Info are parsed\n", params->wci_num);
  return kDot3Result_Success;
}

/**
 * WSA asn.1 정보구조체로부터 WRA 을 파싱하여 저장한다.
 *
 * @param wsa_msg   WSA asn.1 정보구조체
 * @param params    @ref Dot3_ParseWsa
 * @return          성공시 0, 실패시 음수(-Dot3ResultCode)
 */
static int dot3_FFAsn1c_ParseWra(const struct SrvAdvMsg *const wsa_msg, struct Dot3ParseWsaParams *const params)
{
  Log(kDot3LogLevel_event, "Parsing WRA\n");

  /*
   * WSA 내에 WRA 가 없으면 그대로 종료한다.
   */
  if (FALSE == wsa_msg->body.routingAdvertisement_option) {
    Log(kDot3LogLevel_event, "There is no WRA\n");
    return kDot3Result_Success;
  }

  const struct RoutingAdvertisement *const routing_adv = &(wsa_msg->body.routingAdvertisement);

  /*
   * WRA 를 파싱하여 저장한다.
   */
  params->present.wra = true;
  // router lifetime
  params->wra.router_lifetime = routing_adv->lifetime;
  // IPv6 prefix
  if (routing_adv->ipPrefix.len != IPV6_ALEN) {
    Err("Fail to parse WRA - invalid size of ip prefix %d\n", routing_adv->ipPrefix.len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == routing_adv->ipPrefix.buf) {
    Err("Fail to parse WRA - null ip prefix\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  memcpy(params->wra.ip_prefix, routing_adv->ipPrefix.buf, IPV6_ALEN);
  // IPv6 prefix length
  params->wra.ip_prefix_len = routing_adv->ipPrefixLength;
  if (false == dot3_IsValidIPv6PrefixLenValue(params->wra.ip_prefix_len)) {
    Err("Fail to parse WRA - invalid ip prefix len %d\n", params->wra.ip_prefix_len);
    return -kDot3Result_Fail_InvalidIPv6PrefixLenValue;
  }
  // Default gateway
  if (routing_adv->defaultGateway.len != IPV6_ALEN) {
    Err("Fail to parse WRA - invalid size of default gateway %d\n", routing_adv->defaultGateway.len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == routing_adv->defaultGateway.buf) {
    Err("Fail to parse WRA - null default gateway\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  memcpy(params->wra.default_gw, routing_adv->defaultGateway.buf, IPV6_ALEN);
  // Primary DNS
  if (routing_adv->primaryDns.len != IPV6_ALEN) {
    Err("Fail to parse WRA - invalid size of primary dns %d\n", routing_adv->primaryDns.len);
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  if (NULL == routing_adv->primaryDns.buf) {
    Err("Fail to parse WRA - null primary dns\n");
    return -kDot3Result_Fail_Asn1AbnormalOp;
  }
  memcpy(params->wra.primary_dns, routing_adv->primaryDns.buf, IPV6_ALEN);

  // TODO:: 확장필드 파싱 구현

  Log(kDot3LogLevel_event, "Success to parse WRA \n");
  return kDot3Result_Success;
}


/**
 * @copydoc Dot3_ParseWsa
 */
int INTERNAL dot3_FFAsn1c_DecodeWsa(
  const uint8_t *const encoded_wsa,
  const Dot3PduSize encoded_wsa_size,
  struct Dot3ParseWsaParams *const params)
{
  int ret;
  struct SrvAdvMsg *wsa_msg = NULL;
  Log(kDot3LogLevel_event, "Decoding %d bytes WSA\n", encoded_wsa_size);

  /*
   * WSA를 UPER 디코딩한다.
   */
  ASN1Error err;
  asn1_ssize_t decoded_size = asn1_uper_decode((void **)&wsa_msg,
                                               asn1_type_SrvAdvMsg,
                                               encoded_wsa,
                                               encoded_wsa_size,
                                               &err);
  if ((decoded_size < 0) || (decoded_size > encoded_wsa_size) || (!wsa_msg)) {
    Err("Fail to decode WSM - fail to asn1_uper_decode() - decoded_size %d, wsa_msg %p\n", decoded_size, wsa_msg);
    return -kDot3Result_Fail_Asn1Decode;
  }

  /*
   * 헤더 asn.1 정보를 파싱하여 반환정보에 저장한다.
   */
  ret = dot3_FFAsn1c_ParseWsaHdr(wsa_msg, &(params->hdr));
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * WSA Service Infos asn.1 정보를 파싱하여 반환정보에 저장한다.
   */
  ret = dot3_FFAsn1c_ParseWsis(wsa_msg, params);
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * WSA Channel Infos asn.1 정보를 파싱하여 반환정보에 저장한다.
   */
  ret = dot3_FFAsn1c_ParseWcis(wsa_msg, params);
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * WRA 필드의 정보를 파싱하여 반환정보에 저장한다.
   */
  ret = dot3_FFAsn1c_ParseWra(wsa_msg, params);
  if (ret < 0) {
    asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);
    return ret;
  }

  /*
   * asn.1 정보구조체 해제
   */
  asn1_free_value(asn1_type_SrvAdvMsg, wsa_msg);

  Log(kDot3LogLevel_event, "Success to decode WSA\n");
  return kDot3Result_Success;
}
