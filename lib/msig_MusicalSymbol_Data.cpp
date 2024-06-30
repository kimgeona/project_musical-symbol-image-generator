#include <msig_MusicalSymbol.hpp>


namespace msig
{


// config 데이터 생성
int     MusicalSymbol::make_config(){
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


// config 데이터 수정
int     MusicalSymbol::edit_config(){
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


// config 저장하기
int     MusicalSymbol::save_config(){
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


// 현재 rotate, scale 상태를 이미지에 적용하고 각각 기본값 저장.
void    MusicalSymbol::set_default()
{
    // rotate, sclae 값 이미지에 적용
    this->img = mat_rotate(img, rotate, x, y);
    this->img = mat_scale(img, scale, x, y);
    
    // rotate, sclae 에 기본값 저장
    rotate = 0.0;
    scale = 1.0;
}

// 미리보기
void MusicalSymbol::show()
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
