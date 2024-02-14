#include <map>
#include <opencv2/opencv.hpp>

#pragma once
#ifndef MSIG_H
#define MSIG_H
namespace msig
{


class Note{
private:
    // 초기화 함수
    void init_drawing_set();
    void init_drawing_regexp();
    void init_note_imgs();
    void init_note_imgs_config();
    
    // note 데이터
    std::map<std::string, std::string>  drawing_set;        // 악상 기호 세팅 값
    std::map<std::string, std::string>  drawing_regexp;     // 악상 기호 세팅 정규표현식
    std::map<std::string, cv::Mat>      note_imgs;          // 불러온 이미지들
    std::map<std::string, std::string>  note_imgs_config;   // 불러온 이미지 조정 값
    
    // 악보 그리기
    cv::Mat draw();
    
public:
    // 생성자
    Note(){
        init_drawing_set();
        init_drawing_regexp();
        init_note_imgs();
        init_note_imgs_config();
    }
    
    // note 데이터 설정
    void set(std::string key, std::string value);
    
    // 악보 이미지 생성 함수
    void save_as_img(std::string file_name);    // 그린 내용을 file_name 이름으로 저장
    void show();                                // 그린 내용을 화면에 보여주기
    cv::Mat cv_Mat();                           // 그린 내용을 cv::Mat() 형식으로 반환
};


}
#endif
