# Musical-Symbol-Image-Generator (v.1.1.1)

"MSIG(Musical-Symbol-Image-Generator)"는 머신러닝 학습을 위한 악상 기호 데이터셋 생성기입니다. 해당 프로그램은 악상기호를 표현 가능한 모든 경우의 이미지를 생성하는 것을 목표로 하고 있습니다.

현재 해당 프로그램은 현재 실행파일로 제공되지 않고 원본 소스코드로 제공됩니다. 사용하시기 위해서는 이를 직접 빌드 해서 사용하셔야 합니다. 다음 절에서 빌드 방법을 설명합니다.

>지원 환경 : MacOS(Xcode), Windows(VisualStudio2022)

## 업데이트 내용 (v.1.1.0)
* 멀티 스레드를 이용한 데이터셋 생성속도 개선.
* 새로운 DST(의존적 선택 트리)알고리즘의 작성으로 악상기호 조합 계산 성능 대폭 향상.
* 너무 많은 이미지 생성 방지를 위해 각 조합별 생성할 이미지의 비율을 지정하는 기능 추가.

## 프로젝트 환경 준비
해당 프로그램을 사용하기 위해서는 git, make, cmake가 필요합니다. 해당 프로그램들을 먼저 설치하시고 다음 과정을 따라 주시기 바랍니다. (MacOS는 homebrew를 Windows에는 winget을 이용하시면 git, make, cmake를 편리하게 설치하실 수 있습니다.)

## 프로젝트 빌드 방법
프로젝트 관리 도구(Project)를 이용하여 OpenCV 라이브러리와 현재 프로젝트를 다운받아 빌드하시면 됩니다. 방법은 다음과 같습니다.

    # 프로젝트 관리 도구(Project) 다운
    $ git clone https://github.com/kimgeona/project.git

    # 프로젝트 관리 디렉토리로 이동
    $ cd prject

    # OpenCV 라이브러리 다운 및 빌드
    $ make install_opencv

윈도우 운영체제 사용자는 OpenCV 라이브러리 경로를 환경 변수에 직접 등록해 주셔야 합니다. 설치된 OpenCV 라이브러리 경로는 \$ make install_opencv 명령의 수행이 끝나면 자동으로 출력되며, 해당 출력 결과를 놓쳤다면 \$ make info 명령을 통해서 다시 확인하실 수 있습니다.

    # MSIG(Musical-Symbol-Image-Generator) 프로젝트 다운 및 프로젝트 생성
    $ make project PROJECT_REPO=https://github.com/kimgeona/project_musical-symbol-image-generator.git

프로젝트 관리 도구에 대한 자세한 설명은 다음 링크에서 나와져 있습니다. 혹시 해당 과정에서 문제가 발생한다면 다음 링크를 참조하세요. https://github.com/kimgeona/project.git

## 사용 방법
위 프로젝트 빌드를 마시면 project/project_musical-symbol-image-generator/build 디렉토리에 IDE 프로젝트 파일이 생성되어 있을겁니다. 각각의 프로젝트를 실행하고 my_program 이라는 타겟(exe)을 빌드하여 실행하시면 데이터셋이 생성됩니다.

>데이터셋은 project/project_musical-symbol-image-generator/src_io 디렉토리에 생성됩니다.

## 설치 제거
해당 프로젝트와 관련된 모든것은 단순히 맨 처음 git clone 을 통해 다운 받았던 prject 폴더를 삭제해주시면 됩니다. 해당 프로젝트 안에는 OpenCV 라이브러리와 현재의 프로젝트가 담겨져 있습니다.

>윈도우 운영체제 사용자는 OpenCV 라이브러리 경로를 환경 변수에서 제거해 주셔야 완벽히 제거됩니다. 해당 라이브러리 경로는 $ make info 를 통해 확인 가능합니다.

