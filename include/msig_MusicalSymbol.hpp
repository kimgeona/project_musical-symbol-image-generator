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
#include <msig_Util.hpp>

namespace msig {
class MusicalSymbol {
public:
    // 상태 체크
    int                     bad;
    
    // 생성할 이미지 정보
    int                     img_w=256;  // 생성할 이미지 너비
    int                     img_h=512;  // 생성할 이미지 높이
    int                     pad = 26;   // 오선지 간격
    
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
    int edit_config     ();                             // config 수정하기
    
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
