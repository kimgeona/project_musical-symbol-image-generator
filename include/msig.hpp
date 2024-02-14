#include <map>
#include <opencv2/opencv.hpp>

#pragma once
#ifndef MSIG_H
#define MSIG_H
namespace msig
{


class Note{
private:
    // 참고 : https://geumse.tistory.com/14
    // 참고 : https://www.masterclass.com/articles/music-ornaments-guide
    
    // 초기화 함수
    void init_data();               // 악상기호 정보 초기화
    void init_data_reg_exp();       // 악상기호 정규표현식 초기화
    void init_data_img();           // 악상기호 이미지 불러오기
    void init_data_img_center();    // 악상기호 중심좌표 설정
    
    // note 데이터
    std::map<std::string, std::string>  data;
    std::map<std::string, std::string>  data_reg_exp;
    std::map<std::string, cv::Mat>      data_img;
    std::map<std::string, std::string>  data_img_center;
    
    // 악보 그리기
    cv::Mat draw();
    
public:
    // 생성자
    Note(){
        init_data();
        init_data_reg_exp();
        init_data_img();
        init_data_img_center();
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
