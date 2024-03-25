#include <msig_MusicalSymbol.hpp>
#include <windows.h>

namespace msig
{


// msig_MusicalSymbol_Data.cpp
int MusicalSymbol::make_config      (){
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
        Mat     img1 = img_out.clone();
        Mat     img2 = img_symbol.clone();
        
        // 이미지 편집
        img2 = mat_rotate(img2, this->rotate, img2.cols/2.0, img2.rows/2.0);
        img2 = mat_scale(img2, this->scale);
        //img2 = mat_symmetry(img2, this->symmetry);
        
        // 보조선 그리기(오선)
        for (auto h : vector<int>({-pad*2, -pad, 0, pad, pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
        
        // 이미지 합성 좌표 계산 : img1 중심 - img2 중심 + img2 config 값
        int x = (img_w/2.0) - this->x;
        int y = (img_h/2.0) - this->y;
        
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
            return 1;
        }
        switch (key) {
            // 기본 속도
            case 'a':  this->x += 1; break;  // 좌로 이동
            case 'd': this->x -= 1; break;  // 우로 이동
            case 'w':    this->y += 1; break;  // 상으로 이동
            case 's':  this->y -= 1; break;  // 하로 이동

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
    
    return 0;
}
int MusicalSymbol::edit_config      (){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
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
        Mat     img1 = img_out.clone();
        Mat     img2 = img_symbol.clone();
        
        // 이미지 편집
        img2 = mat_rotate(img2, this->rotate, img2.cols/2.0, img2.rows/2.0);
        img2 = mat_scale(img2, this->scale);
        //img2 = mat_symmetry(img2, this->symmetry);
        
        // 보조선 그리기(오선)
        for (auto h : vector<int>({-pad*2, -pad, 0, pad, pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
        
        // 이미지 합성 좌표 계산 : img1 중심 - img2 중심 + img2 config 값
        int x = (img_w/2.0) - this->x;
        int y = (img_h/2.0) - this->y;
        
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
            return 1;
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
    
    return 0;
}


}
