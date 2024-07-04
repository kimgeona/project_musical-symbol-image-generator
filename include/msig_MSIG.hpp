#pragma once
#ifndef msig_MSIG_hpp
#define msig_MSIG_hpp

// c++17
#include <iostream>     // 입출력 스트림
#include <fstream>      // 파일 입출력
#include <filesystem>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>      // system()

// 나의 라이브러리
#include <msig_Algorithm_DST.hpp>
#include <msig_Algorithm_MSPA.hpp>
#include <msig_Canvas.hpp>
#include <msig_Matrix.hpp>
#include <msig_MusicalSymbol.hpp>
#include <msig_Util.hpp>

// 프로그램 버전
#define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))

namespace msig
{


class MSIG
{
private:
    // 데이터셋 주소
    std::filesystem::path dir_dataset;          // 기존 데이터셋
    std::filesystem::path dir_dataset_config;   // 기존 데이터셋 config 파일
    std::filesystem::path dir_new_dataset;      // 새로운 데이터셋
    std::filesystem::path dir_new_dataset_csv;  // 새로운 데이터셋 label 파일
    
    // 자원 보호
    std::mutex      mtx_data;   // 데이터 접근 뮤텍스
    std::mutex      mtx_io;     // 입출력 접근 뮤텍스
    std::mutex      mtx_count;  // 개수 세기 뮤텍스
    
    // 스레드
    unsigned int                number_of_thread;   // 사용 가능 스레드 개수
    std::vector<std::thread>    threads;            // 스레드
    
    // 캔버스
    std::vector<msig::Canvas>   canvases;   // 캔버스
    
    // CSV 파일
    std::fstream csv; // CSV 파일
    
    // 알고리즘
    msig::DST dst;
    std::queue<std::vector<std::filesystem::path>> all_combination;     // 악상 기호 조합 저장 변수
    std::vector<std::string> labels;                                    // 레이블들
    
public:
    // 생성자
    MSIG(std::filesystem::path default_dataset_dir = std::filesystem::path("symbol-dataset"), std::filesystem::path new_dataset_dir = std::filesystem::path(std::string("MusicalSymbol-v.") + MSIG_VERSION));
    
    // 소멸자
    ~MSIG();
    
    // 초기화 및 정보 출력
    void print_info();      // MSIG 시작 문구 출력
    void print_platform();  // 플랫폼 출력
    
    // 준비
    int  prepare_default_dataset(); // 데이터셋 준비
    int  prepare_DST();             // DST 알고리즘 준비
    int  prepare_Canvas();          // Canvas 준비
    int  prepare_csv();             // CSV 파일 생성
    
    // 데이터셋 생성
    int  making_dataset();
    
    // csv 파일
    long count_csv;
    int  append_to_csv(std::string data);
    
    // 스레드 처리
    void thread_processing(int canvas_number, const long data_size); // 스레드 함수
    std::filesystem::path   naming(); // 이미지 이름 생성 함수
    std::string             labeling(std::string name, const std::vector<std::filesystem::path>& v); // 레이블 생성 함수
};


}

#endif /* msig_MSIG_hpp */
