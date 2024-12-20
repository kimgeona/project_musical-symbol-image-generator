/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_Utility.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개별 헤더 파일들 중 하나로, 코드 작성 단순화 및 가독성 증진을 목적으로 작성된 함수들의
 정의들이 기술되어 있습니다.
 -------------------------------------------------------------------------------
 포함된 함수:
 - dir()    : 함수 설명
 - grep()   : 함수 설명
 - split()  : 함수 설명
 - trim()   : 함수 설명
 - lower()  : 함수 설명
 - ...
 -------------------------------------------------------------------------------
 */

#ifndef msig_Utility_hpp
#define msig_Utility_hpp

#include <msig_common.hpp>

namespace MSIG {
namespace Utility {


std::string
grep(std::filesystem::path filePath, std::string grepString);

void
attach(std::filesystem::path filePath, std::string attachString);

std::vector<std::string>
split(std::string str, std::string splitStr);

std::vector<std::vector<std::string>>
split(std::string str, std::string splitStr, std::string splitStr2);

std::string
trim(const std::string& str);


}
}

#endif /* msig_Utility_hpp */
