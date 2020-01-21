/**
 * @file dot3-ffasn1c.c
 * @date 2019-08-17
 * @author gyun
 * @brief ffasn1c 라이브러리 기반 공통 인코딩/디코딩 관련 기능 구현 파일
 */


#include "asn1defs_int.h"

#include "dot3-asn.h"
#include "dot3-internal.h"


/**
 * ASN.1 인코딩을 위해 VarLengthNumber(Psid) 정보구조체를 채운다.
 *
 * @param psid          인코딩할 psid 값
 * @param var_len_num   정보를 채울 정보구조체의 포인터를 전달한다.
 * @return              성공시 0(kDot3Result_Success), 실패시 음수(-Dot3ResultCode)
 */
int INTERNAL dot3_FFAsn1c_FillVarLengthNumber(Dot3Psid psid, VarLengthNumber *const var_len_num)
{
  Log(kDot3LogLevel_event, "Filling VarLengthNumber - psid: %u\n", psid);

  /*
   * Psid 값의 범위에 따라 p-encoded Psid의 길이를 결정한다.
   */
  uint32_t p_encoded_psid_len;
  if (psid <= 127) {
    p_encoded_psid_len = 1;
  } else if (psid <= 16511) {
    p_encoded_psid_len = 2;
  } else if (psid <= 2113663) {
    p_encoded_psid_len = 3;
  } else if (psid <= 270549119) {
    p_encoded_psid_len = 4;
  } else {
    Err("Fail to fill VarLengthNumber(Psid) - invalid Psid %d\n", psid);
    return -kDot3Result_Fail_InvalidPsidValue;
  }

  /*
   * p-encoded Psid 값의 길이에 맞게 VarLengthNumber 정보구조체를 채운다.
   */
  if (p_encoded_psid_len == 1) {
    var_len_num->choice = VarLengthNumber_content;
    var_len_num->u.content = (int)psid;
  }
  else {
    var_len_num->choice = VarLengthNumber_extension;
    if (p_encoded_psid_len == 2) {
      var_len_num->u.extension.choice = Ext1_content;
      var_len_num->u.extension.u.content = (int)psid;
    }
    else {
      var_len_num->u.extension.choice = Ext1_extension;
      if (p_encoded_psid_len == 3) {
        var_len_num->u.extension.u.extension.choice = Ext2_content;
        var_len_num->u.extension.u.extension.u.content = (int)psid;
      }
      else {
        var_len_num->u.extension.u.extension.choice = Ext2_extension;
        var_len_num->u.extension.u.extension.u.extension = (int)psid;
      }
    }
  }

  Log(kDot3LogLevel_event, "Success to fill VarLengthNumber\n");
  return kDot3Result_Success;
}


/**
 * 디코딩된 VarLengthNumber 정보를 파싱하여 PSID 를 반환한다.
 *
 * @param var_len_num   파싱할 VarLengthNumber 정보구조체의 주소를 전달한다.
 * @return              성공시 파싱된 PSID, 실패시 음수(-Dot3ResultCode)
 */
int INTERNAL dot3_FFAsn1c_ParseVarLengthNumber(const VarLengthNumber *const var_len_num)
{
  Log(kDot3LogLevel_event, "Parsing VarLengthNumber\n");

  int psid;
  if (var_len_num->choice == VarLengthNumber_content) {
    psid = var_len_num->u.content;
  } else if (var_len_num->choice == VarLengthNumber_extension) {
    const struct Ext1 *ext1 = &var_len_num->u.extension;
    if (ext1->choice == Ext1_content) {
      psid = ext1->u.content;
    } else if (ext1->choice == Ext1_extension) {
      const struct Ext2 *ext2 = &ext1->u.extension;
      if (ext2->choice == Ext2_content) {
        psid = ext2->u.content;
      } else if (ext2->choice == Ext2_extension) {
        psid = ext2->u.extension;
      } else {
        Err("Fail to parse VarLengthNumber - invalid ext2 choice %d\n", ext2->choice);
        return -kDot3Result_Fail_InvalidPsidFormat;
      }
    } else {
      Err("Fail to parse VarLengthNumber - invalid ext1 choice %d\n", ext1->choice);
      return -kDot3Result_Fail_InvalidPsidFormat;
    }
  } else {
    Err("Fail to parse VarLengthNumber - invalid var_len_num choice %d\n", var_len_num->choice);
    return -kDot3Result_Fail_InvalidPsidFormat;
  }

  Log(kDot3LogLevel_event, "Success to parse VarLengthNumber - psid: %d\n", psid);
  return psid;
}