#include <msig.hpp>

namespace msig
{


cv::Mat Note::combine_mat(const cv::Mat& img, int x, int y, const cv::Mat& img_sub){
    using namespace std;
    using namespace cv;
    
    // img_result 생성
    Mat img_result = img.clone();
    
    // img, img_sub 시작 좌표
    int img_x = 0;
    int img_y = 0;
    int img_sub_x = img_x + x;
    int img_sub_y = img_y + y;
    
    // 겹치는 영역의 좌상단 점 계산
    int x1 = max(img_x, img_sub_x);
    int y1 = max(img_y, img_sub_y);
    
    // 겹치는 영역의 우하단 점 계산
    int x2 = min(img_x+img.cols, img_sub_x+img_sub.cols);
    int y2 = min(img_y+img.rows, img_sub_y+img_sub.rows);
    
    // 겹치는 영역 있는지 확인
    if (x1 < x2 and y1 < y2){
        // ROI 범위 생성
        Range r1(y1-img_y, y2-img_y);           // img의 y(row) 범위
        Range r2(x1-img_x, x2-img_x);           // img의 x(col) 범위
        Range r3(y1-img_sub_y, y2-img_sub_y);   // img_sub의 y(row) 범위
        Range r4(x1-img_sub_x, x2-img_sub_x);   // img_sub의 x(col) 범위
        
        // ROI 생성
        Mat roi1 = img(r1, r2);
        Mat roi2 = img_sub(r3, r4);
        Mat roi_result = img_result(r1, r2);
        
        // 흑백 이미지 합치기(min)
        // 알파 채널 분리 고려 하기...!
        min(roi1, roi2, roi_result);
        
        // 겹치는 영역 찾기 성공, img_result 반환
        return img_result;
    }
    else {
        // 겹치는 영역 찾기 실패, img 반환
        return img.clone();
    }
}

cv::Mat Note::rotate_mat(const cv::Mat& img, double degree, int center_x, int center_y){
    using namespace std;
    using namespace cv;
    
    // radian, sin, cos 값 계산
    double radian = degree * CV_PI / 180.0 ;
    double sin_value = sin(radian);
    double cos_value = cos(radian);
    
    // ==================== img_result의 크기 구하기
    // Point 저장 벡터
    vector<Point> pts = {
        Point(0,0),                 // lt
        Point(img.cols,0),          // rt
        Point(0,img.rows),          // lb
        Point(img.cols,img.rows)    // rb
    };
    vector<Point> pts_out;
    
    // 좌표 계산
    for (auto p: pts){
        // 좌표 계산
        double x_old = p.x;
        double y_old = p.y;
        double cx = center_x;
        double cy = center_y;
        double x_new = (x_old-cx)*cos_value - (y_old-cy)*sin_value + cx;
        double y_new = (x_old-cx)*sin_value + (y_old-cy)*cos_value + cy;
        
        // 올림, 내림 처리
        x_new>=0 ? x_new=ceil(x_new) : x_new=floor(x_new);
        y_new>=0 ? y_new=ceil(y_new) : y_new=floor(y_new);
        
        // 저장
        pts_out.push_back(Point(x_new, y_new));
    }
    
    // 좌표 찾기
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
    
    // 좌표로 Size 생성
    Size img_result_size(w, h);
    
    // ==================== img_result 생성
    // img_result 생성
    Mat img_result(img_result_size, img.type(), Scalar(255));
    Mat img_result_copy = img_result.clone();
    
    //
    int combine_x = (img_result.cols - img.cols)/2.0;
    int combine_y = (img_result.rows - img.rows)/2.0;
    img_result_copy = combine_mat(img_result, combine_x, combine_y, img);
    
    // 역방향사상 범위 제한
    Rect rect(Point(0, 0), img_result_copy.size());
    
    // img_ori(x',y') -> img_result(x,y) 역방향 사상
    for (int y_old=0; y_old<img_result.rows; y_old++){
        for (int x_old=0; x_old<img_result.cols; x_old++){
            // 좌표 계산
            double cx = center_x + combine_x;
            double cy = center_y + combine_y;
            double x_new = (x_old-cx)*cos_value + (y_old-cy)*sin_value + cx;
            double y_new = -(x_old-cx)*sin_value + (y_old-cy)*cos_value + cy;
            
            // 올림, 내림 처리
            x_new>=0 ? x_new=ceil(x_new) : x_new=floor(x_new);
            y_new>=0 ? y_new=ceil(y_new) : y_new=floor(y_new);
            
            // 범위 제한 확인
            if (rect.contains(Point2d(x_new, y_new))){
                img_result.at<uchar>((int)y_new, (int)x_new) = img_result_copy.at<uchar>(y_old, x_old);
            }
        }
    }
    return img_result;
}

cv::Mat Note::scale_mat(const cv::Mat& img, double scale){
    using namespace std;
    using namespace cv;
    
    // img_result 생성
    Mat img_result;
    
    // 변경되고자 하는 크기
    Size_<int> size = (Size_<double>)img.size()*scale;
    
    // 이미지 확대 및 축소
    if (scale > 1.0) {
        resize(img, img_result, size, 0, 0, INTER_LINEAR);
    }
    else if (scale < 1.0) {
        resize(img, img_result, size, 0, 0, INTER_AREA);
    }
    else return img.clone();
    
    // scale 처리된 이미지 반환
    return img_result;
}


}
