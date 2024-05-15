#include <msig_Canvas.hpp>

namespace msig
{


// 그리기
cv::Mat Canvas::draw()
{
    using namespace std;
    using namespace cv;
    
    // 흰 이미지 생성
    Mat img = Mat(height, width, CV_8UC1, Scalar(255));
    
    // 악상기호 배치 알고리즘(MSPA)
    MSPA mspa;
    
    // 그리기 목록(draw_list)에 있는 악상 기호들 배치
    for(auto& ms : draw_list)
    {
        // 악상기호 패딩 제거
        //remove_padding(ms.img, ms.x, ms.y);
        
        // 악상기호 종류에 따라 적절한 위치에 배치
        mspa.add(ms);
    }
    
    // 배치가 완료된 악상기호 이미지에 그리기
    img = draw_symbols(img, mspa.get());
    
    // 생성된 이미지 리턴
    return img;
}



// 악상 기호 그리기
cv::Mat Canvas::draw_symbols(const cv::Mat& img, const MusicalSymbol& ms, bool auxiliary_line)
{
    using namespace std;
    using namespace cv;
    
    // 배경 이미지 준비
    Mat img1 = img.clone();
    
    // 악상 기호 이미지 준비
    Mat img2 = ms.img.clone();
    int cx = ms.x;
    int cy = ms.y;
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    img2 = mat_rotate(img2, ms.rotate, cx, cy);     // 이미지 회전
    img2 = mat_scale(img2, ms.scale, cx, cy);       // 이미지 확대
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true)
    {
        for (auto h : vector<int>({-ms.pad*2, -ms.pad, 0, ms.pad, ms.pad*2}))
        {
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
    if (auxiliary_line==true)
    {
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
    }
    
    return img1;
}


}
