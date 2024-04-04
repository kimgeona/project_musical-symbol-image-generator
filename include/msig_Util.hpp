#pragma once
#ifndef msig_Util_hpp
#define msig_Util_hpp

// c++17
#include <iostream>     // 입출력 스트림
#include <fstream>      // 파일 스트림
#include <filesystem>   // 파일 시스템
#include <vector>
#include <string>
namespace msig {


// 주소 관련
std::string                 my_dir(std::filesystem::path dir);                  // 주소 변환

// 파일 관련
std::string                 my_grep(std::filesystem::path dir, std::string s);  // 터미널 grep

// 문자열 관련
std::vector<std::string>    my_split(std::string s1, std::string s2);           // 파이썬 split
std::string                 my_trim(std::string s1);                            // 파이썬 trim
std::string                 my_lower(std::string s1);                           // 파이썬 lower


}

#endif /* msig_Util_hpp */
