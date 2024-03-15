#pragma once
#ifndef msig_MusicalSymbol_hpp
#define msig_MusicalSymbol_hpp

// c++17
#include <fstream>              // 파일 스트림
#include <filesystem>           // 파일 시스템
#include <regex>                // 정규 표현식
#include <opencv2/opencv.hpp>   // OpenCV

// 나의 라이브러리
#include <msig_Matrix.hpp>

namespace msig {
class MusicalSymbol {
public:
    // 변수
    std::filesystem::path   dir;        // 악상 기호 저장 주소
    std::filesystem::path   dir_config; // 악상 기호 config 파일 주소
    cv::Mat                 img;        // 악상 기호 이미지
    int                     x;          // 중심점 x
    int                     y;          // 중심점 y
    double                  rotate;     // 회전
    double                  scale;      // 확대, 축소
    
    // msig_MusicalSymbol_Init.cpp
    int init_dir        (std::filesystem::path dir);    // image 주소 확인
    int init_dir_config (std::filesystem::path dir);    // config 주소 확인
    int init_img        ();                             // image 불러오기
    int init_config     ();                             // config 불러오기
    
    // msig_MusicalSymbol_Data.cpp
    int make_config     ();                             // config 생성하기
    
    // msig_MusicalSymbol_Utility.cpp
    std::string                 my_dir(std::filesystem::path dir);                  // 나의 주소로 변환
    std::string                 my_grep(std::filesystem::path dir, std::string s);  // 터미널 grep
    std::vector<std::string>    my_split(std::string s1, std::string s2);           // 파이썬 split
    
    // msig_MusicalSymbol_Preprocessing.cpp
    void                        restore_img(std::filesystem::path dir);             // 이미지 재생성
    void                        remove_padding(std::filesystem::path dir);          // 이미지 여백 제거
    
    // msig_MusicalSymbol.cpp
    MusicalSymbol();
    MusicalSymbol(std::filesystem::path dir, std::filesystem::path dir_config);
    MusicalSymbol(const MusicalSymbol& other);
    
    // msig_MusicalSymbol_Operator.cpp
    MusicalSymbol&  operator=(const MusicalSymbol& other);
    bool            operator==(const MusicalSymbol& other);
};
}

#endif /* msig_MusicalSymbol_hpp */
