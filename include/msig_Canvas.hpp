#pragma once
#ifndef msig_Note_hpp
#define msig_Note_hpp

// c++17
#include <iostream>     // 입출력 스트림
#include <fstream>      // 파일 입출력
#include <regex>        // 정규 표현식
#include <map>          // map 컨테이너
#include <vector>       // vector 컨테이너

// 나의 라이브러리
#include <msig_Algorithm_DST.hpp>
#include <msig_Algorithm_MSPA.hpp>
#include <msig_Matrix.hpp>
#include <msig_MusicalSymbol.hpp>
#include <msig_Util.hpp>

namespace msig {
class Canvas
{
    // 캔버스 크기
    int width;
    int height;
    
    // 데이터셋 주소
    std::filesystem::path   dir_ds;             // 데이터셋 주소
    std::filesystem::path   dir_ds_config;      // config 파일 주소
    std::filesystem::path   dir_ds_complete;    // complete 데이터셋 주소
    std::filesystem::path   dir_ds_piece;       // piece 데이터셋 주소

    // 데이터셋
    std::map<std::filesystem::path, MusicalSymbol>  ds_complete;    // 완성형 데이터셋
    std::map<std::filesystem::path, MusicalSymbol>  ds_piece;       // 조합형 데이터셋
    
    // 그릴 목록
    std::vector<MusicalSymbol>  draw_list;

    // 초기화 함수
    int set_size(int width=256, int height=512);        // 캔버스 크기 설정
    int set_dataset_dirs(std::filesystem::path dir);    // 데이터셋 주소를 설정
    int load_dataset_complete();                        // 완성형 데이터셋 불러오기
    int load_dataset_piece();                           // 조합형 데이터셋 불러오기
    
    // 그리기 함수
    cv::Mat draw();                                         // 그리기
    void    locationing(MSPA& mspa, MusicalSymbol& ms);     // 그리기 위치 조정
    cv::Mat draw_symbols(const cv::Mat& img, const MusicalSymbol& ms, bool auxiliary_line=false);
    
public:
    // 생성자
    Canvas();                                                           // 기본 생성자
    Canvas(std::filesystem::path dataset_dir, int width, int height);   // 기본 생성자
    Canvas(const Canvas& other);                                        // 복사 생성자
    
    // 연산자 함수
    Canvas& operator=(const Canvas& other);         // 복사 연산자
    bool    operator==(const Canvas& other) const;  // 비교 연산자
    
    // 사용자
    int     select(std::filesystem::path dir);  // 그릴 음표 선택
    void    select_celar();                     // 음표 선택 초기화
    void    show();                             // 그린 이미지 확인
    void    save(std::string file_name);        // 그린 이미지 저장
};
 

}

#endif /* msig_Note_hpp */
