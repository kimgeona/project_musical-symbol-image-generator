#include <dst.hpp>

namespace dst {


// 생성자
DSTree::DSTree() {
    using namespace std;
    using namespace std::filesystem;
    
    // 초기화
    this->root_dir          = path("");             // 폴더 경로
    this->target_extension  = vector<string>();     // 조사 대상 확장자 목록
    this->pre               = path("");             // 현재 위치
}
DSTree::DSTree(std::string root_dir, std::vector<std::string> target_extension) {
    using namespace std;
    using namespace std::filesystem;
    
    // 초기화
    this->root_dir          = path(root_dir);       // 폴더 경로
    this->target_extension  = target_extension;     // 조사 대상 확장자 목록
    this->pre               = path(root_dir);       // 현재 위치
    
    // 폴더 조사, tree에 DSTree_Node 추가
    for (auto& d : recursive_directory_iterator(path(this->root_dir))){
        // DSTree 추가 조건들
        if (!is_regular_file(d.path())) continue; // 파일이어야 함
        if (find(this->target_extension.begin(), this->target_extension.end(), d.path().extension())==this->target_extension.end()) continue; // 지정된 확장자만
        if (d.path().string().find("@")==string::npos && d.path().string().find("#")==string::npos) continue; // @,# 문자 포함
        // DSTree에 추가
        this->tree[d.path()] = DSTree_Node(d.path().string());
    }
}

// 알고리즘 : 확인
std::vector<std::filesystem::path>  DSTree::get_available_folder(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (!exists(dir) && is_directory(dir)) return vector<path>();
    
    vector<path> list_dir;
    for (auto& d : directory_iterator(dir)){
        // 조건 확인(디렉토리인지, @# 문자 포함, 하위 파일 사용가능여부)
        if (!is_directory(d.path())) continue;
        if (d.path().string().find("-@")==string::npos && d.path().string().find("-#")==string::npos) continue;
        if (get_available_file(d.path()).empty()) continue;
        //
        list_dir.push_back(d.path());
    }
    return list_dir;
}
std::vector<std::filesystem::path>  DSTree::get_available_file(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (!exists(dir) && is_directory(dir)) return vector<path>();
    
    vector<path> list_file;
    for (auto& d : directory_iterator(dir)){
        // 조건 확인(파일인지, 지정된확장자, 사용가능여부)
        if (!is_regular_file(d.path()))
        if (find(this->target_extension.begin(), this->target_extension.end(), d.path().extension())==this->target_extension.end()) continue;
        if (!tree[d.path()].available) continue;
        //
        list_file.push_back(d.path());
    }
    return list_file;
}
std::vector<std::filesystem::path>  DSTree::get_available(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (!exists(dir) && is_directory(dir)) return vector<path>();
    
    // 선택 가능한 폴더 안에, 선택 가능한 파일들 push_back()
    vector<path> list_file;
    for (auto& d : get_available_folder(dir))
        for (auto& f : get_available_file(d))
            list_file.push_back(f);
    return list_file;
}

// 알고리즘 : pre 이동
int                                 DSTree::select(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 파일인지 확인
    if (exists(dir) && is_regular_file(dir)) return 1;
    
    
    // 선택 가능한 파일들 불러오기
    vector<path> list_file = get_available(this->pre);
    
    // 선택 가능한 파일이 없는 경우
    if (list_file.empty()) return -1;
    
    // 파일을 찾을 수 없는 경우
    if (find(list_file.begin(), list_file.end(), dir)==list_file.end()){
        return 1;
    }
    // 파일을 찾은 경우
    else this->pre = dir.parent_path();
    
    
    // @ 의존성 가지치기 실행 : 같은 깊이의 파일 모두 available=false
    if (dir.parent_path().string().find("-@")!=string::npos){
        for (auto& p : directory_iterator(dir.parent_path().parent_path())){
            pruning(p.path());
        }
    }
    // # 의존성 가지치기 실행 : 현재 디렉토리의 파일만 모두 available=false
    if (dir.parent_path().string().find("-#")!=string::npos){
        pruning(dir.parent_path());
    }
    
    // pre 위치 새로고침
    pre_refresh();
    
    return 0;
}
void                                DSTree::pre_refresh(){
    using namespace std;
    using namespace std::filesystem;
    
    // root_dir 탈출 방지
    if (this->pre==this->root_dir) return;
    
    // pre 새로 고쳐야 하는지 확인
    vector<path>list_file = get_available(this->pre);
    
    // pre 한단계 위로 빠져나가기
    if (list_file.empty()){
        pre = pre.parent_path();
        pre_refresh();
    }
}

// 알고리즘 : 데이터 변경
void                                DSTree::pruning(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (exists(dir) && is_directory(dir)) return;
    
    // 현재 디렉토리 내 파일 알아내기
    for (auto& f : directory_iterator(dir)){
        // 조건 확인(파일, 지정된확장자)
        if (!is_regular_file(f.path())) continue;
        if (find(this->target_extension.begin(), this->target_extension.end(), f.path().extension())==this->target_extension.end()) continue;
        //
        tree[f.path()].available = false;
    }
}


}
