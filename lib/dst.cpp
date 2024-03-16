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

// 알고리즘
int                                 DSTree::select_folder(std::filesystem::path name){
    using namespace std;
    using namespace std::filesystem;
    
    auto list_dir = get_available_folder(pre);
    
    // 폴더를 찾을 수 없는 경우
    if (find(list_dir.begin(), list_dir.end(), name)==list_dir.end()){
        return -1;
    }
    // 폴더를 찾은 경우
    else {
        pre = pre / name;
        return 0;
    }
}
int                                 DSTree::select_file(std::filesystem::path name){
    using namespace std;
    using namespace std::filesystem;
    
    auto list_file = get_available_file(pre);
    
    // 파일을 찾을 수 없는 경우
    if (find(list_file.begin(), list_file.end(), name)==list_file.end()){
        return -1;
    }
    // 파일을 찾은 경우
    else {
        // @ "의존성 가지치기 실행"
        if (tree[pre/name].dir.parent_path().filename().string().find("@")!=string::npos){
            // 자신 제거
            pruning(pre);
            // 나머지 제거
            for (auto& dir : recursive_directory_iterator(pre.parent_path())){
                if (!is_directory(dir.path())) continue;
                if (dir.path().string().find(pre.string())!=string::npos) continue;
                pruning(dir.path());
            }
            
        }
        // # "의존성 가지치기 실행"
        if (tree[pre/name].dir.parent_path().string().find("#")!=string::npos){
            // 자신 제거
            pruning(pre);
        }
        
        // pre 새로 고침
        pre_refresh();
        return 0;
    }
}
void                                DSTree::pre_refresh(){
    using namespace std;
    using namespace std::filesystem;
    
    // root_dir 탈출 방지
    if (pre==root_dir) return;
    
    // pre 새로 고침 여부
    bool refresh = true;
    
    // pre 새로 고쳐야 하는지 확인
    for (auto& path_dir : get_available_folder(pre, true))
        for (auto& path_file : get_available_file(path_dir, true))
            if (tree[path_file].available){
                refresh = false;
                break;
            }
    
    // pre 새로 고침
    if (refresh){
        pre = pre.parent_path();
        pre_refresh();
    }
}
void                                DSTree::pruning(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    // 현재 디렉토리 내 파일 알아내기
    for (auto& d : directory_iterator(dir)){
        // 조건 확인(파일, 지정된확장자)
        if (!is_regular_file(d.path())) continue;
        if (find(target_extension.begin(), target_extension.end(), d.path().extension())==target_extension.end()) continue;
        //
        tree[d.path()].available = false;
    }
}
std::vector<std::filesystem::path>  DSTree::get_available_folder(std::filesystem::path p, bool full_path){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_dir;
    for (auto& d : directory_iterator(p)){
        // 조건 확인(디렉토리인지, @# 문자 포함, 하위 파일 사용가능여부)
        if (!is_directory(d.path())) continue;
        if (d.path().string().find("@")==string::npos && d.path().string().find("#")==string::npos) continue;
        if (get_available_file(d.path()).empty()) continue;
        //
        if (full_path)  list_dir.push_back(d.path());
        else            list_dir.push_back(d.path().filename());
    }
    return list_dir;
}
std::vector<std::filesystem::path>  DSTree::get_available_file(std::filesystem::path p, bool full_path){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_file;
    for (auto& d : directory_iterator(p)){
        // 조건 확인(파일인지, 지정된확장자, 사용가능여부)
        if (!is_regular_file(d.path()))
        if (find(target_extension.begin(), target_extension.end(), d.path().extension())==target_extension.end()) continue;
        if (!tree[d.path()].available) continue;
        //
        if (full_path)  list_file.push_back(d.path());
        else            list_file.push_back(d.path().filename());
    }
    return list_file;
}


// 유틸리티 : API
std::vector<std::filesystem::path>  DSTree::get_all_files(){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_file;
    for (auto& e : tree)
        list_file.push_back(e.first);
    return list_file;
}
std::vector<std::filesystem::path>  DSTree::get_selectable(){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_file;
    for (auto& d : directory_iterator(pre)){
        // 조건 확인(디렉토리인지, @# 문자 포함, 하위 파일 사용가능여부)
        if (!is_directory(d.path())) continue;
        if (d.path().string().find("@")==string::npos && d.path().string().find("#")==string::npos) continue;
        // 선택 가능한 파일 구하기
        for (auto& f : get_available_file(d.path(), true))
            list_file.push_back(f);
    }
    return list_file;
}
int                                 DSTree::select(std::filesystem::path folder, std::filesystem::path file){
    using namespace std;
    using namespace std::filesystem;
    
    // 중간 종료
    // 현재 pre에서 @ 이름의 폴더가 없으면 건너뛰기 하도록(현재 pre 아래 있는 모든 파일 available==false 로 하면 됨. ==> .pruning())
    
    // 폴더 선택
    if(select_folder(path(folder))){
        return 1;
    }
    // 파일 선택
    if(select_file(path(file))){
        pre = pre.parent_path();
        return 2;
    }
    
    // 더이상 선택가능한 것이 없을 경우
    if (get_available_folder(pre).empty())  return -1;
    
    return 0;
}
int                                 DSTree::select(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    path p_folder = dir.parent_path();
    path p_file = dir.filename();
    
    return select(p_folder, p_file);
}
int                                 DSTree::select(std::string folder, std::string file){
    using namespace std;
    using namespace std::filesystem;
    
    // 중간 종료
    // 현재 pre에서 @ 이름의 폴더가 없으면 건너뛰기 하도록(현재 pre 아래 있는 모든 파일 available==false 로 하면 됨. ==> .pruning())
    
    // 폴더 선택
    if(select_folder(folder)){
        return 1;
    }
    // 파일 선택
    if(select_file(file)){
        pre = pre.parent_path();
        return 2;
    }
    
    // 더이상 선택가능한 것이 없을 경우
    if (get_available_folder(pre).empty())  return -1;
    
    return 0;
}
int                                 DSTree::select(std::string dir){
    using namespace std;
    using namespace std::filesystem;
    
    path p(dir);
    path p_folder = p.parent_path();
    path p_file = p.filename();
    
    return select(p_folder, p_file);
}
void                                DSTree::print_selectable(){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_dir = get_available_folder(pre, true);
    vector<path> list_file;
    
    size_t count_dir = list_dir.size();
    size_t count_file ;
    
    for (auto& p : list_dir){
        count_dir--;
        if (count_dir==0)   cout << "└── ";
        else                cout << "├── ";
        cout << p.filename().string() << endl;
        
        list_file = get_available_file(p, true);
        count_file = list_file.size();
        for (auto& f : list_file){
            count_file--;
            if (count_dir==0)   cout << "    ";
            else                cout << "│   ";
            if (count_file==0)  cout << "└── ";
            else                cout << "├── ";
            cout << f.filename().string() << endl;
        }
    }
}


}
