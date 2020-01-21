cmake_minimum_required(VERSION 3.13)
project(libdot3)
set(CMAKE_C_STANDARD 99)            # C 표준
set(CMAKE_VERBOSE_MAKEFILE true)    # 컴파일 메시지 출력 활성화
add_compile_options(-Wall)


#########################################################################################################
### 사용자 설정 영역
#########################################################################################################
## 일반
set(TARGET_PLATFORM armhf32)        # x64, arm32, armhf32, aarch64
set(ASN1_LIB_VENDOR ffasn1c)        # objasn1c, ffasn1c
set(ASN1_LIB_VERSION 7.3.1)         # objasn1c인 경우에만 사용
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(VERSION_META "")    # 메타번호는 '-' 문자로 시작해야 한다.

## 단위테스트 빌드 선택 - 주의: TARGET_PLATFORM이 x64일 경우에만 빌드됨
set(BUILD_UNIT_TEST true)                 # true, false
set(BUILD_UNIT_TEST_API true)             # true, false
set(BUILD_UNIT_TEST_INTERNAL_FUNC true)   # true, false

## 1609.3 속성
set(PSR_MAX_NUM 128)                # PSR 테이블 최대저장개수 (표준상 기본값 = 128)
set(WSA_SERVICE_INFO_MAX_NUM 31)    # WSA에 수납되는 Service info 최대 개수 (표준상 기본값 = 31)
set(WSA_CHAN_INFO_MAX_NUM 31)       # WSA에 수납되는 Chan info 최대 개수 (표준상 기본값 = 31)
#########################################################################################################
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_META}")


#########################################################################################################
# 디렉터리 정의
#########################################################################################################
set(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/output)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/ext)
set(EXT_ASN1_DIR ${EXT_DIR}/asn1)
if(${ASN1_LIB_VENDOR} STREQUAL "objasn1c")
    set(EXT_ASN1_LIB_DIR ${EXT_ASN1_DIR}/${ASN1_LIB_VENDOR}/${ASN1_LIB_VERSION})
elseif(${ASN1_LIB_VENDOR} STREQUAL "ffasn1c")
    set(EXT_ASN1_LIB_DIR ${EXT_ASN1_DIR}/${ASN1_LIB_VENDOR})
else()
    message(FATAL_ERROR "Not supported asn.1 library - ${ASN1_LIB}")
endif()
set(PRODUCT_DIR ${CMAKE_CURRENT_LIST_DIR}/product)
set(PRODUCT_INCLUDE_DIR ${PRODUCT_DIR}/include)
set(PRODUCT_LIB_DIR ${PRODUCT_DIR}/lib)
#########################################################################################################


#########################################################################################################
## 플랫폼/운영체제 별 설정
#########################################################################################################
## 타겟플랫폼 컴파일러 설정
if(${TARGET_PLATFORM} STREQUAL "x64")
    #set(CMAKE_C_COMPILER gcc)    # 활성화 시 gtest 에서 에러 발생
    #set(CMAKE_CXX_COMPILER g++)  # 활성화 시 gtest 에서 에러 발생
elseif(${TARGET_PLATFORM} STREQUAL "arm32")
    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "armhf32")
    set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "aarch64")
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    #set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
elseif(${TARGET_PLATFORM} STREQUAL "ppc")
    set(CMAKE_C_COMPILER powerpc-linux-gnu-gcc)
else()
    message(FATAL_ERROR "Not supported target platform - ${TARGET_PLATFORM}")
endif()
#########################################################################################################


#########################################################################################################
### libdot3 라이브러리 빌드
#########################################################################################################
## libdot3 컴파일/빌드
set(TARGET_LIB dot3)
set(TARGET_LIB_FILE "lib${TARGET_LIB}.so")
add_library(${TARGET_LIB} SHARED
        ${PRODUCT_INCLUDE_DIR}/dot3/dot3.h
        ${PRODUCT_INCLUDE_DIR}/dot3/dot3-types.h
        ${PRODUCT_INCLUDE_DIR}/dot3/dot3-api.h
        ${SRC_DIR}/dot3.c
        ${SRC_DIR}/dot3-chaninfo.c
        ${SRC_DIR}/dot3-check-validity.c
        ${SRC_DIR}/dot3-convert.c
        ${SRC_DIR}/dot3-internal.h
        ${SRC_DIR}/dot3-log.c
        ${SRC_DIR}/dot3-mib.h
        ${SRC_DIR}/dot3-mpdu.c
        ${SRC_DIR}/dot3-psr.c
        ${SRC_DIR}/dot3-wsa.c
        ${SRC_DIR}/dot3-wsm.c
        ${SRC_DIR}/api/dot3-api.c
        ${SRC_DIR}/api/dot3-api-psr.c
        ${SRC_DIR}/api/dot3-api-wsa.c
        ${SRC_DIR}/api/dot3-api-wsm.c
        ${SRC_DIR}/api/dot3-api-wsr.c)
if(${ASN1_LIB_VENDOR} STREQUAL "objasn1c")
    target_sources(${TARGET_LIB} PUBLIC
            ${EXT_ASN1_LIB_DIR}/gen-src/dot3-asn.c
            ${EXT_ASN1_LIB_DIR}/gen-src/dot3-asn.h
            ${EXT_ASN1_LIB_DIR}/gen-src/rtkey.h
            ${SRC_DIR}/asn1/objasn1c/dot3-objasn1c.h
            ${SRC_DIR}/asn1/objasn1c/dot3-objasn1c-wsm-encode.c)
    target_include_directories(${TARGET_LIB} PUBLIC ${SRC_DIR}/asn1/objasn1c)
elseif(${ASN1_LIB_VENDOR} STREQUAL "ffasn1c")
    target_sources(${TARGET_LIB} PUBLIC
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1constraints.c
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1defs.h
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1defs_int.h
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1per_dec.c
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1per_enc.c
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1random.c
            ${EXT_ASN1_LIB_DIR}/libffasn1/asn1utils.c
            ${EXT_ASN1_LIB_DIR}/asn1mem.c
            ${EXT_ASN1_LIB_DIR}/gen-src/dot3-asn.c
            ${EXT_ASN1_LIB_DIR}/gen-src/dot3-asn.h
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c.c
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c.h
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c-wsa-decode.c
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c-wsa-encode.c
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c-wsm-decode.c
            ${SRC_DIR}/asn1/ffasn1c/dot3-ffasn1c-wsm-encode.c)
    target_include_directories(${TARGET_LIB} PUBLIC ${SRC_DIR}/asn1/ffasn1c)
else()
    message(FATAL_ERROR "Not supported asn.1 library - ${ASN1_LIB}")
endif()
target_compile_definitions(${TARGET_LIB} PUBLIC
        _DEBUG_
        _PSR_MAX_NUM_=${PSR_MAX_NUM}
        _WSA_SERVICE_INFO_MAX_NUM_=${WSA_SERVICE_INFO_MAX_NUM}
        _WSA_CHAN_INFO_MAX_NUM_=${WSA_CHAN_INFO_MAX_NUM})
target_include_directories(${TARGET_LIB} PUBLIC
        ${PRODUCT_INCLUDE_DIR}
        ${SRC_DIR}
        ${EXT_DIR})
if(${BUILD_UNIT_TEST_INTERNAL_FUNC} STREQUAL "true")
    target_compile_definitions(${TARGET_LIB} PUBLIC _EXPORT_INTERNAL_FUNC_)
endif()

## 3rd party asn.1 라이브러리 연계
## TODO:: TARGET_OS 관련 부분 제거해야 함 (리눅스로 통일)
if(${ASN1_LIB_VENDOR} STREQUAL "objasn1c")
    target_compile_definitions(${TARGET_LIB} PUBLIC OBJASN1C_)
    target_include_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/include ${EXT_ASN1_LIB_DIR}/gen-src)
    if(${TARGET_PLATFORM} STREQUAL "x64")
        if(${TARGET_OS} STREQUAL "win")
            target_link_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/lib/x64-win)
         elseif(${TARGET_OS} STREQUAL "linux")
            target_link_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/lib/x64-linux/libgcc4)
         else()
            message(FATAL_ERROR "Not supported target platform - ${TARGET_PLATFORM}")
         endif()
    elseif(${TARGET_PLATFORM} STREQUAL "arm32")
        message(FATAL_ERROR "objasn1c for ${TARGET_PLATFORM} platform is not supported")
    elseif(${TARGET_PLATFORM} STREQUAL "armhf32")
        target_link_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/lib/armhf-linux/Linaro-gcc-4.8.2/libgcc)
    elseif(${TARGET_PLATFORM} STREQUAL "ppc")
        target_link_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/lib/ppc-linux/GNN-gcc-6/libgcc)
    else()
        message(FATAL_ERROR "Not supported target platform for objasn1c - ${TARGET_PLATFORM}")
    endif()
    if(${TARGET_OS} STREQUAL "win")
        target_link_libraries(${TARGET_LIB} asn1perd asn1rtd)
    elseif(${TARGET_OS} STREQUAL "linux")
        target_link_libraries(${TARGET_LIB} asn1per asn1rt)
    else()
        message(FATAL_ERROR "Not supported target os - ${TARGET_OS}")
    endif()
elseif(${ASN1_LIB_VENDOR} STREQUAL "ffasn1c")
    target_compile_definitions(${TARGET_LIB} PUBLIC FFASN1C_)
    target_include_directories(${TARGET_LIB} PUBLIC ${EXT_ASN1_LIB_DIR}/libffasn1 ${EXT_ASN1_LIB_DIR}/gen-src)
else()
    message(FATAL_ERROR "Not supported asn.1 library - ${ASN1_LIB}")
endif()
#########################################################################################################


#########################################################################################################
### test-app/dot3-test 빌드
#########################################################################################################
set(TARGET_TEST_APP dot3-test)
set(TEST_APP_DIR ${CMAKE_CURRENT_LIST_DIR}/test/test-app)
add_executable(${TARGET_TEST_APP} ${TEST_APP_DIR}/dot3-test.c)
target_include_directories(${TARGET_TEST_APP} PUBLIC ${PRODUCT_INCLUDE_DIR})
target_link_directories(${TARGET_TEST_APP} PUBLIC ${PRODUCT_LIB_DIR})
target_link_libraries(${TARGET_TEST_APP} dot3 pthread)
#########################################################################################################


#########################################################################################################
## 단위테스트 빌드 (x64 PC에서만)
#########################################################################################################
if(${TARGET_PLATFORM} STREQUAL "x64")
    if(${BUILD_UNIT_TEST} STREQUAL "true")
        set(UNIT_TEST_DIR test/unit-test)
        set(GTEST_SRC_DIR ${UNIT_TEST_DIR}/googletest-release-1.8.1)

        ## gtest 빌드
        add_subdirectory(${GTEST_SRC_DIR})

        ## 내부함수 단위테스트 빌드
        if(${BUILD_UNIT_TEST_INTERNAL_FUNC} STREQUAL "true")
            set(INTERNAL_FUNC_UNIT_TEST_DIR ${UNIT_TEST_DIR}/internal-func-test)
            set(TARGET_INTERNAL_FUNC_UNIT_TEST runDot3InternalFuncUnitTest)
            add_executable(${TARGET_INTERNAL_FUNC_UNIT_TEST}
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-ConstructWsa.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-ConstructWsm.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-ParseWsa.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-ParseWsm.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-Psr.cc
                    ${INTERNAL_FUNC_UNIT_TEST_DIR}/internal-func-test-sample-data.cc)
            target_include_directories(${TARGET_INTERNAL_FUNC_UNIT_TEST} PUBLIC ${GTEST_SRC_DIR}/googletest/include)
            target_link_libraries(${TARGET_INTERNAL_FUNC_UNIT_TEST} gtest gtest_main)
            target_include_directories(${TARGET_INTERNAL_FUNC_UNIT_TEST} PUBLIC ${SRC_DIR})
            target_link_directories(${TARGET_INTERNAL_FUNC_UNIT_TEST} PUBLIC ${PRODUCT_LIB_DIR})
            target_link_libraries(${TARGET_INTERNAL_FUNC_UNIT_TEST} ${TARGET_LIB})
            target_compile_definitions(${TARGET_INTERNAL_FUNC_UNIT_TEST} PUBLIC
                    _PSR_MAX_NUM_=${PSR_MAX_NUM}
                    _EXPORT_INTERNAL_FUNC_)
        endif()

        ## API 단위테스트 빌드
        if(${BUILD_UNIT_TEST_API} STREQUAL "true")
            set(API_UNIT_TEST_DIR ${UNIT_TEST_DIR}/api-test)
            set(TARGET_API_UNIT_TEST runDot3ApiUnitTest)
            add_executable(${TARGET_API_UNIT_TEST}
                    ${API_UNIT_TEST_DIR}/api-test.cc
                    ${API_UNIT_TEST_DIR}/api-test-Dot3_ConstructWsa.cc
                    ${API_UNIT_TEST_DIR}/api-test-Dot3_ConstructWsmMpdu.cc
                    ${API_UNIT_TEST_DIR}/api-test-Dot3_ParseWsa.cc
                    ${API_UNIT_TEST_DIR}/api-test-Dot3_ParseWsmMpdu.cc
                    ${API_UNIT_TEST_DIR}/api-test-Dot3_Psr.cc
                    ${API_UNIT_TEST_DIR}/api-test-sample-data.cc)
            target_include_directories(${TARGET_API_UNIT_TEST} PUBLIC ${GTEST_SRC_DIR}/googletest/include)
            target_link_libraries(${TARGET_API_UNIT_TEST} gtest gtest_main)
            target_include_directories(${TARGET_API_UNIT_TEST} PUBLIC ${PRODUCT_INCLUDE_DIR})
            target_link_directories(${TARGET_API_UNIT_TEST} PUBLIC ${PRODUCT_LIB_DIR})
            target_link_libraries(${TARGET_API_UNIT_TEST} ${TARGET_LIB})
        endif()
    endif()
endif()
#########################################################################################################


#########################################################################################################
## 빌드된 라이브러리 파일의 출력 디렉터리 설정
#########################################################################################################
set_target_properties(${TARGET_LIB} PROPERTIES
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
