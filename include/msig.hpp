#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <filesystem>           // 디렉토리 주소 생성 관련
#include <regex>                // 정규 표현식 관련
#include <string>               // 스트링 변환
#include <opencv2/opencv.hpp>

#pragma once
#ifndef MSIG_H
#define MSIG_H
namespace msig
{


enum MusicalSymbolType {
    // 악상기호 레벨
    MSID_LV0 = 0x00,
    MSID_LV1 = 0x01,
    MSID_LV2 = 0x02,
    MSID_LV3 = 0x03,    // 임시
    MSID_LV4 = 0x04,    // 임시
    
    // 악상기호 타입
    MSID_LINE           = 0x00,
    MSID_MEASURE        = 0x10,
    MSID_CLEF           = 0x20,
    MSID_KEY            = 0x30,
    MSID_TIME           = 0x40,
    MSID_NOTE           = 0x50,
    MSID_REST           = 0x60,
    MSID_REPETITION     = 0x70,
    MSID_OCTAVE         = 0x80,
    MSID_ACCIDENTAL     = 0x90,
    MSID_DYNAMIC        = 0xa0,
    MSID_ARTICULATION   = 0xb0,
    MSID_ORNAMENT       = 0xc0,
};

class MusicalSymbol {
public:
    // 악상 기호 정보
    std::string                 dir;            // 저장 위치
    std::string                 dir_simple;     // 저장 위치(간결)
    std::vector<std::string>    dir_dependent;  // 의존성 위치
    std::string                 name;           // 악상 기호 이름
    cv::Mat                     img;            // 악상 기호 이미지
    
    // 악상 기호 config
    int         x;          // 중심점 x
    int         y;          // 중심점 y
    double      rotate;     // 회전
    double      scale;      // 확대, 축소
    
    // 생성자
    MusicalSymbol() {
        dir             = "";
        dir_simple      = "";
        dir_dependent   = std::vector<std::string>();
        name            = "";
        img             = cv::Mat();
        x               = 0;
        y               = 0;
        scale           = 0.0;
        rotate          = 0.0;
    }
    
    // 복사 생성자 (깊은 복사)
    MusicalSymbol(const MusicalSymbol& other) {
        dir             = other.dir;
        dir_simple      = other.dir_simple;
        dir_dependent   = other.dir_dependent;
        name            = other.name;
        img             = other.img.clone();
        x               = other.x;
        y               = other.y;
        scale           = other.scale;
        rotate          = other.rotate;
    }
    
    // 복사 대입 연산자 (깊은 복사)
    MusicalSymbol& operator=(const MusicalSymbol& other) {
        if (this != &other) {
            dir             = other.dir;
            dir_simple      = other.dir_simple;
            dir_dependent   = other.dir_dependent;
            name            = other.name;
            img             = other.img.clone();
            x               = other.x;
            y               = other.y;
            scale           = other.scale;
            rotate          = other.rotate;
        }
        return *this;
    }
    
    // == 연산자 함수
    bool operator==(const MusicalSymbol& other) {
        if (dir!="" && other.dir!="")   return dir == other.dir;
        else                            return dir_simple == other.dir_simple;
    }
};

class Note
{
    // 이미지 크기
    int img_w=256;
    int img_h=512;
    
    // 오선지 간격
    int pad = 25;
    
    // 주소
    std::string                 dir_ds;         // 데이터셋 위치
    std::string                 dir_ds_config;  // 데이터셋 config 파일 위치
    
    // 데이터셋 : 완성형
    std::vector<MusicalSymbol>  ds;             // 불러온 데이터
    std::vector<std::string>    ds_list;        // 불러온 데이터 목록(주소)
    
    // 데이터셋 : 조합형
    std::vector<MusicalSymbol>  ds_piece;       // 불러온 데이터
    std::vector<std::string>    ds_piece_list;  // 불러온 데이터 목록(주소)
    
    // 그리기 목록
    std::vector<MusicalSymbol>  draw_list;      // 그릴 목록

    
    // 초기화
    void load_ds();
    void load_ds_piece();
    
    // 유틸리티 : 계산
    std::vector<std::string>    get_dependent(std::string dir);     // 의존성 위치 알아내기
    std::string                 get_name(std::string dir);          // 악상 기호 이름 알아내기
    std::vector<std::string>    get_config(std::string line);       // config 정보 가져오기
    std::vector<MusicalSymbol>  get_selectable(const std::vector<MusicalSymbol>& ls_ms_1,
                                               const std::vector<MusicalSymbol>& ls_ms_2); // 선택 가능한 악상기호 구하기
    
    // 유틸리티 : 계산
    void do_pruning(std::vector<MusicalSymbol>& ls_ms_1, MusicalSymbol& ms);    // 가지치기
    void do_pruning(std::vector<std::string>& ls1, std::string dir);            // 가지치기
    
    // 유틸리티 : 이미지
    void restore_image(std::string dir);    // 이미지 재생성
    void remove_padding(std::string dir);   // 이미지 여백 제거
    
    // 유틸리티 : 문자열
    std::vector<std::string>    my_split(std::string s1, std::string s2);   // 문자열 분리
    std::string                 my_trim(std::string s1);                    // 문자열 앞,뒤 공백 제거
    std::string                 my_lower(std::string s1);                   // 문자열 소문자로 변환
    
    // 유틸리티 : 파일
    std::string my_grep(std::string dir, std::string s);        // grep 명령어 구현
    
    // 유틸리티 : 변환
    std::string transform_to_write(std::filesystem::path dir);  // 쓰기 변환
    std::string transform_to_write(std::string dir);            // 쓰기 변환
    std::string transform_to_read(std::string dir);             // 읽기 변환
    
    // 유틸리티 : 생성
    std::string make_config(std::string dir);                   // config 값 생성
    
    // 유틸리티 : 백업
    void save_config();                                         // config 값 저장(업데이트)
    
    // 행렬 연산
    cv::Mat attach_mat(const cv::Mat& img, const cv::Mat& img_sub, int x, int y);   // 행렬 합성
    cv::Mat rotate_mat(const cv::Mat& img, double degree, int x, int y);            // 행렬 회전
    cv::Mat scale_mat(const cv::Mat& img, double scale);                            // 행렬 확대
    cv::Mat symmetry_mat(const cv::Mat& img, std::string symmetry);                 // 행렬 대칭
    
    // 그리기
    cv::Mat draw();
    cv::Mat draw_symbols(const cv::Mat& img,
                         const cv::Mat& img_symbol,
                         std::string img_config,
                         bool auxiliary_line=false);
    
public:
    // 생성자
    Note(std::string dataset_dir="symbol_dataset"){
        using namespace std;
        using namespace cv;
        using namespace std::filesystem;
        
        // 데이터셋 주소 생성
        dir_ds = dataset_dir;
        
        // config 파일, 주소 생성
        dir_ds_config = (path(dir_ds)/path("symbol_dataset_config.txt")).string();
        if (!exists(dir_ds_config)){
            fstream(dir_ds_config, ios::out|ios::app).close();
        }
        
        // 초기화 단계
        load_ds();
        load_ds_piece();
        
        // config 백업
        save_config();
    }
    
    // 사용 : 그릴 음표 설정
    void set(std::string type, std::string name);
    
    // 사용 : 그림 확인
    void    save_as_img(std::string file_name);     // 그린 내용을 file_name 이름으로 저장
    void    show();                                 // 그린 내용을 화면에 보여주기
    cv::Mat cv_Mat();                               // 그린 내용을 cv::Mat() 형식으로 반환
};


}
#endif
