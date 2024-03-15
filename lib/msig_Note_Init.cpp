#include <msig_Note.hpp>

namespace msig
{


// msig_Note_Init.cpp
int Note::init_dir(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // dir 존재 확인
    if (exists(dir) && is_directory(dir)){
        this->dir_ds = dir;
        
        path p_config   = dir/path("symbol_dataset_config.txt");
        path p_complete = dir/path("complete");
        path p_piece    = dir/path("piece");
        
        // symbol_dataset_config.txt 존재 확인
        if (exists(p_config) && is_regular_file(p_config)){
            this->dir_ds_config = p_config;
        }
        else {
            fstream(p_config.string(), ios::out|ios::app).close();
        }
        
        // complete 존재 확인
        if (exists(p_complete) && is_directory(p_complete)){
            this->dir_ds_config = p_config;
        }
        else {
            cout << "Note: " << p_complete.string() << "은(는) 존재하는 디렉토리가 아닙니다." << endl;
            return 1;
        }
        
        // piece 존재 확인
        if (exists(p_piece) && is_directory(p_piece)){
            this->dir_ds_piece = p_piece;
        }
        else {
            cout << "Note: " << p_complete.string() << "은(는) 존재하는 디렉토리가 아닙니다." << endl;
            return 1;
        }
        
        return 0;
    }
    else {
        cout << "Note: " << dir.string() << "은(는) 존재하는 디렉토리가 아닙니다." << endl;
        return 1;
    }
}
int Note::init_symbol_selector(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 의존적 선택 트리 생성
    this->symbol_selector = dst::DSTree(this->dir_ds_complete, {".png"});
}
int Note::init_ds_complete(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 불러올 악상기호 이미지 목록 구하기
    vector<path> list_png = this->symbol_selector.get_all_files();
    
    // 악상기호 불러오기
    for (auto& p : list_png){
        this->ds_complete.push_back(MusicalSymbol(p, this->dir_ds_config));
        save_config();
    }
}
int Note::init_ds_piece(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 불러올 악상기호 이미지 목록 구하기
    vector<path> list_png;
    for (auto& p : recursive_directory_iterator(this->dir_ds_piece)){
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png")
            list_png.push_back(p.path());
    }
    
    // 악상기호 불러오기
    for (auto& p : list_png){
        this->ds_piece.push_back(MusicalSymbol(p, this->dir_ds_config));
        save_config();
    }
}


}
