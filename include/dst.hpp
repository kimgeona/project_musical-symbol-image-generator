#pragma once
#ifndef dst_hpp
#define dst_hpp

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>


namespace dst {


// Dependent Selection Node
class DSTree_Node{
public:
    
    // 노드 구성
    std::filesystem::path   dir;        // 파일 경로 : ~start/line-@/staff.png
    bool                    available;  // 사용가능 여부
     
    // 생성자
    DSTree_Node() {}
    DSTree_Node(std::string dir){
        using namespace std;
        using namespace std::filesystem;
        
        // 초기화
        this->dir = path(dir);  // 기본 파일 주소
        this->available = true; // 사용 가능 여부
    }
};

// Dependent Selection Class
class DSTree{
public:
    
    // 변수
    std::filesystem::path       root_dir;           // 최상위 폴더
    std::vector<std::string>    target_extension;   // 대상 파일 확장자 목록
    
    // 자료구조
    std::map<std::filesystem::path, DSTree_Node>    tree;
    std::filesystem::path                           pre;
    
    // 알고리즘 : 확인
    std::vector<std::filesystem::path>  get_available_folder(std::filesystem::path dir);    // 해당 dir 내 선택 가능 디렉토리 가져오기
    std::vector<std::filesystem::path>  get_available_file(std::filesystem::path dir);      // 해당 dir 내 선택 가능한 파일 가져오기
    std::vector<std::filesystem::path>  get_available(std::filesystem::path dir);           // 해당 dir을 기준으로 선택 가능한 파일 가져오기
    
    // 알고리즘 : pre 이동
    int                                 select(std::filesystem::path dir);      // 파일 선택
    void                                pre_refresh();                          // pre 위치 계산
    
    // 알고리즘 : 데이터 변경
    void                                pruning(std::filesystem::path dir);     // 가지치기
    
public:
    // 생성자
    DSTree();
    DSTree(std::string root_dir, std::vector<std::string> target_extension);
    
    // API : 선택 가능 목록 확인
    std::vector<std::filesystem::path>  get_all_files();                    // 현재 트리상에 존재하는 모든 파일 가져오기
    std::vector<std::filesystem::path>  get_selectable();                   // 현재 선택 가능한 모든 파일 가져오기
    bool                                is_selectable();                    // 현재 선택 가능한지 여부 확인
    void                                print_selectable();                 // 선택 가능 목록 출력
    
    // API : 선택
    int select(std::string folder, std::string file);                       // 선택
    int select(std::string dir);                                            // 선택
    
    
};


}



#endif /* dst_hpp */
