#pragma once
#ifndef msig_Note_hpp
#define msig_Note_hpp

// c++17
#include <iostream>     // 입출력 스트림
#include <fstream>      // 파일 입출력
#include <regex>        // 정규 표현식

// 나의 라이브러리
#include <dst.hpp>
#include <msig_MusicalSymbol.hpp>
#include <msig_Matrix.hpp>

namespace msig {
class Note
{
    // 데이터셋 주소
    std::filesystem::path       dir_ds;             // 데이터셋 위치
    std::filesystem::path       dir_ds_config;      // 데이터셋 config 파일 위치
    std::filesystem::path       dir_ds_complete;    // 데이터셋 complete 폴더 위치
    std::filesystem::path       dir_ds_piece;       // 데이터셋 piece 폴더 위치

    // 데이터셋
    std::vector<MusicalSymbol>  ds_complete;    // 데이터셋 : 완성형
    std::vector<MusicalSymbol>  ds_piece;       // 데이터셋 : 조합형
    
    // Dependent Selection Tree
    dst::DSTree                 symbol_selector;    // 의존적 선택 트리 알고리즘
    
    // draw_list
    std::vector<MusicalSymbol>  draw_list;          // 그릴 목록

    // msig_Note_Init.cpp
    int init_dir(std::filesystem::path dir);    // 데이터셋 주소 초기화 함수
    int init_symbol_selector();                 // 의존적 선택 트리 알고리즘 초기화 함수
    int init_ds();                              // 데이터셋 불러오기
    int init_ds_complete();                     // 완성형 데이터셋 불러오기
    int init_ds_piece();                        // 조합형 데이터셋 불러오기
    
    // msig_Note_Draw.cpp
    cv::Mat draw();
    cv::Mat draw_symbols(const cv::Mat& img, const cv::Mat& img_symbol, std::string img_config, bool auxiliary_line=false);
    
    // msig_Note_Backup.cpp
    void save_config();         // config 값 새로 저장(리프레쉬)
    void add_config();          // config 값 추가(업데이트)
    
    // msig_Note_Utility.cpp
    std::vector<std::string>    my_split(std::string s1, std::string s2);   // 문자열 분리
    std::string                 my_trim(std::string s1);                    // 문자열 앞,뒤 공백 제거
    std::string                 my_lower(std::string s1);                   // 문자열 소문자로 변환
    
    
public:
    // msig_Note.cpp
    Note();
    Note(std::string dataset_dir);
    
    // msig_Note_API.cpp
    int     set(std::string type, std::string name);    // 그릴 음표 설정
    void    save_as_img(std::string file_name);         // 그린 내용을 file_name 이름으로 저장
    void    show();                                     // 그린 내용을 화면에 보여주기
    cv::Mat cv_Mat();                                   // 그린 내용을 cv::Mat() 형식으로 반환
    void    edit_config();                              // 현재 불러와진 config 정보들 수정
};
 

}

#endif /* msig_Note_hpp */
