//
// Created by gyun on 2019-05-08.
//

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "wlanaccess.h"

static uint8_t test_mpdu[] = {
  0x88, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF,
  0x20, 0x00, 0x88, 0xDC, 0x03, 0x00, 0x0A, 0x0A,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09
};

static void* PollEvent(void *none);
static void ProcessAccessChannelResultCallback(const AlIfIndex ifindex);
static void ProcessSetIfMacAddressResultCallback(const AlIfIndex ifindex);
static void ProcessTransmitResultCallback(const AlTxResultCode result, const int dev_specific_errcode);
static void ProcessRxMpduCallback(
  const uint8_t *const mpdu,
  const AlMpduSize mpdu_size,
  const struct AlMpduRxParams *const rxparams);

volatile bool g_channel_access_done = false;
volatile bool g_set_if_mac_addr_done = false;


/**
 * @brief 메인 함수
 * @param argc 사용하지 않음
 * @param argv 사용하지 않음
 * @return
 */
int main(int argc, char *argv[])
{
  int ret;
  printf("libwlanaccess test\n");

  /*
   * 라이브러리 초기화
   */
  int ifnum = Al_Init(kAlLogLevel_none);
  if (ifnum < 0) {
    printf("Fail to Al_Init() - ifnum: %d\n", ifnum);
    return -1;
  }
  printf("Success to Al_Init() - %d interface is supported\n", ifnum);

  /*
   * 콜백함수 등록
   *  - 채널접속결과 콜백
   *  - MAC주소설정결과 콜백
   *  - 전송결과 콜백
   *  - 수신 콜백
   */
  Al_RegisterCallbackAccessChannelResult(ProcessAccessChannelResultCallback);
  Al_RegisterCallbackSetIfMacAddressResult(ProcessSetIfMacAddressResultCallback);
  Al_RegisterCallbackTransmitResult(ProcessTransmitResultCallback);
  Al_RegisterCallbackRxMpdu(ProcessRxMpduCallback);

  /*
   * 이벤트 폴링 쓰레드 생성
   *  - 콜백함수 등록 후 이벤트 폴링을 시작한다.
   */
  pthread_t poll_thread;
  ret = pthread_create(&poll_thread, NULL, PollEvent, NULL);
  if (ret) {
    printf("Fail to create polling thread\n");
    return -1;
  }

  /*
   * 채널 접속
   */
  uint8_t chan;
  for (int i = 0; i < ifnum; i++) {
    chan = 172 + (i * 4); // 172, 176, ...
    printf("Access channel %u on interface %d\n", chan, i);
    g_channel_access_done = false;
    ret = Al_AccessChannel(i, chan, chan);
    if (ret < 0) {
      printf("Fail to Al_AccessChannel() - ret: %d\n", ret);
      return -1;
    }
    while(!g_channel_access_done);
  }

  /*
   * MAC 주소 설정
   */
  AlMacAddress addr = {0x00,0x01,0x02,0x03,0x04,0x00};
  for (int i = 0; i < ifnum; i++) {
    addr[5] = 0x00 + i;
    printf("Set interface MAC address %02X:%02X:%02X:%02X:%02X:%02X on interface %d\n",
           addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], i);
    g_set_if_mac_addr_done = false;
    ret = Al_SetIfMacAddress(i, addr);
    if (ret < 0) {
      printf("Fail to Al_SetIfMacAddress() - ret: %d\n", ret);
      return -1;
    }
    while(!g_set_if_mac_addr_done);
  }

  /*
   * 접속채널 확인
   */
  uint8_t ts0_chan, ts1_chan;
  for (int i = 0; i < ifnum; i++) {
    ret = Al_GetCurrentChannel(i, &ts0_chan, &ts1_chan);
    if (ret < 0) {
      printf("Fail to Al_GetCurrentChannel() for on %u - %d\n", i, ret);
      return -1;
    }
    printf("Current channel on interface %d - ts0: %u, ts1: %u\n", i, ts0_chan, ts1_chan);
  }

  /*
   * 송신 테스트 (조합: 172/0, 176/1, 180/2, 184/3)
   */
//  struct AlMpduTxParams txparams;
//  txparams.channel = 172;
//  txparams.timeslot = 0;
//  txparams.datarate = 6;
//  txparams.expiry = 0;
//  txparams.txpower = 40;
//  ret = Al_TransmitMpdu(0, test_mpdu, sizeof(test_mpdu), &txparams);
//  if (ret < 0) {
//    return -1;
//  }

  /*
   * 폴링 쓰레드 종료 대기
   *  - 폴링 쓰레드 종료 전까지 콜백함수가 호출될 수 있다.
   */
  pthread_join(poll_thread, (void **)&ret);

  /*
   * 채널 접속 해제
   */
//  ret = Al_ReleaseChannel(0, kAlTimeSlot_both);
//  if (ret < 0) {
//    return ret;
//  }

  return 0;
}


/**
 * 이벤트 폴링 쓰레드
 *  - 이벤트 폴링 API를 지속적으로 호출해야 콜백함수가 호출될 수 있다.
 */
static void* PollEvent(void *none)
{
  int ret = Al_PollEvent();
  if (ret < 0) {
    printf("Fail to poll event - ret: %d\n", ret);
  }
  return NULL;
}


/**
 * @brief 채널접속결과 결과 수신 콜백 함수
 * @param ifindex 채널접속된 인터페이스 식별번호
 *
 * Al_RegisterCallbackAccessChannelResult() API를 통해 액세스계층 라이브러리에 등록된다.
 * Al_PollEvent() API 를 지속적으로 호출하는 경우에만, 본 콜백함수가 호출될 수 있다.
 */
static void ProcessAccessChannelResultCallback(const AlIfIndex ifindex)
{
  printf("Access channel result callback - ifindex: %u\n", ifindex);
  g_channel_access_done = true;
}


/**
 * @brief MAC주소설정결과 결과 수신 콜백 함수
 * @param ifindex MAC주소 설정된 인터페이스 식별번호
 *
 * Al_RegisterCallbackSetIfMacAddressResult() API를 통해 액세스계층 라이브러리에 등록된다.
 * Al_PollEvent() API 를 지속적으로 호출하는 경우에만, 본 콜백함수가 호출될 수 있다.
 */
static void ProcessSetIfMacAddressResultCallback(const AlIfIndex ifindex)
{
  printf("Set interface MAC address result callback - ifindex: %u\n", ifindex);
  g_set_if_mac_addr_done = true;
}


/**
 * @brief 송신 결과 수신 콜백 함수
 * @param result 송신 요청 처리 결과
 * @param dev_specific_errcode 에러 발생 시(result<0), 디바이스별로 정의된 상세 에러코드
 *
 * Al_RegisterCallbackTransmitResult() API를 통해 액세스계층 라이브러리에 등록된다.
 * Al_PollEvent() API 를 지속적으로 호출하는 경우에만, 본 콜백함수가 호출될 수 있다.
 */
static void ProcessTransmitResultCallback(const AlTxResultCode result, const int dev_specific_errcode)
{
  printf("Transmit result callback - result: %d, errcode: %d\n", result, dev_specific_errcode);
}


/**
 * @brief MPDU 수신 콜백함수
 * @param mpdu 수신된 MPDU (MAC 헤더 + MSDU + MAC CRC)
 * @param mpdu_size 수신 MPDU의 크기
 * @param rxparams 수신정보(채널, 파워 등)
 *
 * Al_RegisterCallbackRxMpdu() API를 통해 액세스계층 라이브러리에 등록된다.
 * Al_PollEvent() API 를 지속적으로 호출하는 경우에만, 본 콜백함수가 호출될 수 있다.
 */
static void ProcessRxMpduCallback(
  const uint8_t *const mpdu,
  const AlMpduSize mpdu_size,
  const struct AlMpduRxParams *const rxparams)
{
  printf("Rx MPDU callback - MPDU size: %u, ifindex: %u, timeslot: %u, channel: %u, "
         "rxpower: %d(0.5dBm), rcpi: %u, datarate: %u(500kbps)\n",
         mpdu_size, rxparams->ifindex, rxparams->timeslot, rxparams->channel,
         rxparams->rxpower, rxparams->rcpi, rxparams->datarate);
  for (int i = 0; i < mpdu_size; i++) {
    if ((i!=0) && (i%16==0)) {
      printf("\n");
    }
    printf("%02X ", mpdu[i]);
  }
  printf("\n");
}
