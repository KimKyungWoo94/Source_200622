//
// Created by gyun on 2019-07-11.
//

#include "dot3-internal.h"

/**
 * @brief -128~127 사이의 파워값을 UPER 인코딩을 위해 0~255 값으로 변환한다.
 * @param signed_power 변환할 파워값
 * @return 변환된 파워값
 *
 * IEEE 1609와 ISO ITS 표준과의 조화작업을 통해 양수값만을 사용하도록 제안되었으며(per CVCOC AppNotes)
 * UPER 인코딩 시 양수값만을 사용하는 ISO의 룰에 따르기로 함.
 */
uint8_t dot3_ConvertPowerForEncoding(Dot3Power signed_power)
{
  return (uint8_t)(signed_power + 128);
}
