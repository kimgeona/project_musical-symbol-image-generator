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

// c++17
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace MSIG
{
namespace Utility
{


std::string grep(std::filesystem::path filePath, std::string grepString)
{
    namespace fs = std::filesystem;
    
    // 1. 파일 열기
    std::fstream fin(filePath.string(), std::ios::in);
    if (!fin)
        throw std::runtime_error("MSIG::Utility::grep() : 파일을 열 수 없습니다.");
    
    // 2. grepString가 포함된 첫 문자열 찾아서 반환
    std::string line;
    while (getline(fin, line)) {
        if (line.find(grepString)!=-1) {
            fin.close();
            return line;
        }
    }
    return "";
}

void        attach(std::filesystem::path filePath, std::string attachString)
{
    namespace fs = std::filesystem;
    
    // 1. 파일 열기
    std::fstream fout(filePath.string(), std::ios::out|std::ios::app);
    if (!fout)
        throw std::runtime_error("MSIG::Utility::attach() : 파일을 열 수 없습니다.");
    
    // 2. 파일의 맨 뒤에 해당 문자열 추가
    fout << std::endl << attachString;
    fout.close();
}

std::vector<std::string> split(std::string str, std::string splitStr)
{
    namespace fs = std::filesystem;
    
    // 1. 빈 문자열 검사
    if (str.empty()) return std::vector<std::string>();
    
    // 2. str을 splitStr로 분할하여 벡터에 저장 후 반환.
    std::vector<std::string> result;
    size_t p = 0;
    size_t n = str.find(splitStr);
    while (n != std::string::npos) {
        result.push_back(str.substr(p, n-p));
        p = n + 1;
        n = str.find(splitStr, p);
    }
    result.push_back(str.substr(p, str.length()-p));
    return result;
}

std::vector<std::vector<std::string>> split(std::string str, std::string splitStr, std::string splitStr2)
{
    namespace fs = std::filesystem;
    
    // 1. 빈 문자열 검사
    if (str.empty()) return std::vector<std::vector<std::string>>();
    
    // 2. str을 splitStr로 분할하여 벡터에 저장 후 반환.
    std::vector<std::string> strs;
    size_t p = 0;
    size_t n = str.find(splitStr);
    while (n != std::string::npos) {
        strs.push_back(str.substr(p, n-p));
        p = n + 1;
        n = str.find(splitStr, p);
    }
    strs.push_back(str.substr(p, str.length()-p));
    
    // 3. strs을 splitStr2로 분할하여 벡터에 저장 후 반환.
    std::vector<std::vector<std::string>> result;
    for (auto& s : strs) {
        result.push_back(split(s, splitStr2));
    }
    
    return result;
}


}
}

#endif /* msig_Utility_hpp */
