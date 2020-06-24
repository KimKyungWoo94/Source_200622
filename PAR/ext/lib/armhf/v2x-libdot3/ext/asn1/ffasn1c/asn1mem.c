
/**
 * @file asn-base.h
 * @date 2019-080-92
 * @author gyun
 * @brief ffasn1c에서 사용되는 메모리 관련 함수를 정의한다.
 *
 * libffasn1은 메모리 할당/재할당/해제에 관련된 함수를 직접 구현하지 않고, 사용자 코드에서 구현하도록 지시하고 있다.
 * (asn1defs.h 파일에서 함수 원형만 정의하고 있다)
 *
 * @code
 * in asn1defs.h
 * // These 3 functions must be provided by the user. asn1_malloc()
 * //  should return a value different from NULL when size = 0.
 * void *asn1_malloc(size_t size);
 * void *asn1_realloc(void *ptr, size_t size);
 * void asn1_free(void *ptr);
 * @endcode
 */

#include <stdlib.h>

/**
 * @brief 메모리 공간을 할당한다.
 * @param size 할당할 크기
 * @return 할당된 메모리 공간 주소
 */
void *asn1_malloc(size_t size)
{
  return malloc(size);
}

/**
 * @brief 이미 할당된 메모리 공간을 size만큼의 크기로 재할당한다.
 * @param ptr 할당되어 있는 메모리 공간 주소
 * @param size 재할당할 크기
 * @return 재할당된 메모리 공간 주소
 */
void *asn1_realloc(void *ptr, size_t size)
{
  return realloc(ptr, size);
}

/**
 * @brief 할당된 메모리 공간을 해제한다.
 * @param ptr 해제할 메모리 공간 주소
 */
void asn1_free(void *ptr)
{
  free(ptr);
}
