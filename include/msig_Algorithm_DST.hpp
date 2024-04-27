#pragma once
#ifndef msig_Algorithm_DST_hpp
#define msig_Algorithm_DST_hpp

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>


namespace msig {


// Dependent Selection Node
class DSNode
{
public:
    
    // 변수
    std::filesystem::path   dir;        // 파일 경로
    bool                    available;  // 해당 파일 사용가능 여부
    
    // 생성자
    DSNode(){}
    DSNode(std::string dir){
        this->dir = std::filesystem::path();
        this->available = true;
    }
    
    // 연산자
    bool operator==(const DSNode& other) const {
        return ((this->dir==other.dir) && (this->available==other.available));
    }
};


// Dependent Selection Tree
class DSTree
{
    // 트리 정보
    std::filesystem::path       root_dir;   // 최상위 폴더
    std::vector<std::string>    target;     // 대상 확장자 목록
    
    // 불러온 파일 목록
    std::map<std::filesystem::path, DSNode> nodes;
    
    // 현재 위치
    std::filesystem::path pre;
    
public:
    // 생성자
    DSTree();                                                                   // 기본 생성자
    DSTree(std::string root_dir, std::vector<std::string> target_extension);    // 생성자

    // 연산자
    bool operator==(const DSTree& other) const;                                 // 비교 연산자 함수
    
    // 확인 함수
    inline int check_file(std::filesystem::path p, bool available_check=false);         // 대상 파일인지 확인
    inline int check_directory(std::filesystem::path p, bool available_check=false);    // 대상 폴더인지 확인
    
    // 목록 구하기 함수
    std::vector<std::filesystem::path>  get_files(std::filesystem::path dir);       // 선택 가능 파일 구하기
    std::vector<std::filesystem::path>  get_folders(std::filesystem::path dir);     // 선택 가능 디렉토리 구하기
    std::vector<std::filesystem::path>  get(std::filesystem::path dir);             // 선택 가능한 파일들 구하기

private:
    // 수정 함수
    void pruning(std::filesystem::path dir);    // 가지치기
    void grafting(std::filesystem::path dir);   // 가지치기 되돌리기
    void pre_refresh();                         // pre 위치 계산
    
public:
    // 사용자
    int                                 select(std::filesystem::path dir);  // 파일 선택
    int                                 selectable();                       // 파일 선택 가능한지 확인
    std::vector<std::filesystem::path>  get();                              // 현재 선택 가능한 파일 목록 구하기
    void                                reset();                            // 선택 초기화
    void                                state(std::vector<std::filesystem::path> list, bool available_state); // list에 명시된 것들만 available 또는 disavailable 시킴
};


}



#endif /* dst_hpp */
