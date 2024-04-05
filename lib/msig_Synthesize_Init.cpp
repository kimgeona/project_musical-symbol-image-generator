#include <msig_Synthesize.hpp>

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
            if (exists(p_config) && is_regular_file(p_config)){
                this->dir_ds_config = p_config;
            }
            else return 1;
        }
        
        // complete 존재 확인
        if (exists(p_complete) && is_directory(p_complete)){
            this->dir_ds_complete = p_complete;
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
            cout << "Note: " << p_piece.string() << "은(는) 존재하는 디렉토리가 아닙니다." << endl;
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
    this->symbol_selector = DSTree(this->dir_ds_complete.string(), {".png"});
    
    // 생성한 트리 상태 체크
    if (this->symbol_selector==DSTree())    return 1;   // 생성 안됨
    else                                    return 0;   // 잘 생성됨
}
int Note::init_ds(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    int error = 0;
    
    error |= init_ds_complete();
    error |= init_ds_piece();
    
    save_config(); // config 파일 저장
    
    if (error)  return 1;
    else        return 0;
}
int Note::init_ds_complete(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 불러올 악상기호 이미지 목록 구하기
    vector<path> list_png = this->symbol_selector.get_all_files();
    
    // 악상기호 불러오기
    for (auto& p : list_png){
        // 악상 기호 생성
        MusicalSymbol ms(p, this->dir_ds_config);
        // 악상 기호 상태 체크
        if (ms.status) continue;
        // 악상 기호 추가
        this->ds_complete[p] = ms;
        // config 내용 추가
        add_config();
    }
    return 0;
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
        // 악상 기호 생성
        MusicalSymbol ms(p, this->dir_ds_config);
        // 악상 기호 상태 체크
        if (ms.status) continue;
        // 악상 기호 추가
        this->ds_complete[p] = ms;
        // config 내용 추가
        add_config();
    }
    return 0;
}


}
