# web-server-multi-thread
네트워크 수업 프로젝트 

&nbsp;

## Build environment requirements
1. Ubuntu 20.04.2 LTS
2. gcc
3. make

&nbsp;

## Building
1. `make`

&nbsp;

## Project progress
1. 코드 컴파일, 빌드 가능하도록 Makefile 작성
  - pthread 모듈 관련 unreference 오류 --> 링커 옵션 추가
  - 빌드 시 bin 폴더 없으면 추가하도록 설정
2. 제공받은 web_server.c에서 Segmentation fault 에러 발생
  - 에러 발생 이후 다시 실행 파일 실행하면 bind() error 발생
  - ***issue 1***