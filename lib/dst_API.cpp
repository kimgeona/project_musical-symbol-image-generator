#include <dst.hpp>

namespace dst {


// API : 선택 가능 목록 확인
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
    
    return get_available(pre);
}
void                                DSTree::print_selectable(){
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> list_dir = get_available_folder(pre);
    vector<path> list_file;
    
    size_t count_dir = list_dir.size();
    size_t count_file ;
    
    for (auto& p : list_dir){
        count_dir--;
        if (count_dir==0)   cout << "└── ";
        else                cout << "├── ";
        cout << p.filename().string() << endl;
        
        list_file = get_available_file(p);
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

// API : 선택
int                                 DSTree::select(std::string dir){
    using namespace std;
    using namespace std::filesystem;
    
    return select(path(dir));
}
int                                 DSTree::select(std::string folder, std::string file){
    using namespace std;
    using namespace std::filesystem;
    
    // 선택 가능한 목록 불러오기
    vector<path> list_file = get_available(this->pre);
    
    // 선택 가능한 목록이 없다면
    if (list_file.empty()) return -1;
    
    // 찾기
    for (auto& p : list_file){
        if (p.parent_path().filename().string()==folder && p.filename().string()==file){
            // 찾은 경우
            return select(p);
            break;
        }
    }

    // 찾지 못한 경우
    return 1;
}


}
