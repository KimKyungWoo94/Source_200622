//
// Created by gyun on 2019-08-03.
//

#include <arpa/inet.h>
#include <string.h>

#include "dot3-internal.h"


/// WSMP Ethertype
#define ETHERTYPE_WSMP 0x88DC

/*
 * Frame Control 필드 설정/확인 매크로
 */
#define DOT11_GET_FC_PVER(fc) (fc & 3)
#define DOT11_GET_FC_FTYPE(fc) ((fc >> 2) & 3)
#define DOT11_GET_FC_FSTYPE(fc) ((fc >> 4) & 0xf)
#define DOT11_GET_FC_TODS(fc) ((fc >> 8) & 1)
#define DOT11_GET_FC_FROMDS(fc) ((fc >> 9) & 1)
#define DOT11_GET_FC_MOREFRAG(fc) ((fc >> 10) & 1)
#define DOT11_GET_FC_RETRY(fc) ((fc >> 11) & 1)
#define DOT11_GET_FC_PWRMGT(fc) ((fc >> 12) & 1)
#define DOT11_GET_FC_MOREDATA(fc) ((fc >> 13) & 1)
#define DOT11_GET_FC_ISWEP(fc) ((fc >> 14) & 1)
#define DOT11_GET_FC_ORDER(fc) ((fc >> 15) & 1)
#define DOT11_SET_FC_PVER(n) (uint16_t)(n & 3)
#define DOT11_SET_FC_FTYPE(n) (uint16_t)((n & 3) << 2)
#define DOT11_SET_FC_FSTYPE(n) (uint16_t)((n & 0xf) << 4)
#define DOT11_SET_FC_TODS(n) (uint16_t)((n & 1) << 8)
#define DOT11_SET_FC_FROMDS(n) (uint16_t)((n & 1) << 9)
#define DOT11_SET_FC_MOREFRAG(n) (uint16_t)((n & 1) << 10)
#define DOT11_SET_FC_RETRY(n) (uint16_t)((n & 1) << 11)
#define DOT11_SET_FC_PWRMGT(n) (uint16_t)((n & 1) << 12)
#define DOT11_SET_FC_MOREDATA(n) (uint16_t)((n & 1) << 13)
#define DOT11_SET_FC_ISWEP(n) (uint16_t)((n & 1) << 14)
#define DOT11_SET_FC_ORDER(n) (uint16_t)((n & 1) << 15)

/*
 * Sequence Control 필드 설정/확인 매크로
 */
#define	DOT11_GET_SC_SEQ(sc) ((sc >> 4) & 0xfff)
#define	DOT11_GET_SC_FRAG(sc) (sc & 0xf)
#define DOT11_SET_SC_SEQ(n) (uint16_t)((n & 0xfff) << 4)
#define DOT11_SET_SC_FRAG(n) (uint16_t)(n & 0xf)

/*
 * QoS Control 필드 설정/확인 매크로
 */
#define	DOT11_GET_QC_TID(qc) (qc & 0xf)
#define	DOT11_GET_QC_EOSP(qc) ((qc >> 4) & 1)
#define	DOT11_GET_QC_ACK_POLICY(qc) ((qc >> 5) & 3)
#define	DOT11_GET_QC_AMSDU_PRESENT(qc)	((qc >> 7) & 1)
#define	DOT11_GET_QC_TXOP_DUR_REQ(qc) ((qc >> 8) & 0xff)
#define	DOT11_GET_QC_UP(qc) DOT11_GET_QC_TID(qc)
#define	DOT11_SET_QC_TID(n) (uint16_t)(n & 0xf)
#define	DOT11_SET_QC_EOSP(n) (uint16_t)((n & 1) << 4)
#define	DOT11_SET_QC_ACK_POLICY(n) (uint16_t)((n & 3) << 5)
#define	DOT11_SET_QC_AMSDU_PRESENT(n) (uint16_t)((n & 1) << 7)
#define	DOT11_SET_QC_TXOP_DUR_REQ(n) (uint16_t)((n & 0xff) << 8)
#define	DOT11_SET_QC_UP(n) DOT11_SET_QC_TID(n)

/*
 * 개별/그룹 MAC주소 확인 매크로
 */
#define DOT11_GET_MAC_ADDR_IG(addr) (addr[0]&1)
#define DOT11_MAC_ADDR_IG_INDIVIDUAL 0
#define DOT11_MAC_ADDR_IG_GROUP 1


/**
 * @brief 802.11 MAC 헤더 형식
 */
struct Dot11MacHdr
{
  uint16_t fc;  ///< Frame control
  uint16_t dur; ///< Duration/AID
  uint8_t addr1[kDot3MacAddrSize];  ///< ADDR1
  uint8_t addr2[kDot3MacAddrSize];  ///< ADDR2
  uint8_t addr3[kDot3MacAddrSize];  ///< ADDR3
  uint16_t sc;  ///< Sequence control
  uint16_t qc;  ///< QoS control : QoS Data 프레임에만 존재한다.
} __attribute__ ((packed));

/**
 * @brief LLC 헤더 형식
 */
struct LLCHdr
{
  uint16_t type;  ///< EtherType
} __attribute__ ((packed));

/**
 * @brief 802.11 Frame control type
 */
enum eDot11FcType
{
  kDot11FcType_mgmt = 0,
  kDot11FcType_ctrl = 1,
  kDot11FcType_data = 2,
};
/// @copydoc eDot11FcType
typedef uint16_t Dot11FcType;

/**
 * @brief 802.11 Frame control subtype
 */
enum eDot11FcSubType
{
  kDot11FcSubType_data = 0x0,
  kDot11FcSubType_ta = 0x6,
  kDot11FcSubType_qos_data = 0x8,
  kDot11FcSubType_rts = 0xb,
  kDot11FcSubType_cts = 0xc,
  kDot11FcSubType_ack = 0xd
};
/// @copydoc eDot11FcSubType
typedef uint16_t Dot11FcSubType;

/**
 * @brief 802.11 Service class
 *
 * 멀티캐스트인 경우 NoAck, 유니캐스트인 경우 Ack으로 설정된다.
 */
enum eDot11ServiceClass
{
  kDot11ServiceClass_QosAck,
  kDot11ServiceClass_QosNoAck,
};
/// @copydoc eDot11ServiceClass
typedef uint16_t Dot11ServiceClass;

/// wildcard BSSID (all 1)
static const uint8_t wildcard_bssid[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/// 802.11 프로토콜 버전
static const uint8_t kDot11ProtocolHdr_ProtocolVersion = 0;

/**
 * @brief MPDU를 구성한다.
 * @param params params 전송 파라미터
 * @param outbuf outbuf MSDU가 저장된 버퍼의 주소 ("MAC 헤더 + LLC 헤더" 길이 오프셋 지점에 MSDU가 저장되어 있다)
 */
void dot3_ConstructMpdu(struct Dot3WsmMpduTxParams *const params, uint8_t *const outbuf)
{
  Log(kDot3LogLevel_event, "Constructing MPDU\n");

  /*
   * MAC 헤더 설정
   */
  struct Dot11MacHdr *mac_hdr = (struct Dot11MacHdr *)outbuf;
  mac_hdr->fc = DOT11_SET_FC_FTYPE(kDot11FcType_data) | DOT11_SET_FC_FSTYPE(kDot11FcSubType_qos_data);
  mac_hdr->dur = 0;
  memcpy(mac_hdr->addr1, params->dst_mac_addr, kDot3MacAddrSize);
  memcpy(mac_hdr->addr2, params->src_mac_addr, kDot3MacAddrSize);
  memset(mac_hdr->addr3, 0xff, kDot3MacAddrSize);
  mac_hdr->sc = 0xFFFE; // for LLC library
  mac_hdr->qc = DOT11_SET_QC_UP(params->priority);
  if (DOT11_GET_MAC_ADDR_IG(params->dst_mac_addr) == DOT11_MAC_ADDR_IG_INDIVIDUAL) {
    mac_hdr->qc |= DOT11_SET_QC_ACK_POLICY(kDot11ServiceClass_QosAck);
  } else {
    mac_hdr->qc |= DOT11_SET_QC_ACK_POLICY(kDot11ServiceClass_QosNoAck);
  }
  // TODO:: 빅엔디안 시스템에서 fc, sc, qc 스왑 기능 추가

  /*
   * LLC 헤더 설정
   */
  struct LLCHdr *llc_hdr = (struct LLCHdr *)(outbuf + sizeof(struct Dot11MacHdr));
  llc_hdr->type = htons(ETHERTYPE_WSMP);
}

/**
 * @brief MPDU 를 파싱한다.
 * @param mpdu      파싱할 MPDU 가 저장된 버퍼의 주소를 전달한다.
 * @param mpdu_size 파싱할 MPDU 의 길이를 전달한다.
 * @param params    수신파라미터정보가 저장될 정보구조체의 주소를 전달한다.
 * @return          성공 시 하위계층(MAC+LLC) 헤더의 크기, 실패 시 음수(-Dot3ResultCode)
 *
 * MPDU 의 MAC 헤더와 LLC 헤더를 파싱하면서 각 필드의 유효성도 확인한다. 유효하지 않을 경우 실패를 반환한다.
 */
int dot3_ParseMpdu(const uint8_t *const mpdu, const Dot3PduSize mpdu_size, struct Dot3WsmMpduRxParams *const params)
{
  Log(kDot3LogLevel_event, "Parsing MPDU\n");

  /*
   * MAC 헤더 파싱
   *  - 각 필드 유효성 확인 및 반환정보 저장
   *  - TODO:: 필드 유효성 확인을 이미 하위계층에서 수행했을 경우(예: 칩 드라이버), 생략할 수 있다.
   */
  struct Dot11MacHdr *mac_hdr = (struct Dot11MacHdr *)mpdu;
  uint16_t fc = mac_hdr->fc;
  uint8_t ver = DOT11_GET_FC_PVER(fc);
  if (ver != kDot11ProtocolHdr_ProtocolVersion) {
    Err("Fail to parse MPDU - invalid 802.11 protocol version %u\n", ver);
    return -kDot3Result_Fail_InvalidLowerLayerProtocolVersion;
  }
  uint8_t ftype = DOT11_GET_FC_FTYPE(fc);
  uint8_t fstype = DOT11_GET_FC_FSTYPE(fc);
  if (ftype != kDot11FcType_data) {
    Err("Fail to parse MPDU - invalid ftype %u\n", ftype);
    return -kDot3Result_Fail_InvalidLowerLayerFrameType;
  }
  if (fstype != kDot11FcSubType_qos_data) {
    Err("Fail to parse MPDU - invalid fstype %u\n", fstype);
    return -kDot3Result_Fail_InvalidLowerLayerFrameType;
  }
  memcpy(params->dst_mac_addr, mac_hdr->addr1, kDot3MacAddrSize);
  memcpy(params->src_mac_addr, mac_hdr->addr2, kDot3MacAddrSize);
  if (memcmp(mac_hdr->addr3, wildcard_bssid, kDot3MacAddrSize)) {
    Err("Fail to parse MPDU - addr3 is not wildcard bssid. it's %02X:%02X:%02X:%02X:%02X:%02X\n",
        mac_hdr->addr3[0], mac_hdr->addr3[1], mac_hdr->addr3[2],
        mac_hdr->addr3[3], mac_hdr->addr3[4], mac_hdr->addr3[5]);
    return -kDot3Result_Fail_NotWildcardBssid;
  }
  params->priority = DOT11_GET_QC_UP(mac_hdr->qc);

  /*
   * LLC 헤더 파싱
   */
  struct LLCHdr *llc_hdr = (struct LLCHdr *)(mpdu + sizeof(struct Dot11MacHdr));
  uint16_t ether_type = ntohs(llc_hdr->type);
  if (ether_type != ETHERTYPE_WSMP) {
    Err("Fail to parse MDPU - not supported ether type 0x%04X\n", ether_type);
    return -kDot3Result_Fail_NotSupportedEtherType;
  }

  /*
   * MAC + LLC 헤더 길이 반환
   */
  int ret = kQoSMacHdrSize + kLLCHdrSize;
  Log(kDot3LogLevel_event, "Success to parse MPDU - MAC+LLC header size is %u\n", ret);
  return ret;
}
