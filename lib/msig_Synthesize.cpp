#include <msig_Synthesize.hpp>

namespace msig
{


// 빈 생성자
Note::Note(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    this->dir_ds            = path();
    this->dir_ds_config     = path();
    this->dir_ds_complete   = path();
    this->dir_ds_piece      = path();
    this->symbol_selector   = DSTree();
    this->ds_complete       = map<path, MusicalSymbol>();
    this->ds_piece          = map<path, MusicalSymbol>();
    this->draw_list         = vector<MusicalSymbol>();
}


// 기본 사용 생성자
Note::Note(std::filesystem::path dataset_dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 아래 과정에서 이상이 있을 경우
    if (init_dir(dataset_dir)   ||
        init_symbol_selector()  ||
        init_ds_complete()      ||
        init_ds_piece())
    {
        cout << "Note: " << dataset_dir.string() << " 을(를) 통하여 초기화하는 과정에서 문제가 있습니다." << endl;
        this->dir_ds            = path();
        this->dir_ds_config     = path();
        this->dir_ds_complete   = path();
        this->dir_ds_piece      = path();
        this->symbol_selector   = DSTree();
        this->ds_complete       = map<path, MusicalSymbol>();
        this->ds_piece          = map<path, MusicalSymbol>();
        this->draw_list         = vector<MusicalSymbol>();
    }
}


}
