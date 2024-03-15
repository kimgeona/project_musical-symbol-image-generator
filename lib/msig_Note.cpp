#include <msig_Note.hpp>

namespace msig
{


// msig_Note.cpp
Note::Note(){
    dir_ds          = std::filesystem::path();
    dir_ds_config   = std::filesystem::path();
    dir_ds_complete = std::filesystem::path();
    dir_ds_piece    = std::filesystem::path();
    symbol_selector = dst::DSTree();
    ds_complete     = std::vector<MusicalSymbol>();
    ds_piece        = std::vector<MusicalSymbol>();
    draw_list       = std::vector<MusicalSymbol>();
}
Note::Note(std::string dataset_dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // string 주소 -> path 주소
    path d(dataset_dir);
    
    // 아래 과정에서 이상이 있을 경우
    if (init_dir(path(d))       ||
        init_symbol_selector()  ||
        init_ds_complete()      ||
        init_ds_piece())
    {
        cout << "Note:" << d.string() << " 을(를) 통하여 초기화하는 과정에서 문제가 있습니다." << endl;
        dir_ds          = std::filesystem::path();
        dir_ds_config   = std::filesystem::path();
        dir_ds_complete = std::filesystem::path();
        dir_ds_piece    = std::filesystem::path();
        symbol_selector = dst::DSTree();
        ds_complete     = std::vector<MusicalSymbol>();
        ds_piece        = std::vector<MusicalSymbol>();
        draw_list       = std::vector<MusicalSymbol>();
    }
}


}
