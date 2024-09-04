#ifndef msig_hpp
#define msig_hpp

/*
 -------------------------------------------------------------------------------
 프로젝트: Musical-Symbol-Image-Generator
 파일이름: msig.hpp
 -------------------------------------------------------------------------------
 설명:
 이 헤더 파일은 MSIG(Musical-Symbol-Imgae-Generator) 모듈에 필요한 모든 헤더 파일을 포함합니다.
 이 파일을 포함하면, 개별 헤더 파일을 각각 포함할 필요 없이 모든 필수 종속성이 한 번에 해결됩니다.
 -------------------------------------------------------------------------------
 목적:
 이 파일은 프로젝트 내에서 헤더 파일을 포함을 단순화하기 위해 작성되었습니다.
 -------------------------------------------------------------------------------
 포함된 헤더:
 - msig_Algorithm.hpp  : 자료구조와 알고리즘을 제공합니다.
 - msig_Processing.hpp : 이미지 처리에 대한 기능을 제공합니다.
 - msig_Utility.hpp    : 코드 작성을 단순화 하기 유틸리티 기능들을 제공합니다.
 - msig_Rendering.hpp  : 악상기호 이미지를 만들어줍니다.
 -------------------------------------------------------------------------------
 참고:
 모듈에 새로운 헤더가 추가되거나 제거될 때마다 업데이트되어야 합니다.
 -------------------------------------------------------------------------------
 */

#include <msig_Algorithm.hpp>
#include <msig_Processing.hpp>
#include <msig_Utility.hpp>
#include <msig_Rendering.hpp>

// 추후 지울 예정
#include <msig_Algorithm_DST.hpp>
#include <msig_Algorithm_MSPA.hpp>
#include <msig_Canvas.hpp>
#include <msig_MSIG.hpp>
#include <msig_Matrix.hpp>
#include <msig_MusicalSymbol.hpp>
#include <msig_Util.hpp>

#endif
