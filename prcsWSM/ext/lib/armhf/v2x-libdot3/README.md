# v2x-libdot3

IEEE 1609.3 표준 기능을 구현한 라이브러리

프로젝트 경로: PROJECT_ROOT/v2x-libdot3



## 빌드 방법

### 빌드환경 세팅

리눅스 PC(=Host PC)에 cmake 최신버전을 설치한다. (3.13 버전 이상이어야 한다.)  

- https://cmake.org/download/

```
HostPC$ tar zxf cmake-3.15.2.tar.gz 
HostPC$ cd cmake-3.15.2 
HostPC$ ./bootstrap && make && sudo make install
```



### 빌드 설정

빌드 설정을 변경하려면 CMakeLists.txt 파일을 수정한다.  

- CMakeLists.txt 파일 내 "사용자 설정 영역"이라고 표시된 부분의 항목을 원하는대로 수정한다.
  - TARGET_PLATFORM : 대상 플랫폼을 선택한다.
  - ASN1_LIB_VENDOR : 3rd party asn.1 라이브러리 벤더를 선택한다.
  - VERSION_* : 버전을 선택한다.



### 빌드

```
HostPC$ cd v2x-libdot3
HostPC$ rm CMakeCache.txt    (해당 파일이 존재할 경우)
HostPC$ cmake CMakeLists.txt
HostPC$ make clean;make
```

빌드에 성공하면 product/lib/ 디렉터리에 libdot3.so 파일이 생성된다.



libdot3 라이브러리를 어플리케이션에서 링크하고 사용할 수 있도록, 다음 파일들을 해당 어플리케이션 프로젝트로 복사한다. 

* product/include/dot3/ 디렉터리를 어플리케이션 프로젝트의 ext/include/ 디렉터리에 복사한다.
* product/lib/libdot3 파일을 어플리케이션 프로젝트의 ext/lib/${TARGET_PLATFORM}/ 디렉터리에 복사한다.

어플리케이션 빌드 시에 위 파일들을 인클루드 및 링크한다.



## 타겟보드 실행 방법

### 파일 다운로드

ext/lib/libdot3.so 라이브러리 파일을 타겟보드의 /lib/ 디렉터리에 다운로드한다.



### 어플리케이션 실행

이제 타겟보드에서 libdot3 라이브러리를 사용하는 어플리케이션을 실행할 수 있다. (예: v2x-chan, v2x-wsm, ...)