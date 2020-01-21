cmake_minimum_required(VERSION 3.13)
project(timeSync)
set(CMAKE_C_STANDARD 99)            # C 표준
set(CMAKE_VERBOSE_MAKEFILE true)    # 컴파일 메시지 출력 활성화

add_compile_definitions(_PSR_MAX_NUM_=128 _WSA_SERVICE_INFO_MAX_NUM_=31 _WSA_CHAN_INFO_MAX_NUM_=31)

#########################################################################################################
### 사용자 설정 영역
#########################################################################################################
set(TARGET_PLATFORM aarch64)        # 가능 항목 : x64, arm, armhf, aarch64, ppc, ...
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
set(EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/ext)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PRODUCT_DIR ${CMAKE_CURRENT_LIST_DIR}/product)
set(EXT_INC_DIR ${EXT_DIR}/include)
set(EXT_LIB_DIR ${EXT_DIR}/lib/${TARGET_PLATFORM})
#set(FFASN1_DIR ${CMAKE_CURRENT_LIST_DIR}/../../J2735/ffasn1c/)
#set(FFASN1_INC_DIR ${FFASN1_DIR}/include)
#set(FFASN1_LIB_DIR ${FFASN1_DIR}/Debug/)
#set(CITS_DIR ${CMAKE_CURRENT_LIST_DIR}/../../J2735/J2735_CITS_DS/)
#set(CITS_INC_DIR ${CITS_DIR}/include)
#set(CITS_LIB_DIR ${CITS_DIR}/Debug)
#set(GPSD_LIB_DIR ~/GPSD/gpsd-3.17/)
#########################################################################################################


#########################################################################################################
## 플랫폼/운영체제 별 설정
#########################################################################################################
## 타겟플랫폼별 컴파일러 경로 설정
if(${TARGET_PLATFORM} STREQUAL "x64")
    set(CMAKE_C_COMPILER gcc)
elseif(${TARGET_PLATFORM} STREQUAL "arm")
    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "armhf")
    set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "aarch64")
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
elseif(${TARGET_PLATFORM} STREQUAL "ppc")
    set(CMAKE_C_COMPILER powerpc-linux-gnu-gcc)
else()
    message(FATAL_ERROR "Not supported target platform - ${TARGET_PLATFORM}")
endif()
#########################################################################################################


#########################################################################################################
### prcsJ2735 어플리케이션 빌드
#########################################################################################################
## prcsJ2735 컴파일/빌드
set(TARGET_APP timeSync)
set(OUTPUT_FILE "${TARGET_APP}")
add_executable(${TARGET_APP}
        ${SRC_DIR}/main.c)

add_compile_options(-Wall)
target_compile_definitions(${TARGET_APP} PUBLIC
        DEBUG_)
target_include_directories(${TARGET_APP} 
        PUBLIC
        ${EXT_INC_DIR} 
        ${SRC_DIR})
target_link_directories(${TARGET_APP} PUBLIC
        ${EXT_LIB_DIR})
target_link_libraries(${TARGET_APP}
        gps
        m
        rt)
#########################################################################################################


#########################################################################################################
## 빌드된 파일의 출력 디렉터리 설정
#########################################################################################################
set_target_properties(${TARGET_APP} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
#cp ${OUTPUT_DIR}${TARGET_APP} /media/sf_Source/tftp/    
#########################################################################################################
