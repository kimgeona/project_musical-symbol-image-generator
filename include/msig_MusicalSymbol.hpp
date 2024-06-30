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

namespace msig
{


class MusicalSymbol {
public:
    // 상태 체크
    size_t                  status;
    
    // 악상기호 이미지 기본 설정
    int                     img_w = 256;    // 생성할 이미지 너비
    int                     img_h = 512;    // 생성할 이미지 높이
    int                     pad = 26;       // 오선지 간격
    
    // 악상기호 이미지 관련 디렉토리
    std::filesystem::path   dir;            // 악상 기호 저장 주소
    std::filesystem::path   dir_config;     // 악상 기호 config 파일 주소
    
    // 데이터
    cv::Mat                 img;            // 악상 기호 이미지
    int                     x;              // 중심점 x
    int                     y;              // 중심점 y
    double                  rotate;         // 회전
    double                  scale;          // 확대, 축소
    
    // 초기화 관련
    int init_dir        (std::filesystem::path dir);    // dir 초기화
    int init_dir_config (std::filesystem::path dir);    // dir_config 초기화
    int init_img        ();                             // image 초기화
    int init_config     ();                             // config 초기화
    
    // 데이터 관련
    int     make_config();  // config 생성하기
    int     edit_config();  // config 수정하기
    int     save_config();  // config 저장하기
    void    set_default();  // 현재 rotate, scale 상태를 이미지에 적용하고 각각 기본값 저장.
    
    // 미리보기
    void show();
    
    // 생성자
    MusicalSymbol();                                                                // 빈 생성자
    MusicalSymbol(std::filesystem::path dir, std::filesystem::path dir_config);     // 기본 생성자
    MusicalSymbol(const MusicalSymbol& other);                                      // 복사 생성자
    
    // 연산자 함수
    MusicalSymbol&  operator=(const MusicalSymbol& other);                          // 복사(대입) 연산자
    bool            operator==(const MusicalSymbol& other) const;                   // 비교 연산자
    MusicalSymbol&  operator+=(const MusicalSymbol& other);                         // 덧셈 대입 연산자
    bool            operator&(const MusicalSymbol& other);                          // 이미지 겹침 확인 연산자
};


}

#endif /* msig_MusicalSymbol_hpp */
