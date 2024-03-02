#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <filesystem>           // 디렉토리 주소 생성 관련
#include <regex>                // 정규 표현식 관련
#include <cctype>               // 소문자 변환
#include <string>               // 스트링 변환
#include <opencv2/opencv.hpp>

#pragma once
#ifndef MSIG_H
#define MSIG_H
namespace msig
{


class Note{
private:
    // dataset 저장 위치
    std::string                 ds_dir;
    
    // dataset 저장 정보
    std::vector<std::string>    ds_cons = {
        // 참고 : https://en.wikipedia.org/wiki/List_of_musical_symbols#Lines
        
        // line
        "ln_staff",            // 오선
        "ln_ledger",           // 오선 위 아래 추가적 선
        "ln_bar",              // 단일 바
        "ln_bar-double",       // 이중 바
        "ln_bar-double-bold",  // 이중 바(볼드)
        "ln_bar-dotted",       // 점선 바
        
        // clef
        "cf_g",              // 높은음자리표
        "cf_g-plus-8",       // 높은음자리표 +8 옥타브
        "cf_g-plus-15",      // 높은음자리표 +15 옥타브
        "cf_g-minus-8",      // 높은음자리표 -8 옥타브
        "cf_g-minus-15",     // 높은음자리표 -15 옥타브
        "cf_g-double",
        "cf_c-alto",
        "cf_c-tenor",
        "cf_f",
        
        // note
        "nt_1",          // 온음표
        "nt_2",          // 2분음표
        "nt_4",          // 4분음표
        "nt_8",          // ...
        "nt_16",         //
        "nt_32",         //
        "nt_64",         //
        "nt_beam",       // 연속적인 꼬리
        "nt_ghost",      // 비트 음표(?)
        
        "nt_head-2",     // 2분음표 머리
        "nt_head-4",     // 4분음표 머리
        "nt_tail-8",
        "nt_tail-16",
        "nt_tail-32",
        "nt_tail-64",
        "nt_dot",

        // rest
        "rt_1",
        "rt_2",
        "rt_4",
        "rt_8",
        "rt_16",
        "rt_32",
        "rt_64",
        "rt_measure-rest",
        
        // breaks
        "br_brath-mark",
        "br_caesura",
        
        // accidental-signatures
        "as_flat",
        "as_flat-double",
        "as_natural",
        "as_sharp",
        "as_sharp-double",
        
        // key-signatures
        "ks_flat-1",
        "ks_flat-2",
        "ks_flat-3",
        "ks_flat-4",
        "ks_flat-5",
        "ks_flat-6",
        "ks_flat-7",
        "ks_sharp-1",
        "ks_sharp-2",
        "ks_sharp-3",
        "ks_sharp-4",
        "ks_sharp-5",
        "ks_sharp-6",
        "ks_sharp-7",
        
        // time-signatures
        "ts_2",
        "ts_3",
        "ts_4",
        "ts_5",
        "ts_6",
        "ts_7",
        "ts_8",
        "ts_9",
        "ts_12",
        "ts_common",
        "ts_cut",
        
        // note-relationships
        "nr_tie",
        "nr_slur",
        "nr_glissando",
        "nr_tuplet",
        "nr_arpeggio",
        
        // dynamics
        "dy_pianississimo",
        "dy_pianissimo",
        "dy_piano",
        "dy_mezzo-piano",
        "dy_mezzo-forte",
        "dy_forte",
        "dy_fortissimo",
        "dy_fortississimo",
        "dy_sforzando",
        "dy_fortepiano",
        "dy_crescendo",
        "dy_decrescendo",
        
        // articulation-marks
        "am_staccato",
        "am_staccatissimo",
        "am_tenuto",
        "am_fermata",
        "am_accent",
        "am_marcato",
        
        // ornaments
        "or_tremolo-8",
        "or_tremolo-16",
        "or_tremolo-32",
        "or_tremolo-64",
        "or_tril",
        "or_tril-plus",
        "or_mordent-upper",
        "or_mordent-lower",
        "or_turn",
        "or_turn-inverted",
        "or_turn-slash",
        "or_appoggiatura",
        "or_acciaccatura",
        
        // octave-signs
        "os_8-up",
        "os_8-down",
        "os_15-up",
        "os_15-down",
        
        // repetitions
        "rp_repeat-start",
        "rp_repeat-end",
        "rp_simile-start",
        "rp_simile-end",
        "rp_volta-1",
        "rp_volta-2",
        "rp_da-capo",
        "rp_dal-segno",
        "rp_segno",
        "rp_coda",
    };
    
    // 이미지 크기
    int img_w=256;
    int img_h=512;
    
    // 오선지 간격
    int pad = 25;
    
    // 이미지 데이터
    std::map<std::string, cv::Mat>      img_symbols;    // 악상기호 이미지들
    std::map<std::string, std::string>  img_configs;    // 악상기호 이미지 조정 값
    std::map<std::string, std::string>  draw_list;      // 그릴 내용들
    
    // 초기화
    void load_img_symbols();    // 악상 기호 이미지들 불러오기
    void load_img_configs();    // 악상 기호 이미지설정값들 불러오기
    
    // 그리기
    cv::Mat draw();                                     // draw_list에 있는 내용들 그리기
    cv::Mat draw_symbols(const cv::Mat& img,
                         const cv::Mat& img_symbol,
                         std::string img_config,
                         bool auxiliary_line=false);    // 악상 기호 그리기
    
    // 유틸리티
    void restore_image(std::string dir);    // 이미지 재생성
    void remove_padding(std::string dir);   // 이미지 여백 제거
    void integrity_symbols();               // symbols 무결성 유지 함수
    void integrity_configs();               // configs 무결성 유지 함수
    void save_symbol_dataset_config();      // symbol_dataset_config 저장
    void string_trim(std::string& str);     // 문자열 앞,뒤 공백 제거
    void string_to_lower(std::string& str); // 문자열 소문자로 변환
    void make_config(std::string symbol_name, std::string& symbol_config);  // config 값 생성
    std::vector<std::string> split(std::string s, std::string c);  // 문자열 분리
    
    // 행렬 연산
    cv::Mat attach_mat(const cv::Mat& img, const cv::Mat& img_sub, int x, int y);   // 행렬 합성
    cv::Mat rotate_mat(const cv::Mat& img, double degree, int x, int y);            // 행렬 회전
    cv::Mat scale_mat(const cv::Mat& img, double scale);                            // 행렬 확대
    cv::Mat symmetry_mat(const cv::Mat& img, std::string symmetry);                 // 행렬 대칭
    
public:
    // 생성자
    Note(std::string dataset_dir="symbol_dataset"){
        // symbol dataset 디렉토리 설정
        ds_dir = dataset_dir;
        
        // 초기화 단계
        load_img_symbols();
        load_img_configs();
        
        // 무결성 유지
        integrity_symbols();
        integrity_configs();
        
        // config 정보 저장
        save_symbol_dataset_config();
    }
    
    // 사용 : 그릴 음표 설정
    void set(std::string note,
             std::string relationship="",
             std::string dynamic="",
             std::string articulations_mark="",
             std::string ornament="",
             std::string octavesign="",
             std::string kegetition_and_codas="");
    
    // 사용 : 그림 확인
    void save_as_img(std::string file_name);    // 그린 내용을 file_name 이름으로 저장
    void show();                                // 그린 내용을 화면에 보여주기
    cv::Mat cv_Mat();                           // 그린 내용을 cv::Mat() 형식으로 반환
};


}
#endif
