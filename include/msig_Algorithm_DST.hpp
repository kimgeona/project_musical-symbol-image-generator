#pragma once
#ifndef msig_Algorithm_DST_hpp
#define msig_Algorithm_DST_hpp

// c++17
#include <iostream>
#include <filesystem>
#include <vector>
#include <queue>
#include <random>
#include <map>
#include <set>
#include <algorithm>

// 나의 라이브러리
#include <msig_Util.hpp>

namespace msig
{


// Dependent Selection Tree File
class DSTFile
{
private:
    // 파일 상태
    bool state;
    
    // 파일 주소
    std::filesystem::path dir;
    
public:
    // 생성자
    DSTFile(){};
    DSTFile(const std::string& dir);
    DSTFile(const std::filesystem::path& dir);
    
    // bool() 연산자
    operator bool() const;
    
    // 상태 설정 함수
    void set_true();
    void set_false();
    
    // 정보 가져오기
    std::filesystem::path get_filename();
    std::filesystem::path get_parent(bool full_path=false);
    std::filesystem::path get_dir();
};

// Dependent Selection Tree Folder
class DSTFolder
{
private:
    // 폴더 주소
    std::filesystem::path dir;
    
    // 폴더 타입
    bool duplication;
    long duplication_count;
    
    // 연결 정보
    std::vector<DSTFolder>  folders;    // 하위 폴더들
    std::vector<DSTFile>    files;      // 저장되어 있는 파일들
    
public:
    // 생성자
    DSTFolder(){}
    DSTFolder(const std::string& dir, const std::vector<std::string>& folder_types, const std::vector<std::string>& file_types);
    DSTFolder(const std::filesystem::path& dir, const std::vector<std::string>& folder_types, const std::vector<std::string>& file_types);
    
    // bool() 연산자
    operator bool() const;
    
    // 상태 설정 함수
    void set_true(bool recursive = false);
    void set_false(bool recursive = false);
    void set_true(std::filesystem::path dir);
    void set_false(std::filesystem::path dir);
    void set_duplication_count();
    
    // 정보 가져오기
    bool get_duplication();
    int  get_files_count();
    int  get_folders_count();
    
    // 파일 이름 레이블 생성
    std::set<std::string> get_labels(bool recursive = false);
    
    // 파일 하나 뽑기
    std::vector<std::filesystem::path> pick();
    std::vector<std::filesystem::path> pick(long n);
};

// Dependent Selection Tree
class DST
{
private:
    // 시작 폴더
    DSTFolder root;
    
    // 랜덤
    std::mt19937                        gen;    // Mersenne Twister 엔진
    std::uniform_real_distribution<>    dis;    // 균등 분포
    
public:
    // 생성자
    DST(){}
    DST(const std::string& dataset_dir);
    
    // 처음 상태로 돌림
    void reset();
    
    // 특정 악상기호 활성화
    void set_true(bool recursive);
    void set_true(std::vector<std::filesystem::path> vp_folders);
    
    // 특정 악상기호 비활성화
    void set_false(bool recursive);
    void set_false(std::vector<std::filesystem::path> vp_folders);
    
    // 파일 이름들 레이블 가져오기
    std::vector<std::string> get_labels();
    
    // 가능한 모든 조합 구하기
    std::queue<std::vector<std::filesystem::path>> list(std::vector<double> rate);
};


}



#endif /* dst_hpp */
