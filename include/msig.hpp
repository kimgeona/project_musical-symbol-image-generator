#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <filesystem>           // 디렉토리 주소 생성
#include <regex>                // 정규 표현식 라이브러리
#include <cctype>               // 소문자 변환
#include <opencv2/opencv.hpp>

#pragma once
#ifndef MSIG_H
#define MSIG_H
namespace msig
{


class Note{
private:
    // 프로그램 저장 위치
    std::string dataset_dir;
    
    // 생성할 이미지 크기
    int width=256, height=512;
    
    // 저장 정보 : "폴더명", "이미지 이름"
    std::vector<std::string> dirs = {
        // 참고 : https://en.wikipedia.org/wiki/List_of_musical_symbols#Lines
        
        // line
        "ln", "staff",            // 오선
        "ln", "ledger",           // 오선 위 아래 추가적 선
        "ln", "bar",              // 단일 바
        "ln", "bar_double",       // 이중 바
        "ln", "bar_double_bold",  // 이중 바(볼드)
        "ln", "bar_dotted",       // 점선 바
        
        // clef
        "cf", "g",              // 높은음자리표
        "cf", "g_plus_8",       // 높은음자리표 +8 옥타브
        "cf", "g_plus_15",      // 높은음자리표 +15 옥타브
        "cf", "g_minus_8",      // 높은음자리표 -8 옥타브
        "cf", "g_minus_15",     // 높은음자리표 -15 옥타브
        "cf", "g_double",
        "cf", "c_alto",
        "cf", "c_tenor",
        "cf", "f",
        
        // note
        "nt", "1",          // 온음표
        "nt", "2",          // 2분음표
        "nt", "4",          // 4분음표
        "nt", "8",          // ...
        "nt", "16",         //
        "nt", "32",         //
        "nt", "64",         //
        "nt", "beam",       // 연속적인 꼬리
        "nt", "ghost",      // 비트 음표(?)
        
        "nt", "head_2",     // 2분음표 머리
        "nt", "head_4",     // 4분음표 머리
        "nt", "tail_8",
        "nt", "tail_16",
        "nt", "tail_32",
        "nt", "tail_64",
        "nt", "dot",

        // rest
        "rt", "1",
        "rt", "2",
        "rt", "4",
        "rt", "8",
        "rt", "16",
        "rt", "32",
        "rt", "64",
        "rt", "measure_rest",
        
        // breaks
        "br", "brath_mark",
        "br", "caesura",
        
        // accidental-signatures
        "as", "flat",
        "as", "flat_double",
        "as", "natural",
        "as", "sharp",
        "as", "sharp_double",
        
        // key-signatures
        "ks", "flat_1",
        "ks", "flat_2",
        "ks", "flat_3",
        "ks", "flat_4",
        "ks", "flat_5",
        "ks", "flat_6",
        "ks", "flat_7",
        "ks", "sharp_1",
        "ks", "sharp_2",
        "ks", "sharp_3",
        "ks", "sharp_4",
        "ks", "sharp_5",
        "ks", "sharp_6",
        "ks", "sharp_7",
        
        // time-signatures
        "ts", "2",
        "ts", "3",
        "ts", "4",
        "ts", "5",
        "ts", "6",
        "ts", "7",
        "ts", "8",
        "ts", "9",
        "ts", "12",
        "ts", "common",
        "ts", "cut",
        
        // note-relationships
        "nr", "tie",
        "nr", "slur",
        "nr", "glissando",
        "nr", "tuplet",
        "nr", "arpeggio",
        
        // dynamics
        "dy", "pianississimo",
        "dy", "pianissimo",
        "dy", "piano",
        "dy", "mezzo_piano",
        "dy", "mezzo_forte",
        "dy", "forte",
        "dy", "fortissimo",
        "dy", "fortississimo",
        "dy", "sforzando",
        "dy", "fortepiano",
        "dy", "crescendo",
        "dy", "decrescendo",
        
        // articulation-marks
        "am", "staccato",
        "am", "staccatissimo",
        "am", "tenuto",
        "am", "fermata",
        "am", "accent",
        "am", "marcato",
        
        // ornaments
        "or", "tremolo_8",
        "or", "tremolo_16",
        "or", "tremolo_32",
        "or", "tremolo_64",
        "or", "tril",
        "or", "tril_plus",
        "or", "mordent_upper",
        "or", "mordent_lower",
        "or", "turn",
        "or", "turn_inverted",
        "or", "turn_slash",
        "or", "appoggiatura",
        "or", "acciaccatura",
        
        // octave-signs
        "os", "8_up",
        "os", "8_down",
        "os", "15_up",
        "os", "15_down",
        
        // repetitions
        "rp", "repeat_start",
        "rp", "repeat_end",
        "rp", "simile_start",
        "rp", "simile_end",
        "rp", "volta_1",
        "rp", "volta_2",
        "rp", "da-capo",
        "rp", "dal-segno",
        "rp", "segno",
        "rp", "coda",
    };
    
    // note 데이터
    std::map<std::string, cv::Mat>      imgs;           // 불러온 이미지들
    std::map<std::string, std::string>  imgs_config;    // 불러온 이미지 조정 값
    std::map<std::string, std::string>  draw_list;      // 그릴 내용들
    
    // 초기화
    void load_imgs();
    void load_imgs_config();
    
    // 기타
    std::string make_config(std::string symbol_name);
    cv::Mat combine_mat(const cv::Mat& img, int x, int y, const cv::Mat& img_sub);
    cv::Mat rotate_mat(const cv::Mat& img, double degree);
    cv::Mat scale_mat(const cv::Mat& img, double scale);
    
    // 악보 그리기
    cv::Mat draw();
    
public:
    // 생성자
    Note(std::string dataset_dir="symbol_dataset"){
        // symbol dataset 디렉토리 설정
        this->dataset_dir = dataset_dir;
        // 초기화
        load_imgs();
        load_imgs_config();
    }
    
    // 그리기 내용 추가
    void add(std::string key, std::string config_symmetry="", std::string config_progress="");
    
    // 악보 이미지 생성 함수
    void save_as_img(std::string file_name);    // 그린 내용을 file_name 이름으로 저장
    void show();                                // 그린 내용을 화면에 보여주기
    cv::Mat cv_Mat();                           // 그린 내용을 cv::Mat() 형식으로 반환
};


}
#endif
