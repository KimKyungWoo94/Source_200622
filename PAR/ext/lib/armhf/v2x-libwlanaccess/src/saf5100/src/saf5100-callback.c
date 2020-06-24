/**
 * @file saf5100-callback.c
 * @date 2019-05-10
 * @author gyun
 * @brief saf5100 LLC 드라이버로부터 호출되는 콜백함수들을 구현한 파일
 */

#include <inttypes.h>

#include "wlanaccess-80211.h"
#include "wlanaccess-internal.h"

#include "llc-api.h"
#include "pktbuf-app.h"

#include "saf5100.h"


/**
 * @brief SAF5100 플랫폼 TxCnf() 콜백함수 구현부.
 * @param pMKx MKx 핸들
 * @param pTxPkt TxReq()에서 송신했던 패킷정보
 * @param pTxEvent 송신이벤트 정보
 * @param pPriv TxReq() 송신한 패킷버퍼 = 함수의 pPriv 인자로 전달된 데이터 포인터 (=struct PktBuf)
 * @return tMKxStatus
 *
 * TxReq()에 대한 응답인 TxCnf() 콜백함수이며, 이벤트폴링 함수인 MKx_Recv() 에서 호출된다.
 */
tMKxStatus INTERNAL al_SAF5100_TxCnf(struct MKx *pMKx, tMKxTxPacket *pTxPkt, const tMKxTxEvent *pTxEvent, void *pPriv)
{
  /*
   * 파라미터 체크
   */
  if ((pMKx == NULL) || (pTxPkt == NULL) || (pTxEvent == NULL) || (pPriv == NULL)) {
    Err("Null pointer from LLC device\n");
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }

  PktBuf_Free((struct PktBuf *)pPriv);

  struct MKxTxPacketData *pkt_data = &(pTxPkt->TxPacketData);
  const struct MKxTxEventData *event_data = &(pTxEvent->TxEventData);
  Log(kAlLogLevel_event, "Confirm tx - ifindex: %u, timeslot: %u\n", pkt_data->RadioID, pkt_data->ChannelID);
  Log(kAlLogLevel_all, "  eMKxStatus: %d, MACSequenceNumber: %u, TxTime: %" PRIu64
    ", AckResponseDelay: %u\n"
    "                            CTSResponseDelay: %u, MACDwellTime: %u, NumShortRetries: %u, NumLongRetries: %u\n"
    "                            Dest - %02X:%02X:%02X:%02X:%02X:%02X\n",
      event_data->TxStatus, event_data->MACSequenceNumber, event_data->TxTime,
      event_data->AckResponseDelay, event_data->CTSResponseDelay, event_data->MACDwellTime,
      event_data->NumShortRetries, event_data->NumLongRetries,
      event_data->DestAddress[0], event_data->DestAddress[1], event_data->DestAddress[2],
      event_data->DestAddress[3], event_data->DestAddress[4], event_data->DestAddress[50]);


  /*
   * 어플리케이션 콜백함수를 호출한다.
   */
  struct AlPlatform *platform = g_al_saf5100_platform.parent;
  if (platform->ProcessTransmitResultCallback) {
    if (event_data->TxStatus == MKXSTATUS_SUCCESS) {
      platform->ProcessTransmitResultCallback(kAlTxResult_Success, 0);
    } else {
      platform->ProcessTransmitResultCallback(-kAlTxResult_DevSpecificErr, 0);
    }
  } else {
    Log(kAlLogLevel_event, "  No transmit result callback function\n");
  }

  return MKXSTATUS_SUCCESS;
}


/**
 * @brief SAF5100 플랫폼 RxAlloc() 콜백함수 구현부
 * @param pMKx MKx 핸들
 * @param BufLen 수신패킷정보를 저장하기 위해 필요한 버퍼길이 (sizeof(struct MKxRxPacket) + 수신된 MPDU 길이(CRC 포함))
 * @param ppBuf 본 콜백함수의 수행 후 호출될 RxInd() 콜백함수의 pRxPkt 인자로 전달될 수신패킷정보의 주소를 저장하여 반환한다.
 * @param ppPriv 본 콜백함수의 수행 후 호출될 RxInd() 콜백함수의 pPriv 인자로 전달될 패킷버퍼의 주소를 저장하여 반환한다.
 * @return tMKxStatus
 *
 * LLC 디바이스는 MPDU를 수신하면, 본 콜백함수를 호출하여 수신패킷정보를 저장할 버퍼의 할당을 요청한다.
 * 이 때 BufLen 인자를 통해 할당되어야 할 버퍼의 길이를 알려주며,
 *   이 값은 수신패킷정보 전체의 길이(struct MKxRxPacket의 길이 + MPDU길이)를 나타낸다.
 *
 * 본 콜백함수 내에서는, 전달된 BufLen의 길이만큼 버퍼를 할당하여 ppBuf 주소를 통해 LLC 디바이스로 전달한다.
 * (여기서 할당된 버퍼는 다음에 호출될 RxInd() 콜백함수에서 해제하면 된다)
 * ppPriv 에는 RxInd() 콜백함수에서 수신패킷정보가 저장될 버퍼를 다루기 위해 필요한 추가정보가 포함된 정보를 같이 전달한다.
 * 구현 형태에 따라, ppPriv는 필요 없을 수 있다?
 */
tMKxStatus INTERNAL al_SAF5100_RxAlloc(struct MKx *pMKx, int BufLen, uint8_t **ppBuf, void **ppPriv)
{
  Log(kAlLogLevel_all, "Allocate buffer for rx MPDU - len: %d\n", BufLen);

  /*
   * 디바이스를 신뢰 못할 경우
   */
  if ((pMKx == NULL) || (ppBuf == NULL) || (ppPriv == NULL)) {
    Err("Null pointer from LLC device\n");
    goto error;
  }
  if ((BufLen <= 0) || (BufLen > kAlMpduMaxSize)) {
    Err("Invalid BufLen(%d) from LLC device\n", BufLen);
    goto error;
  }

  /*
   * 패킷 버퍼 할당 및 반환
   *  - TODO:: PktBuf를 자체 정의한 구조로 변경하자.
   */
  struct PktBuf *pktbuf = PktBuf_Alloc(LLC_DEV_HEADROOM + LLC_DEV_TAILROOM + BufLen);
  if (pktbuf == NULL) {
    Err("Fail to allocate packet buffer - no memory\n");
    goto error;
  }
  PktBuf_Reserve(pktbuf, LLC_DEV_HEADROOM);
  PktBuf_Put(pktbuf, BufLen);
  *ppPriv = pktbuf;
  *ppBuf = pktbuf->data;

  return MKXSTATUS_SUCCESS;

error:
  *ppBuf = NULL;
  *ppPriv = NULL;
  return MKXSTATUS_FAILURE_INTERNAL_ERROR;
}


/**
 * @brief RxInd() 콜백함수로 전달된 각 인자 및 수신패킷정보의 유효성을 확인한다.
 * @param pMKx MKx 핸들
 * @param pRxPkt 수신패킷정보
 * @param pPriv 수신패킷정보가 저장된 패킷버퍼 (= RxAlloc() 콜백함수의 pPriv 인자에 반환된 버퍼)
 * @return tMKxStatus
 */
static tMKxStatus al_SAF5100_IsValid_RxInd(struct MKx *pMKx, tMKxRxPacket *pRxPkt, void *pPriv)
{
  if ((pMKx == NULL) || (pRxPkt == NULL) || (pPriv == NULL)) {
    Err("Null pointer from LLC device\n");
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }
  if (pRxPkt->Hdr.Type != MKXIF_RXPACKET) {
    Err("Invalid rx packet header type(%d) from LLC device. must be %d\n", pRxPkt->Hdr.Type, MKXIF_RXPACKET);
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }

  struct MKxRxPacketData *rx_pkt_data = &pRxPkt->RxPacketData;
  if ((rx_pkt_data->RxFrameLength < kAlMpduMinSizeWithCrc) || (rx_pkt_data->RxFrameLength > kAlMpduMaxSizeWithCrc)) {
    Err("Invalid rx packet length(%u) from LLC device\n", rx_pkt_data->RxFrameLength);
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }
  if (rx_pkt_data->FCSPass == 0) {
    Err("Invalid rx packet from LLC device. FCS failed\n");
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }
  if ((rx_pkt_data->RadioID < 0) || (rx_pkt_data->RadioID > MKX_RADIO_MAX)) {
    Err("Invalid RadioID(%d) from LLC device\n", rx_pkt_data->RadioID);
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }
  if ((rx_pkt_data->ChannelID < 0) || (rx_pkt_data->ChannelID > MKX_CHANNEL_MAX)) {
    Err("Invalid ChannelID(%d) from LLC device\n", rx_pkt_data->ChannelID);
    return MKXSTATUS_FAILURE_INTERNAL_ERROR;
  }

  return MKXSTATUS_SUCCESS;
}


/**
 * @brief SAF5100 플랫폼 RxInd() 콜백함수 구현부
 * @param pMKx MKx 핸들
 * @param pRxPkt 수신패킷정보 (= RxAlloc() 콜백함수의 ppBuf 인자에 반환된 버퍼)
 * @param pPriv 수신패킷정보가 저장된 패킷버퍼 (= RxAlloc() 콜백함수의 pPriv 인자에 반환된 버퍼)
 * @return tMKxStatus
 */
tMKxStatus INTERNAL al_SAF5100_RxInd(struct MKx *pMKx, tMKxRxPacket *pRxPkt, void *pPriv)
{
  /*
   * 파라미터 유효성 체크
   */
  int ret = al_SAF5100_IsValid_RxInd(pMKx, pRxPkt, pPriv);
  if (ret != MKXSTATUS_SUCCESS) {
    if (pPriv) {
      PktBuf_Free((struct PktBuf *)pPriv);
    }
    return ret;
  }

  struct SAF5100Device *saf5100_dev = (struct SAF5100Device *)(pMKx->pPriv);
  struct MKxRxPacketData *rx_pkt_data = &pRxPkt->RxPacketData;

  // rx_pkt_data->RxFrameLength: 수신된 MPDU의 길이 (CRC 포함)
  Log(kAlLogLevel_event, "Receiving %u-bytes MPDU(including CRC)\n", rx_pkt_data->RxFrameLength);
  Log(kAlLogLevel_event, "  RadioID: %d, ChannelID: %d, MCS: 0x%01X, FCSPass: %u, RxPowerA: %d\n",
    rx_pkt_data->RadioID, rx_pkt_data->ChannelID, rx_pkt_data->MCS, rx_pkt_data->FCSPass, rx_pkt_data->RxPowerA);
  Log(kAlLogLevel_event, "  RxPowerB: %d, RxNoiseA: %d, RxNoiseB: %d, FreqOffset: %d, RxTSF: %"PRIu64"\n",
      rx_pkt_data->RxPowerB, rx_pkt_data->RxNoiseA, rx_pkt_data->RxNoiseB, rx_pkt_data->RxMeta.FreqOffset,
      rx_pkt_data->RxTSF);
  if (g_al_log >= kAlLogLevel_dump) {
    al_PrintPacketDump(rx_pkt_data->RxFrame, rx_pkt_data->RxFrameLength);
  }

  struct AlMpduRxParams rxparams;
  rxparams.ifindex = (saf5100_dev->dev_index * SAF5100_IF_NUM_IN_DEV) + rx_pkt_data->RadioID;
  rxparams.timeslot = rx_pkt_data->ChannelID;
  const struct MKxRadioConfigData *radio_cfg_data = al_SAF5100_GetCurrentRadioConfigData(pMKx, rxparams.ifindex);
  rxparams.channel = al_ConvertFreqToChannelNumber(radio_cfg_data->ChanConfig[rx_pkt_data->ChannelID].PHY.ChannelFreq);
  if (rx_pkt_data->RadioID == MKX_RADIO_A) {
    rxparams.rxpower = rx_pkt_data->RxPowerA;
  } else {
    rxparams.rxpower = rx_pkt_data->RxPowerB;
  }
  if (rxparams.rxpower != -32768) {
    rxparams.rxpower -= 8;  // VERA모듈 실측 결과, 실제보다 4dB 높게 나와서 빼 준다.
    rxparams.rcpi = al_ConvertRxPowerToRcpi(rxparams.rxpower);
  }
  if (radio_cfg_data->ChanConfig[rx_pkt_data->ChannelID].PHY.Bandwidth == MKXBW_10MHz) {
    rxparams.datarate = al_SAF5100_Convert10MhzMcsToDataRate(rx_pkt_data->MCS);
  } else {
    rxparams.datarate = al_SAF5100_Convert20MhzMcsToDataRate(rx_pkt_data->MCS);
  }
  Log(kAlLogLevel_event, "Receiving %u-bytes MPDU(including CRC)\n", rx_pkt_data->RxFrameLength);
  Log(kAlLogLevel_event, "  ifindex: %u, timeslot: %u, channel: %u, rxpower: %d, rcpi: %u, datarate: %u\n",
      rxparams.ifindex, rxparams.timeslot, rxparams.channel, rxparams.rxpower, rxparams.rcpi, rxparams.datarate);

  /*
   * 어플리케이션 콜백함수를 호출한다.
   */
  struct AlPlatform *platform = g_al_saf5100_platform.parent;
  if (platform->ProcessRxMpduCallback) {
    platform->ProcessRxMpduCallback(rx_pkt_data->RxFrame, rx_pkt_data->RxFrameLength, &rxparams);
  } else {
    Log(kAlLogLevel_event, "  No rx MPDU callback function\n");
  }

  PktBuf_Free((struct PktBuf *)pPriv);
  return MKXSTATUS_SUCCESS;
}

/**
 * @brief SAF5100 플랫폼 NotifInd() 콜백함수 구현부
 * @param pMKx MKx 핸들
 * @param Notif Notification 정보
 * @return tMKxStatus
 */
tMKxStatus INTERNAL al_SAF5100_NotifInd(struct MKx *pMKx, tMKxNotif Notif)
{
  /*
   * 파라미터 유효성 체크
   */
  if (pMKx == NULL) {
    return MKXSTATUS_FAILURE_INVALID_HANDLE;
  }

  Log(kAlLogLevel_event, "Notification: 0x%07X\n", Notif);

  struct SAF5100Device *saf5100_dev = (struct SAF5100Device *)(pMKx->pPriv);
  struct AlPlatform *platform = g_al_saf5100_platform.parent;

  if (Notif & MKX_NOTIF_MASK_RADIOA) {

    // 채널접속 또는 MAC주소설정 요청에 대한 Notification
    if (Notif == MKX_NOTIF_MASK_RADIOA) {
      if (saf5100_dev->req[MKX_RADIO_A] == kSAF5100Req_AccessChannel) {
        saf5100_dev->req[MKX_RADIO_A] = kSAF5100Req_None;
        if (platform->ProcessAccessChannelResultCallback) {
          platform->ProcessAccessChannelResultCallback(saf5100_dev->dev_index * SAF5100_IF_NUM_IN_DEV);
        }
      } else if (saf5100_dev->req[MKX_RADIO_A] == kSAF5100Req_SetIfMacAddress) {
        saf5100_dev->req[MKX_RADIO_A] = kSAF5100Req_None;
        if (platform->ProcessSetIfMacAddressResultCallback) {
          platform->ProcessSetIfMacAddressResultCallback(saf5100_dev->dev_index * SAF5100_IF_NUM_IN_DEV);
        }
      }
    }
  }
  else if (Notif & MKX_NOTIF_MASK_RADIOB) {

    // 채널접속 또는 MAC주소설정 요청에 대한 Notification
    if (Notif == MKX_NOTIF_MASK_RADIOB) {
      if (saf5100_dev->req[MKX_RADIO_B] == kSAF5100Req_AccessChannel) {
        saf5100_dev->req[MKX_RADIO_A] = kSAF5100Req_None;
        if (platform->ProcessAccessChannelResultCallback) {
          platform->ProcessAccessChannelResultCallback((saf5100_dev->dev_index * SAF5100_IF_NUM_IN_DEV) + 1);
        }
      } else if (saf5100_dev->req[MKX_RADIO_B] == kSAF5100Req_SetIfMacAddress) {
        saf5100_dev->req[MKX_RADIO_A] = kSAF5100Req_None;
        if (platform->ProcessSetIfMacAddressResultCallback) {
          platform->ProcessSetIfMacAddressResultCallback((saf5100_dev->dev_index * SAF5100_IF_NUM_IN_DEV) + 1);
        }
      }
    }
  }

  /*
   * 어플리케이션 콜백함수를 호출한다.
   */
  // TODO::

  return MKXSTATUS_SUCCESS;
}