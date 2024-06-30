#include <msig_Canvas.hpp>

namespace msig
{


// 기본 생성자
Canvas::Canvas(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    this->width             = 0;
    this->height            = 0;
    this->dir_ds            = path();
    this->dir_ds_config     = path();
    this->dir_ds_complete   = path();
    this->dir_ds_piece      = path();
    this->ds_complete       = map<path, MusicalSymbol>();
    this->ds_piece          = map<path, MusicalSymbol>();
    this->draw_list         = vector<MusicalSymbol>();
}


// 기본 생성자
Canvas::Canvas(std::filesystem::path dataset_dir, int width, int height){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 아래 과정에서 이상이 있을 경우
    if (set_size(width, height) ||
        set_dataset_dirs(dataset_dir))
    {
        std::runtime_error("msig::Canvas::Canvas() : 캔버스를 생성하는 과정에서 문제가 있습니다.");
    }
}


// 복사 생성자
Canvas::Canvas(const Canvas& other){
    this->width             = other.width;
    this->height            = other.height;
    this->dir_ds            = other.dir_ds;
    this->dir_ds_config     = other.dir_ds_config;
    this->dir_ds_complete   = other.dir_ds_complete;
    this->dir_ds_piece      = other.dir_ds_piece;
    this->ds_complete       = other.ds_complete;
    this->ds_piece          = other.ds_piece;
    this->draw_list         = other.draw_list;
}


// 연산자 함수 =
Canvas& Canvas::operator=(const Canvas& other){
    this->width             = other.width;
    this->height            = other.height;
    this->dir_ds            = other.dir_ds;
    this->dir_ds_config     = other.dir_ds_config;
    this->dir_ds_complete   = other.dir_ds_complete;
    this->dir_ds_piece      = other.dir_ds_piece;
    this->ds_complete       = other.ds_complete;
    this->ds_piece          = other.ds_piece;
    this->draw_list         = other.draw_list;
    return *this;
}


// 연산자 함수 ==
bool Canvas::operator==(const Canvas& other) const {
    if (this->width             == other.width              &&
        this->height            == other.height             &&
        this->dir_ds            == other.dir_ds             &&
        this->dir_ds_config     == other.dir_ds_config      &&
        this->dir_ds_complete   == other.dir_ds_complete    &&
        this->dir_ds_piece      == other.dir_ds_piece       &&
        this->ds_complete       == other.ds_complete        &&
        this->ds_piece          == other.ds_piece           &&
        this->draw_list         == other.draw_list) return true;
    else                                            return false;
}


}
