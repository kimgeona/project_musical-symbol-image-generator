/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 메인 헤더 파일로, 모든 개별 헤더 파일들을 포함하고 있습니다. 해당 파일을 include 하
 면 필요한 모든 클래스와 함수들에 대해 접근할 수 있습니다.
 -------------------------------------------------------------------------------
 포함된 헤더:
 - msig_Algorithm.hpp  : 자료구조와 알고리즘을 제공합니다.
 - msig_Processing.hpp : 이미지 처리 기능을 제공합니다.
 - msig_Utility.hpp    : 코드 작성을 단순화 하는 기능들을 제공합니다.
 - msig_Rendering.hpp  : 악상기호 이미지를 만들어줍니다.
 -------------------------------------------------------------------------------
 */

#ifndef msig_hpp
#define msig_hpp

// 프로그램 버전
#define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))

#include <msig_Algorithm.hpp>
#include <msig_Processing.hpp>
#include <msig_Utility.hpp>
#include <msig_Rendering.hpp>

#endif
