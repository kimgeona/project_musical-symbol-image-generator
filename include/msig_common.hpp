/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_common.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개발에 사용되는 기본적인 헤더파일들과 매크로가 정의되어있는 헤더파일.
 -------------------------------------------------------------------------------
 */

#ifndef msig_common_hpp
#define msig_common_hpp

// 입출력
#include <iostream>
#include <fstream>

// 파일 시스템
#include <filesystem>

// 컨테이너
#include <queue>
#include <vector>
#include <array>
#include <set>
#include <map>

// 알고리즘
#include <algorithm>

// 문자열 연산
#include <string>
#include <regex>

// 랜덤
#include <random>

// 스레딩
#include <thread>
#include <mutex>

// 영상처리
#include <opencv2/opencv.hpp>

namespace MSIG
{
    // MSIG 버전
    #define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))

    // MusicalSymbol Positioning 매크로
    #define MS_POSITION_TYPE uint8_t
    #define MS_FIXED  0b00000000
    #define MS_TOP    0b00000001
    #define MS_BOTTOM 0b00000010
    #define MS_LEFT   0b00000100
    #define MS_RIGHT  0b00001000
    #define MS_STAFF  0b10000000
    #define MS_IN     0b00010000
    #define MS_OUT    0b00100000

    // TODO: 에러 클래스와 해당 에러가 무슨 타입인지를 나타내는 에러 매크로 작성하기
}

#endif /* msig_common_hpp */
