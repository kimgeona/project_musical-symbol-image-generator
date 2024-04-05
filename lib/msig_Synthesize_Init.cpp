#include <msig_Synthesize.hpp>

namespace msig
{


// 주소 초기화
int Note::init_dir(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // dir 존재 확인
    if (exists(dir) && is_directory(dir))   this->dir_ds = dir;
    else                                    return -1;
    
    // dir_ds_config, dir_ds_complete, dir_ds_piece 주소 생성
    path p_config   = dir/path("symbol_dataset_config.txt");
    path p_complete = dir/path("complete");
    path p_piece    = dir/path("piece");
    
    
    // dir_ds_config 존재 확인
    if (exists(p_config) && is_regular_file(p_config)) this->dir_ds_config = p_config;
    else {
        // 없으면 생성
        fstream(p_config.string(), ios::out|ios::app).close();
        this->dir_ds_config = p_config;
    }
    
    // dir_ds_complete 존재 확인
    if (exists(p_complete) && is_directory(p_complete)) this->dir_ds_complete = p_complete;
    else                                                return -1;
    
    // dir_ds_piece 존재 확인
    if (exists(p_piece) && is_directory(p_piece))   this->dir_ds_piece = p_piece;
    else                                            return -1;
    
    return 0;
}


// 의존적 선택 트리 알고리즘 초기화
int Note::init_symbol_selector(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 의존적 선택 트리 생성
    this->symbol_selector = DSTree(this->dir_ds_complete.string(), {".png"});
    
    // 생성한 트리 상태 체크
    if (this->symbol_selector==DSTree())    return -1;  // 생성 안됨
    else                                    return 0;   // 잘 생성됨
}


// 완성형 데이터셋 불러오기
int Note::init_ds_complete(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 불러올 (완성형) 악상기호 이미지 목록 구하기
    vector<path> list_png = this->symbol_selector.get_all_files();
    
    // 악상기호 불러오기
    for (auto& p : list_png){
        // 악상 기호 생성
        MusicalSymbol ms(p, this->dir_ds_config);
        // 악상 기호 상태 체크
        if (ms.status){
            cout << "악상 기호 : " << p << "가 로드되지 않음." << endl;
            continue;
        }
        // 악상 기호 추가
        this->ds_complete[p] = ms;
    }
    return 0;
}


// 조합형 데이터셋 불러오기
int Note::init_ds_piece(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 불러올 (조합형) 악상기호 이미지 목록 구하기
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
        if (ms.status){
            cout << "악상 기호 : " << p << "가 로드되지 않음." << endl;
            continue;
        }
        // 악상 기호 추가
        this->ds_complete[p] = ms;
    }
    return 0;
}


}
