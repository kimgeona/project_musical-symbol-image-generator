#include <msig_Matrix.hpp>

namespace msig
{


// 행렬 합성
cv::Mat mat_attach      (const cv::Mat& img, const cv::Mat& img_sub, int x, int y){
    using namespace std;
    using namespace cv;
    
    // 이미지 준비
    Mat img1 = img.clone();
    Mat img2 = img_sub.clone();
    
    // img, img_sub 시작 좌표
    int img_x = 0;
    int img_y = 0;
    int img_sub_x = img_x + x;
    int img_sub_y = img_y + y;
    
    // 겹치는 영역의 좌상단 점 계산
    int x1 = max(img_x, img_sub_x);
    int y1 = max(img_y, img_sub_y);
    
    // 겹치는 영역의 우하단 점 계산
    int x2 = min(img_x+img1.cols, img_sub_x+img2.cols);
    int y2 = min(img_y+img1.rows, img_sub_y+img2.rows);
    
    // 겹치는 영역 있는지 확인
    if (x1 < x2 && y1 < y2){
        // ROI 범위 생성
        Range r1(y1-img_y, y2-img_y);           // img의 y(row) 범위
        Range r2(x1-img_x, x2-img_x);           // img의 x(col) 범위
        Range r3(y1-img_sub_y, y2-img_sub_y);   // img_sub의 y(row) 범위
        Range r4(x1-img_sub_x, x2-img_sub_x);   // img_sub의 x(col) 범위
        
        // ROI 생성
        Mat roi1 = img1(r1, r2);
        Mat roi2 = img2(r3, r4);
        
        // 흑백 이미지 합치기(min)
        min(roi1, roi2, roi1);
    }
    
    return img1;
}

// 행렬 회전
cv::Mat mat_rotate      (const cv::Mat& img, double degree, int x, int y){
    using namespace std;
    using namespace cv;
    
    // 이미지 준비
    Mat img1 = img.clone();
    
    // radian, sin, cos 값 계산
    double radian = degree * CV_PI / 180.0 ;
    double sin_value = sin(radian);
    double cos_value = cos(radian);
    
    // 새로운 이미지 꼭짓점 좌표 계산
    vector<Point> pts = {
        Point(0,0),                 // lt
        Point(img1.cols,0),         // rt
        Point(0,img1.rows),         // lb
        Point(img1.cols,img1.rows)  // rb
    };
    vector<Point> pts_out;
    for (auto p: pts){
        // 좌표 계산
        double x_old = p.x;
        double y_old = p.y;
        double x_new = (x_old-x)*cos_value - (y_old-y)*sin_value + x;
        double y_new = (x_old-x)*sin_value + (y_old-y)*cos_value + y;
        
        // 올림, 내림 처리
        x_new>=0 ? x_new=ceil(x_new) : x_new=floor(x_new);
        y_new>=0 ? y_new=ceil(y_new) : y_new=floor(y_new);
        
        // 저장
        pts_out.push_back(Point(x_new, y_new));
    }
    
    // 새로운 이미지 크기 계산
    int minX, maxX, minY, maxY;
    minX = maxX = pts_out[0].x;
    minY = maxY = pts_out[0].y;
    for (auto p : pts_out) {
       minX = min(minX, p.x);
       maxX = max(maxX, p.x);
       minY = min(minY, p.y);
       maxY = max(maxY, p.y);
    }
    int w = maxX - minX;
    int h = maxY - minY;
    Size img_size(w, h); // 이미지 크기
    
    // 이미지 준비
    Mat img2(img_size, img1.type(), Scalar(255));
    Mat img3 = img2.clone();
    
    // 이미지 합성 위치 계산
    int combine_x = (img2.cols - img.cols)/2.0;
    int combine_y = (img2.rows - img.rows)/2.0;
    img3 = mat_attach(img3, img1, combine_x, combine_y);
    
    // 역방향사상 범위 제한
    Rect rect(Point(0, 0), img3.size());
    
    // img3(x',y') -> img2(x,y) 역방향 사상
    for (int y_old=0; y_old<img2.rows; y_old++){
        for (int x_old=0; x_old<img2.cols; x_old++){
            // 좌표 계산
            double cx = x + combine_x;
            double cy = y + combine_y;
            double x_new = (x_old-cx)*cos_value + (y_old-cy)*sin_value + cx;
            double y_new = -(x_old-cx)*sin_value + (y_old-cy)*cos_value + cy;
            
            // 올림, 내림 처리
            x_new>=0 ? x_new=ceil(x_new) : x_new=floor(x_new);
            y_new>=0 ? y_new=ceil(y_new) : y_new=floor(y_new);
            
            // 범위 제한 확인
            if (rect.contains(Point2d(x_new, y_new))){
                img2.at<uchar>((int)y_new, (int)x_new) = img3.at<uchar>(y_old, x_old);
            }
        }
    }
    return img2;
}

// 행렬 확대
cv::Mat mat_scale       (const cv::Mat& img, double scale){
    using namespace std;
    using namespace cv;
    
    // img_result 생성
    Mat img1 = img.clone();
    Mat img2;
    
    // 변경되고자 하는 크기
    Size_<int> size = (Size_<double>)img1.size()*scale;
    
    // 이미지 확대 및 축소
    if (scale > 1.0) resize(img1, img1, size, 0, 0, INTER_LINEAR);
    if (scale < 1.0) resize(img1, img1, size, 0, 0, INTER_AREA);
    
    return img1;
}

// 행렬 대칭
cv::Mat mat_symmetry    (const cv::Mat& img, std::string symmetry){
    using namespace std;
    using namespace cv;
    
    // 이미지 복사
    Mat img1 = img.clone();
    
    // 소문자로 변환
    transform(symmetry.begin(), symmetry.end(), symmetry.begin(), ::tolower);
    
    // 문자열 symmetry에 'x' 문자 존재 확인하고 있으면 x축 대칭 수행
    if (symmetry.find('x') != string::npos) { flip(img1, img1, 0);}
    
    // 문자열 symmetry에 'y' 문자 존재 확인하고 있으면 y축 대칭 수행
    if (symmetry.find('y') != string::npos) {flip(img1, img1, 1);}
    
    // 완성된 이미지 리턴
    return img1;
}


}
