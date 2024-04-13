#include <msig_Canvas.hpp>

namespace msig
{


// 그리기
cv::Mat Canvas::draw(){
    using namespace std;
    using namespace cv;
    
    // 변수들
    int     img_w=256;  // 생성할 이미지 너비
    int     img_h=512;  // 생성할 이미지 높이
    
    // 흰 이미지 생성
    Mat img = Mat(img_h, img_w, CV_8UC1, Scalar(255));
    
    // 누적된 악상기호 이미지
    MusicalSymbol ms_buff;
    
    // 누적된 앙상기호 패딩
    int pad = 0;
    
    // draw_list에 있는 악상 기호들 img에 그리기
    for(auto ms : draw_list)
    {
        // 악상기호 패딩 제거
        //remove_padding(ms.img, ms.x, ms.y);
        
        // 처음 그려지는 경우
        if (ms_buff==MusicalSymbol())
        {
            ms_buff = ms;                   // 이미지 복사
            continue;
        }
        
        // 독립적 위치
        if (ms.dir.parent_path().filename().string()=="line-@"      ||
            ms.dir.parent_path().filename().string()=="edge-@"      ||
            ms.dir.parent_path().filename().string()=="note-up-@"   ||
            ms.dir.parent_path().filename().string()=="note-down-@")
        {
            ms_buff += ms;                  // 이미지 갱신
        }
        
        // 의존적 위치 : 악상기호 위치 조정(아래로 보내기)
        else
        {
            while (true)
            {
                // 겹치지 않을 때 까지 내려보내기
                while (ms_buff & ms)
                {
                    ms.y = ms.y - 1;
                }
                
                // 추가 간격 조정
                ms.y = ms.y - (ms.pad / 2.5);
                
                // 겹치는지 확인
                if (ms_buff & ms)   continue;
                else                break;
            }
            
            ms_buff += ms;                  // 이미지 갱신
        }
    }
    
    // 악상기호 이미지 그리기
    img = draw_symbols(img, ms_buff);
    
    // 생성된 이미지 리턴
    return img;
}


// 그리기 위치 조정
void    Canvas::draw_adjustment(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 백업용
    map<path, vector<size_t>> dirs_pts; // {xl, xr, yt, yb}
    
    // draw_list 순회
    for (auto& ms : draw_list){
        /*
        // 현재 pt
        static vector<size_t> pt = {0b0, 0b0, 0b0, 0b0};
        
        // 백업용 pt 찾기
        if (dirs_pts.find(ms.dir.parent_path())!=dirs_pts.end())
            pt = dirs_pts[ms.dir.parent_path()];    // 찾은 경우 현재 pt를 백업용 pt로 교체
        
        // 악상기호 이미지 별로 xl, xr, yt, yb 값 조정 0b0000->0bxxxx
        string p = ms.dir.parent_path().string();
        if (p=="line-@"){
            // 지울 오선지 갯수 구하기
            int n = 0;
            char c = ms.dir.filename().string()[6];
            for (int i=1; i<stoi(ms.dir.filename().string()[7]); i++) n += 7;
                 if (c == 'c') n+= 1;
            else if (c == 'd') n+= 2;
            else if (c == 'e') n+= 3;
            else if (c == 'f') n+= 4;
            else if (c == 'g') n+= 5;
            else if (c == 'a') n+= 6;
            else if (c == 'b') n+= 7;
            
            if (n % 2 == 0) n = 3;
            else            n = 2;
            
            // 해당 갯수만큼 xl, xr, yt, yb 값 조정
            // 그 그 config 조정
        }
        else if (p=="note-up-@"){
            
        }
        else if (p=="accidental-#"){
            
        }
        
        // 악상기호 이미지 별로 xl, xr, yt, yb 값에 의한 config x, y 조정
        // ...
        
        // pt 백업
        dirs_pts[ms.dir.parent_path()] = pt;
         */
    }
}


// 악상 기호 그리기
cv::Mat Canvas::draw_symbols(const cv::Mat& img, const msig::MusicalSymbol& ms, bool auxiliary_line){
    using namespace std;
    using namespace cv;
    
    // 편집할 이미지들
    Mat img1 = img.clone();         // 행렬 복사
    Mat img2 = ms.img.clone();      // 행렬 복사
    
    // 악상기호 이미지 중점
    int cx = ms.x;
    int cy = ms.y;
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    //img2 = mat_rotate(img2, ms.rotate, cx, cy);
    img2 = mat_scale(img2, ms.scale, cx, cy);
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true){
        for (auto h : vector<int>({-ms.pad*2, -ms.pad, 0, ms.pad, ms.pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
    }
    
    // 이미지 합성 좌표 계산
    int x = (img.cols)/2.0 - cx;   // 이미지 중심 - sub 이미지 중심 + config 값
    int y = (img.rows)/2.0 - cy;
    
    // 이미지 합성
    img1 = mat_attach(img1, img2, x, y);
    
    // 보조선 그리기 (십자선)
    if (auxiliary_line==true){
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
    }
    
    return img1;
}


}
