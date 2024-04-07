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


// 연산자 함수 +=
MusicalSymbol&   MusicalSymbol::operator+=(const MusicalSymbol& other) {
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 새로운 MusicalSymbol 생성
    MusicalSymbol ms1(*this);   // this
    MusicalSymbol ms2(other);   // other
    
    // this 와 other 의 이미지 회전 및 크기 조정.
    ms1.img = mat_rotate(ms1.img, ms1.rotate, ms1.x, ms1.y);
    ms1.img = mat_scale(ms1.img, ms1.scale, ms1.x, ms1.y);
    ms2.img = mat_rotate(ms2.img, ms2.rotate, ms2.x, ms2.y);
    ms2.img = mat_scale(ms2.img, ms2.scale, ms2.x, ms2.y);
    
    // 두 이미지를 포함하는 영역 계산
    int ms1_t = (ms1.y);
    int ms1_b = (ms1.img.rows) - (ms1.y);
    int ms1_l = (ms1.x);
    int ms1_r = (ms1.img.cols) - (ms1.x);
    int ms2_t = (ms2.y);
    int ms2_b = (ms2.img.rows) - (ms2.y);
    int ms2_l = (ms2.x);
    int ms2_r = (ms2.img.cols) - (ms2.x);
    
    int t = (ms1_t > ms2_t) ? (ms1_t) : (ms2_t);
    int b = (ms1_b > ms2_b) ? (ms1_b) : (ms2_b);
    int l = (ms1_l > ms2_l) ? (ms1_l) : (ms2_l);
    int r = (ms1_r > ms2_r) ? (ms1_r) : (ms2_r);
    
    Size sz(l+r, t+b);
    
    // 흰 배경 생성
    Mat new_img(sz, CV_8UC1, Scalar(255));
    
    // 악상기호 붙여넣기
    int p1_x, p1_y, p2_x, p2_y;
    if (ms1_l > ms2_l) {
        p1_x = 0;
        p2_x = ms1_l - ms2_l;
    }
    else {
        p1_x = ms2_l - ms1_l;
        p2_x = 0;
    }
    if (ms1_t > ms2_t) {
        p1_y = 0;
        p2_y = ms1_t - ms2_t;
    }
    else {
        p1_y = ms2_t - ms1_t;
        p2_y = 0;
    }
    
    // 합성
    Mat roi1 = new_img(Rect(p1_x, p1_y, ms1.img.cols, ms1.img.rows));
    Mat roi2 = new_img(Rect(p2_x, p2_y, ms2.img.cols, ms2.img.rows));
    cv::min(roi1, ms1.img, roi1);   // new_img 의 roi1 영역에 this->img 를 복사
    cv::min(roi2, ms2.img, roi2);   // new_img 의 roi2 영역에 other.img 를 복사
    
    // 기존 config 정보 수정
    if (ms1_l < ms2_l) ms1.x = ms2_l;
    if (ms1_t < ms2_t) ms1.y = ms2_t;
    
    // 기존 정보 조정
    this->status        = -1;
    this->dir           = path();
    this->dir_config    = path();
    this->img           = new_img.clone();  // 새로운 이미지로 교체
    this->x             = ms1.x;            // 새로운 config들로 교체
    this->y             = ms1.y;
    this->rotate        = 0.0;
    this->scale         = 1.0;
    
    return *this;
}


// 연산자 함수 &
bool            MusicalSymbol::operator&(const MusicalSymbol& other){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 두 이미지를 포함하는 영역 계산
    int ms1_t = (this->y);
    int ms1_b = (this->img.rows) - (this->y);
    int ms1_l = (this->x);
    int ms1_r = (this->img.cols) - (this->x);
    int ms2_t = (other.y);
    int ms2_b = (other.img.rows) - (other.y);
    int ms2_l = (other.x);
    int ms2_r = (other.img.cols) - (other.x);
    
    int t = (ms1_t > ms2_t) ? (ms1_t) : (ms2_t);
    int b = (ms1_b > ms2_b) ? (ms1_b) : (ms2_b);
    int l = (ms1_l > ms2_l) ? (ms1_l) : (ms2_l);
    int r = (ms1_r > ms2_r) ? (ms1_r) : (ms2_r);
    
    Size sz(l+r, t+b);
    
    // 두개의 흰 배경 생성
    Mat img1(sz, CV_8UC1, Scalar(255));
    Mat img2(sz, CV_8UC1, Scalar(255));
    
    // 악상기호 붙여넣기
    int p1_x, p1_y, p2_x, p2_y;
    if (ms1_l > ms2_l) {
        p1_x = 0;
        p2_x = ms1_l - ms2_l;
    }
    else {
        p1_x = ms2_l - ms1_l;
        p2_x = 0;
    }
    if (ms1_t > ms2_t) {
        p1_y = 0;
        p2_y = ms1_t - ms2_t;
    }
    else {
        p1_y = ms2_t - ms1_t;
        p2_y = 0;
    }
    this->img.copyTo(img1(Rect(p1_x, p1_y, this->img.cols, this->img.rows)));   // this->img 를 img1 안에 복사
    other.img.copyTo(img2(Rect(p2_x, p2_y, other.img.cols, other.img.rows)));   // other->img 를 img2 안에 복사
    
    // 임계값 처리
    threshold(img1, img1, 5, 255, THRESH_BINARY);   // 5를 기준으로 이미지 img1 이진화
    threshold(img2, img2, 5, 255, THRESH_BINARY);   // 5를 기준으로 이미지 img2 이진화
    
    // 겹치는 부분 구하기 (논리 곱 연산 또는 논리 합 연산)
    Mat overlap_mask;
    bitwise_or(img1, img2, overlap_mask);  // or 연산
    
    // 겹치는 부분 확인
    double minVal, maxVal;
    minMaxLoc(overlap_mask, &minVal, &maxVal);
    
    if (minVal > 0) return false;
    else            return true;
}


}
