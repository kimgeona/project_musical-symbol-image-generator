#include <msig_MusicalSymbol.hpp>

namespace msig
{


// 빈 생성자
MusicalSymbol::MusicalSymbol() {
    this->status        = -1;
    this->dir           = std::filesystem::path();
    this->dir_config    = std::filesystem::path();
    this->img           = cv::Mat();
    this->x             = 0;
    this->y             = 0;
    this->scale         = 1.0;
    this->rotate        = 0.0;
}


// 기본 생성자
MusicalSymbol::MusicalSymbol(std::filesystem::path dir, std::filesystem::path dir_config) {
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 기본 상태 설정
    this->status = 0;
    
    // 아래 과정에서 이상이 있을 경우
    if (init_dir(dir)               ||
        init_dir_config(dir_config) ||
        init_img()                  ||
        init_config())
    {
        cout << "MusicalSymbol: " << dir.filename().string() << " 을(를) 생성하는 과정에서 문제가 있습니다." << endl;
        this->status        = -1;
        this->dir           = std::filesystem::path();
        this->dir_config    = std::filesystem::path();
        this->img           = cv::Mat();
        this->x             = 0;
        this->y             = 0;
        this->scale         = 1.0;
        this->rotate        = 0.0;
        
    }
}


// 복사 생성자
MusicalSymbol::MusicalSymbol(const MusicalSymbol& other) {
    this->status        = other.status;
    this->dir           = other.dir;
    this->dir_config    = other.dir_config;
    this->img           = other.img.clone();
    this->x             = other.x;
    this->y             = other.y;
    this->scale         = other.scale;
    this->rotate        = other.rotate;
}


// 연산자 함수 =
MusicalSymbol&  MusicalSymbol::operator=(const MusicalSymbol& other) {
    this->status        = other.status;
    this->dir           = other.dir;
    this->dir_config    = other.dir_config;
    this->img           = other.img.clone();
    this->x             = other.x;
    this->y             = other.y;
    this->scale         = other.scale;
    this->rotate        = other.rotate;
    return *this;
}


// 연산자 함수 ==
bool            MusicalSymbol::operator==(const MusicalSymbol& other) const {
    if (this->status        == other.status &&
        this->dir           == other.dir &&
        this->dir_config    == other.dir_config &&
        //this->img           == other.img &&
        this->x             == other.x &&
        this->y             == other.y &&
        this->scale         == other.scale &&
        this->rotate        == other.rotate)    return true;
    else                                        return false;
}


}
