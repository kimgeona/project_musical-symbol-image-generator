#include <msig_MusicalSymbol.hpp>

namespace msig
{


MusicalSymbol::MusicalSymbol()
{
    this->status        = -1;
    this->dir           = std::filesystem::path();
    this->dir_config    = std::filesystem::path();
    this->img           = cv::Mat();
    this->x             = 0;
    this->y             = 0;
    this->scale         = 1.0;
    this->rotate        = 0.0;
}

MusicalSymbol::MusicalSymbol(std::filesystem::path dir, std::filesystem::path dir_config)
{
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
        std::runtime_error("msig::MusicalSymbol::MusicalSymbol() : 악상 기호 객체를 생성하는 과정에서 문제가 있습니다.");
    }
}

MusicalSymbol::MusicalSymbol(const MusicalSymbol& other)
{
    this->status        = other.status;
    this->dir           = other.dir;
    this->dir_config    = other.dir_config;
    this->img           = other.img.clone();
    this->x             = other.x;
    this->y             = other.y;
    this->scale         = other.scale;
    this->rotate        = other.rotate;
}



MusicalSymbol&  MusicalSymbol::operator=(const MusicalSymbol& other)
{
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

bool            MusicalSymbol::operator==(const MusicalSymbol& other) const
{
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

MusicalSymbol&  MusicalSymbol::operator+=(const MusicalSymbol& other)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 빈 객체와 연산시
    if (*this==MusicalSymbol())
    {
        *this = other;
        return *this;
    }
    
    // 이미지 복사 및 편집 1
    MusicalSymbol ms1(*this);
    ms1.set_default();
    
    // 이미지 복사 및 편집 2
    MusicalSymbol ms2(other);
    ms2.set_default();
    
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
    ms1.img.copyTo(img1(Rect(p1_x, p1_y, ms1.img.cols, ms1.img.rows)));   // ms1.img 를 img1 안에 복사
    ms2.img.copyTo(img2(Rect(p2_x, p2_y, ms2.img.cols, ms2.img.rows)));   // other->img 를 img2 안에 복사
    
    // 합성
    Mat new_img;
    bitwise_and(img1, img2, new_img);
    
    // 기존 정보 조정
    ms1.status        = -1;
    ms1.dir           = path();
    ms1.dir_config    = path();
    ms1.img           = new_img.clone();  // 새로운 이미지로 교체
    ms1.rotate        = 0.0;
    ms1.scale         = 1.0;
    
    // 기존 정보 수정
    if (ms1_l < ms2_l) ms1.x = ms2_l;
    if (ms1_t < ms2_t) ms1.y = ms2_t;
    
    // 새로운 데이터로 교체
    *this = ms1;
    
    return *this;
}

bool            MusicalSymbol::operator&(const MusicalSymbol& other)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 복사 및 편집 1
    MusicalSymbol ms1(*this);
    ms1.set_default();
    
    // 이미지 복사 및 편집 2
    MusicalSymbol ms2(other);
    ms2.set_default();
    
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
    ms1.img.copyTo(img1(Rect(p1_x, p1_y, ms1.img.cols, ms1.img.rows)));   // ms1.img 를 img1 안에 복사
    ms2.img.copyTo(img2(Rect(p2_x, p2_y, ms2.img.cols, ms2.img.rows)));   // ms2.img 를 img2 안에 복사
    
    // 임계값 처리
    threshold(img1, img1, 128, 255, THRESH_BINARY);   // 5를 기준으로 이미지 img1 이진화
    threshold(img2, img2, 128, 255, THRESH_BINARY);   // 5를 기준으로 이미지 img2 이진화
    
    // 겹치는 부분 구하기 (논리 곱 연산 또는 논리 합 연산)
    Mat overlap_mask;
    bitwise_or(img1, img2, overlap_mask);  // or 연산
    
    // 겹치는 부분 확인
    double minVal, maxVal;
    minMaxLoc(overlap_mask, &minVal, &maxVal);
    
    if (minVal > 0) return false;
    else            return true;
}



int MusicalSymbol::init_dir(std::filesystem::path dir)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // .png 파일인지 확인 후 저장
    if (exists(dir) && is_regular_file(dir) && dir.extension().string()==".png"){
        this->dir = dir;
        return 0;
    }
    else return 1;
}

int MusicalSymbol::init_dir_config(std::filesystem::path dir)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // .txt 파일인지 확인 후 저장
    if (exists(dir) && is_regular_file(dir) && dir.extension().string()==".txt"){
        this->dir_config = dir;
        return 0;
    }
    else return 1;
}

int MusicalSymbol::init_img()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 전처리
    restore_img(this->dir);     // 이미지 저장 오류 복구
    //remove_padding(this->dir);  // 이미지 패딩 제거
    
    // 이미지 불러오기
    this->img = imread(this->dir.string(), IMREAD_GRAYSCALE);   // 흑백 영상으로 불러오기
    
    // 불러온 이미지 확인
    if (this->img.data) return 0;
    else                return 1;
}

int MusicalSymbol::init_config()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 나의 디렉토리 표기법으로 주소 변환
    string search_dir = my_dir(this->dir);
    
    // config 파일에서 dir 찾기
    string find = my_grep(this->dir_config, search_dir);
    
    // 읽어온 config 정보 확인
    if (find=="") return make_config();
    else {
        // key=value 분리
        find = find.substr(search_dir.length()+1, find.length());
        
        // value 정규표현식 검사
        string re_i = "([-+]?[0-9]+)";
        string re_d = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
        regex reg("^"+re_i+"_"+re_i+"_"+re_d+"_"+re_d+"$");
        
        // 정규표현식 매칭 확인 후 저장
        if (regex_match(find, reg)){
            vector<string> configs = my_split(find, "_");
            this->x         = stoi(configs[0]);
            this->y         = stoi(configs[1]);
            this->rotate    = stod(configs[2]);
            this->scale     = stod(configs[3]);
            return 0;
        }
        else return make_config();
    }
}



int     MusicalSymbol::make_config()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 변수들
    Mat     img_out(img_h, img_w, CV_8UC1, Scalar(255));    // 흰 배경
    Mat     img_symbol  = this->img.clone();                // 악상 기호 이미지
    string  img_name    = this->dir.filename().string();    // 악상 기호 이름
    
    // config 정보 초기화
    this->x         = 0;
    this->y         = 0;
    this->rotate    = 0.0;
    this->scale     = 1.0;
    
    // 창 생성
    namedWindow(img_name, WINDOW_AUTOSIZE);
    
    // 화면에 그리기 (30Hz)
    while (true) {
        Mat     img1 = img_out.clone();     // 흰 배경
        Mat     img2 = img_symbol.clone();  // 악상 기호 이미지
        
        // 이미지 편집
        int tmp_x = this->x;
        int tmp_y = this->y;
        img2 = mat_rotate(img2, this->rotate, tmp_x, tmp_y);    // 이미지 회전
        img2 = mat_scale(img2, this->scale, tmp_x, tmp_y);      // 이미지 확대 축소
        
        // 보조선 그리기(오선)
        for (auto h : vector<int>({-pad*2, -pad, 0, pad, pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
        
        // 이미지 합성 좌표 계산 : img1 중심 - img2 중심 + img2 config 값
        int x = (img_w/2.0) - tmp_x;
        int y = (img_h/2.0) - tmp_y;
        
        // 이미지 합성
        img1 = mat_attach(img1, img2, x, y);
        
        // 보조선 그리기(십자선)
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
        
        // 이미지 화면에 그리기
        imshow(img_name, img1);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);         // 키보드 레이턴시
        if (key=='\n'|| key=='\r') break;   // 완료에 의한 종료
        if (key==27){                       // 중단에 의한 종료
            destroyWindow(img_name);
            return -1;
        }
        switch (key) {
            // 기본 속도
            case 'a': this->x += 1; break;  // 좌로 이동
            case 'd': this->x -= 1; break;  // 우로 이동
            case 'w': this->y += 1; break;  // 상으로 이동
            case 's': this->y -= 1; break;  // 하로 이동
            case 'q': this->rotate+=1.0;  break;  // 반시계 회전
            case 'e': this->rotate-=1.0;  break;  // 시계 회전
            case 'z': this->scale-=0.1;   break;  // 축소
            case 'c': this->scale+=0.1;   break;  // 확대
            // 빠르게
            case 'A': this->x+=5;         break;  // 좌로 이동
            case 'D': this->x-=5;         break;  // 우로 이동
            case 'W': this->y+=5;         break;  // 상으로 이동
            case 'S': this->y-=5;         break;  // 하로 이동
            case 'Q': this->rotate+=3.0;  break;  // 반시계 회전
            case 'E': this->rotate-=3.0;  break;  // 시계 회전
            case 'Z': this->scale-=0.2;   break;  // 축소
            case 'C': this->scale+=0.2;   break;  // 확대
            // 초기화
            case 'x':                       // 초기화
            case 'X':                       // 초기화
                this->x=0;
                this->y=0;
                this->rotate=0.0;
                this->scale=1.0;
                break;
        }
        
        // 범위 값 처리
        if (this->rotate<0.0)   this->rotate += 360.0;
        if (this->rotate>360.0) this->rotate -= 360.0;
        if (this->scale<0.1)    this->scale = 0.1;
    }
    destroyWindow(img_name);
    
    return save_config();
}

int     MusicalSymbol::edit_config()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 상태 체크
    if (this->status){
        cout << "수정이 불가능한 악상 기호 이미지입니다." << endl;
        return -1;
    }
    
    // 변수들
    Mat     img_out(img_h, img_w, CV_8UC1, Scalar(255));    // 흰 배경
    Mat     img_symbol  = this->img.clone();                // 악상 기호 이미지
    string  img_name    = this->dir.filename().string();    // 악상 기호 이름
    
    // 기존 config 정보 백업
    int     bp_x        = this->x;
    int     bp_y        = this->y;
    double  bp_scale    = this->scale;
    double  bp_rotate   = this->rotate;
    
    // 창 생성
    namedWindow(img_name, WINDOW_AUTOSIZE);
    
    // 화면에 그리기 (30Hz)
    while (true) {
        Mat     img1 = img_out.clone();     // 흰 배경
        Mat     img2 = img_symbol.clone();  // 악상 기호 이미지
        
        // 이미지 편집
        int tmp_x = this->x;
        int tmp_y = this->y;
        img2 = mat_scale(img2, this->scale, tmp_x, tmp_y);      // 이미지 확대 축소
        img2 = mat_rotate(img2, this->rotate, tmp_x, tmp_y);    // 이미지 회전
        
        // 보조선 그리기(오선)
        for (auto h : vector<int>({-pad*2, -pad, 0, pad, pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
        
        // 이미지 합성 좌표 계산 : img1 중심 - img2 중심 + img2 config 값
        int x = (img_w/2.0) - tmp_x;
        int y = (img_h/2.0) - tmp_y;
        
        // 이미지 합성
        img1 = mat_attach(img1, img2, x, y);
        
        // 보조선 그리기(십자선)
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
        
        // 이미지 화면에 그리기
        imshow(img_name, img1);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);         // 키보드 레이턴시
        if (key=='\n'|| key=='\r') break;   // 완료에 의한 종료
        if (key==27){                       // 중단에 의한 종료
            this->x         = bp_x;
            this->y         = bp_y;
            this->scale     = bp_scale;
            this->rotate    = bp_rotate;
            destroyWindow(img_name);
            return -1;
        }
        switch (key) {
            // 기본 속도
            case 'a': this->x+=1;         break;  // 좌로 이동
            case 'd': this->x-=1;         break;  // 우로 이동
            case 'w': this->y+=1;         break;  // 상으로 이동
            case 's': this->y-=1;         break;  // 하로 이동
            case 'q': this->rotate+=1.0;  break;  // 반시계 회전
            case 'e': this->rotate-=1.0;  break;  // 시계 회전
            case 'z': this->scale-=0.1;   break;  // 축소
            case 'c': this->scale+=0.1;   break;  // 확대
            // 빠르게
            case 'A': this->x+=3;         break;  // 좌로 이동
            case 'D': this->x-=3;         break;  // 우로 이동
            case 'W': this->y+=3;         break;  // 상으로 이동
            case 'S': this->y-=3;         break;  // 하로 이동
            case 'Q': this->rotate+=3.0;  break;  // 반시계 회전
            case 'E': this->rotate-=3.0;  break;  // 시계 회전
            case 'Z': this->scale-=0.2;   break;  // 축소
            case 'C': this->scale+=0.2;   break;  // 확대
            // 초기화
            case 'x':                       // 초기화
                this->x=bp_x;
                this->y=bp_y;
                this->scale=bp_scale;
                this->rotate=bp_rotate;
                break;
            case 'X':                       // 초기화(완젼 초기화)
                this->x=0;
                this->y=0;
                this->scale=1.0;
                this->rotate=0.0;
                break;
        }
        
        // 범위 값 처리
        if (this->rotate<0.0)   this->rotate += 360.0;
        if (this->rotate>360.0) this->rotate -= 360.0;
        if (this->scale<0.1)    this->scale = 0.1;
    }
    destroyWindow(img_name);
    
    return save_config();
}

int     MusicalSymbol::save_config()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 상태 체크
    if (this->status){
        cout << "수정이 불가능한 악상 기호 이미지입니다." << endl;
        return -1;
    }
    
    // "key=value" 문자열 생성
    string key = my_dir(this->dir);           // 나의 주소 구하기
    string value = to_string(this->x)+"_"+to_string(this->y)+"_"+to_string(this->rotate)+"_"+to_string(this->scale);
    
    // config 파일에 저장
    my_filter_out(this->dir_config, key);               // config 에서 해당 이름을 가진 라인 제거
    my_attach(this->dir_config, key + "=" + value);     // config 맨 뒤에 추가
    
    return 0;
}

void    MusicalSymbol::set_default()
{
    // rotate, sclae 값 이미지에 적용
    this->img = mat_rotate(img, rotate, x, y);
    this->img = mat_scale(img, scale, x, y);
    
    // rotate, sclae 에 기본값 저장
    rotate = 0.0;
    scale = 1.0;
}

void    MusicalSymbol::show()
{
    using namespace std;
    using namespace cv;
    
    // 흰 이미지 생성
    Mat img1 = Mat(img_h, img_w, CV_8UC1, Scalar(255));
    
    // 악상 기호 이미지 준비
    Mat img2 = img.clone();
    int cx = x;
    int cy = y;
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    img2 = mat_rotate(img2, rotate, cx, cy);     // 이미지 회전
    img2 = mat_scale(img2, scale, cx, cy);       // 이미지 확대
    
    // 이미지 합성 좌표 계산
    int nx = (img1.cols)/2.0 - cx;   // 이미지 중심 - sub 이미지 중심 + config 값
    int ny = (img1.rows)/2.0 - cy;
    
    // 이미지 합성
    img1 = mat_attach(img1, img2, nx, ny);
    
    // 이미지 미리보기
    cv::imshow("MusicalSymbol::show()", img1);
    cv::waitKey();
    cv::destroyWindow("MusicalSymbol::show()");
}


}
