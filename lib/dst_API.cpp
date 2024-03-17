#include <dst.hpp>

namespace dst {


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
