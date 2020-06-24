
cmake_minimum_required(VERSION 3.13)
project(libwlanaccess)
set(CMAKE_C_STANDARD 99)            # C 표준
set(CMAKE_VERBOSE_MAKEFILE true)    # 컴파일 메시지 출력 활성화
add_compile_options(-Wall)


#########################################################################################################
### 사용자 설정 영역 - 플랫폼, 칩 디바이스, 버전
#########################################################################################################
set(TARGET_PLATFORM armhf32)          # x64, arm32, armhf32, aarch64
set(TARGET_DEVICE saf5100)        # saf5100, saf5400, craton2, secton
set(TARGET_PLATFORM_V2X_IF_NUM 4) # 플랫폼에서 지원하는 V2X 인터페이스 최대 개수
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION_META "")    # 메타번호는 '-' 문자로 시작해야 한다.
#########################################################################################################
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_META}")


#########################################################################################################
# 디렉터리 정의
#########################################################################################################
set(DOC_DIR ${CMAKE_CURRENT_LIST_DIR}/doc)
set(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/output)
set(PRODUCT_DIR ${CMAKE_CURRENT_LIST_DIR}/product)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(COMMON_SRC_DIR ${SRC_DIR}/common/src)
set(PRODUCT_INCLUDE_DIR ${PRODUCT_DIR}/include/wlanaccess)
set(PRODUCT_LIB_DIR ${PRODUCT_DIR}/lib)
#########################################################################################################


#########################################################################################################
## 플랫폼 별 설정
#########################################################################################################
## 플랫폼별 컴파일러 설정
if(${TARGET_PLATFORM} STREQUAL "x64")
    set(CMAKE_C_COMPILER gcc)
elseif(${TARGET_PLATFORM} STREQUAL "arm32")
    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "armhf32")
    set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "aarch64")
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
else()
    message(FATAL_ERROR "Not supported target platform - ${TARGET_PLATFORM}")
endif()
#########################################################################################################


#########################################################################################################
## 칩디바이스 별 설정
#########################################################################################################
set(TARGET_DEVICE_DIR ${SRC_DIR}/${TARGET_DEVICE})
if(${TARGET_DEVICE} STREQUAL "saf5100")
    set(TARGET_DEVICE_SRC
            ${TARGET_DEVICE_DIR}/src/saf5100.c
            ${TARGET_DEVICE_DIR}/src/saf5100.h
            ${TARGET_DEVICE_DIR}/src/saf5100-callback.c)
else()
    message(FATAL_ERROR "Not supported target device - ${TARGET_DEVICE}")
endif()
#########################################################################################################


#########################################################################################################
### libwlanaccess 빌드
#########################################################################################################
set(TARGET_LIB wlanaccess)
set(TARGET_LIB_FILE "lib${TARGET_LIB}.so")
add_library(${TARGET_LIB} SHARED
        ${PRODUCT_INCLUDE_DIR}/wlanaccess.h
        ${PRODUCT_INCLUDE_DIR}/wlanaccess-types.h
        ${PRODUCT_INCLUDE_DIR}/wlanaccess-api.h
        ${COMMON_SRC_DIR}/wlanaccess-internal.h
        ${COMMON_SRC_DIR}/wlanaccess-80211.h
        ${COMMON_SRC_DIR}/wlanaccess-16094.h
        ${COMMON_SRC_DIR}/wlanaccess.c
        ${TARGET_DEVICE_SRC})
target_include_directories(${TARGET_LIB} PUBLIC
        ${PRODUCT_INCLUDE_DIR}
        ${COMMON_SRC_DIR})
target_compile_definitions(${TARGET_LIB} PUBLIC
        _DEBUG_
        _V2X_IF_NUM_=${TARGET_PLATFORM_V2X_IF_NUM}
        _PLATFORM_="${TARGET_DEVICE}")
target_include_directories(${TARGET_LIB} PUBLIC ${TARGET_DEVICE_DIR}/ext)
target_link_directories(${TARGET_LIB} PUBLIC ${TARGET_DEVICE_DIR}/ext/${TARGET_PLATFORM})
target_link_libraries(${TARGET_LIB} LLC)
#########################################################################################################


#########################################################################################################
### test-app/wlanaccess-test 빌드
#########################################################################################################
set(TARGET_TEST_APP wlanaccess-test)
set(TEST_APP_DIR ${CMAKE_CURRENT_LIST_DIR}/test-app)
add_executable(${TARGET_TEST_APP} ${TEST_APP_DIR}/wlanaccess-test.c)
target_include_directories(${TARGET_TEST_APP} PUBLIC ${PRODUCT_INCLUDE_DIR})
target_link_directories(${TARGET_TEST_APP} PUBLIC ${PRODUCT_LIB_DIR})
target_link_libraries(${TARGET_TEST_APP} wlanaccess pthread)
#########################################################################################################


#########################################################################################################
## 빌드된 라이브러리 파일의 출력 디렉터리 설정
#########################################################################################################
set_target_properties(${TARGET_LIB}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PRODUCT_LIB_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PRODUCT_LIB_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PRODUCT_LIB_DIR})
add_custom_command(TARGET ${TARGET_LIB} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove "${OUTPUT_DIR}/*")
add_custom_command(TARGET ${TARGET_LIB} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PRODUCT_LIB_DIR}/${TARGET_LIB_FILE} "${OUTPUT_DIR}/${TARGET_LIB_FILE}"
        )#COMMAND ${CMAKE_COMMAND} -E copy ${PRODUCT_LIB_DIR}/${TARGET_LIB_FILE} "${OUTPUT_DIR}/${TARGET_LIB_FILE}.${VERSION}")
add_custom_command(TARGET ${TARGET_TEST_APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_TEST_APP} "${OUTPUT_DIR}/${TARGET_TEST_APP}"
        )#COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_TEST_APP} "${OUTPUT_DIR}/${TARGET_TEST_APP}.${VERSION}")
#########################################################################################################
