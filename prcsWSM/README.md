# v2x-obu

v2x-sw 라이브러리들을 이용하여 OBU 동작을 수행하는 샘플 어플리케이션

프로젝트 경로: PROJECT_ROOT/v2x-obu



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
  - VERSION_* : 버전을 선택한다.



### 빌드

```
HostPC$ cd v2x-obu
HostPC$ rm CMakeCache.txt    (해당 파일이 존재할 경우)
HostPC$ cmake CMakeLists.txt
HostPC$ make clean;make
```

빌드에 성공하면 output/ 디렉터리에 "obu" 실행파일이 생성된다.



## 타겟보드 실행 방법

### 파일 다운로드

다음 파일들을 타겟보드로 다운로드한다.

- obu (실행파일)
- 타겟보드의 /lib/ 디렉터리에 libwlanaccess.so, libdot3.so 파일이 존재해야 한다.



### 어플리케이션 실행

타겟보드에서 실행한다.

```
Target$ sudo ./obu
```

