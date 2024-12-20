
#include <msig_Processing.hpp>

namespace MSIG {
namespace Processing {

// 합성
cv::Mat
Matrix::mat_attach(const cv::Mat& img, const cv::Mat& img_sub, int x, int y, bool trim) {
    using namespace std;
    using namespace cv;
    
    // 이미지 준비
    Mat img1 = img.clone();
    Mat img2 = img_sub.clone();
    
    // img1 크기를 넘어가면 잘라내기
    if (trim)
    {
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
            cv::min(roi1, roi2, roi1);
        }
        return img1;
    }
    
    // img1 크기를 넘어가도 잘라내지 않기
    else
    {
        // Rect 객체 생성
        Rect rect1 = Rect(0, 0, img1.cols, img1.rows);
        Rect rect2 = Rect(x, y, img2.cols, img2.rows);
        
        // x, y 음수값 제거
        while (rect2.y < 0) {
            rect1 = rect1 + Point(0, 1);
            rect2 = rect2 + Point(0, 1);
        }
        while (rect2.x < 0) {
            rect1 = rect1 + Point(1, 0);
            rect2 = rect2 + Point(1, 0);
        }
        
        // 크기 구하기
        Rect rect_new = rect1 | rect2;
        
        // 새로운 이미지 생성
        Mat img3 = Mat(rect_new.size(), CV_8UC1, Scalar(255));
        
        // 흑백 이미지 합치기(min)
        Mat roi1 = img3(rect1);
        Mat roi2 = img3(rect2);
        Mat roi3 = img1(rect1 - rect1.tl());
        Mat roi4 = img2(rect2 - rect2.tl());
        cv::min(roi1, roi3, roi1);
        cv::min(roi2, roi4, roi2);
        
        return img3;
    }
}

// 회전
cv::Mat
Matrix::mat_rotate(const cv::Mat& img, double& degree, int& cx, int& cy) {
    using namespace std;
    using namespace cv;
    
    //
    if (degree==0.0) return img.clone();
    
    // 화면 좌표계에 의한 각도 변환
    degree = -degree;
    
    // radian, sin, cos 값 계산
    double r1 = CV_PI / 180.0 * degree;         // degree -> radian
    double r2 = CV_PI / 180.0 * (90 - degree);  // (90 - degree) -> radian
    double sin_value1 = sin(r1);
    double sin_value2 = sin(r2);
    double cos_value1 = cos(r1);
    double cos_value2 = cos(r2);
    
    // 새로운 이미지 크기 계산
    int new_width = abs(img.rows * cos_value2) + abs(img.cols * cos_value1);
    int new_height = abs(img.cols * cos_value2) + abs(img.rows * cos_value1);
    
    // 기존 영상과 새로운 영상의 시작 좌표 차이 계산 : 순방향 사상
    int x_min = (0 - cx) * cos_value1 - (0 - cy) * sin_value1 + cx;
    int y_min = (0 - cx) * sin_value1 + (0 - cy) * cos_value1 + cy;
    for (auto& pt : vector<Point>({Point(img.cols, 0), Point(0, img.rows), Point(img.cols, img.rows)}))
    {
        // 좌표 계산
        int x_new = (pt.x - cx) * cos_value1 - (pt.y - cy) * sin_value1 + cx;
        int y_new = (pt.x - cx) * sin_value1 + (pt.y - cy) * cos_value1 + cy;
        
        // 최소값 찾기
        if (x_new < x_min) x_min = x_new;
        if (y_new < y_min) y_min = y_new;
    }
    x_min = -x_min;
    y_min = -y_min;
    
    // 이미지 준비
    Mat img_new = Mat(new_height, new_width, img.type());

    // 역방향사상 범위 제한
    Rect rect(Point(0, 0), img.size());
    
    // 이미지 사상 : 역방향 사상
    for (int y_new=-y_min; y_new<img_new.rows-y_min; y_new++)
    {
        for (int x_new=-x_min; x_new<img_new.cols-x_min; x_new++)
        {
            // 좌표 계산
            int x_old = (x_new - cx) * cos_value1 + (y_new - cy) * sin_value1 + cx;
            int y_old = -(x_new - cx) * sin_value1 + (y_new - cy) * cos_value1 + cy;
            
            // 사상 범위 제한에 따른 픽셀값 배정
            if (rect.contains(Point(x_old, y_old))) img_new.at<uchar>(y_new+y_min, x_new+x_min) = img.at<uchar>(y_old, x_old);
            else                                    img_new.at<uchar>(y_new+y_min, x_new+x_min) = 255;
        }
    }
    
    // 중심 좌표 계산
    cx = cx + x_min;
    cy = cy + y_min;
    
    // 회전 값 초기화
    degree = 0.0;
    
    return img_new;
}

// 확대
cv::Mat
Matrix::mat_scale(const cv::Mat& img, double& scale, int& cx, int& cy) {
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
    
    // 중심점 계산
    cx = (int)(cx * scale);
    cy = (int)(cy * scale);
    
    // 배율값 초기화
    scale = 1.0;
    
    return img1;
}

// 대칭
cv::Mat
Matrix::mat_symmetry(const cv::Mat& img, std::string symmetry) {
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

// 배경 제거
cv::Mat
Matrix::remove_padding(const cv::Mat& img, int& center_x, int& center_y) {
    using namespace std;
    using namespace cv;

    // 이미지를 Grayscale로 변환
    Mat gray;
    if (img.channels() > 1) {
        cvtColor(img, gray, COLOR_BGR2GRAY);
    }
    else {
        gray = img.clone();
    }

    // 이미지 색상을 반전
    Mat inverted;
    bitwise_not(gray, inverted);

    // Threshold를 적용하여 220부터 흰색까지의 색상을 모두 검은색으로 변환
    Mat binary;
    threshold(inverted, binary, 128, 255, THRESH_BINARY);
    
    //
    int top = 0, bottom = binary.rows - 1;
    int left = 0, right = binary.cols - 1;
    
    // 상단에서 첫 번째 0이 아닌 행을 찾음
    for (int i = 0; i < binary.rows; ++i) {
        if (cv::sum(binary.row(i))[0] > 0) {
            top = i;
            break;
        }
    }
    
    // 하단에서 첫 번째 0이 아닌 행을 찾음
    for (int i = binary.rows - 1; i >= 0; --i) {
        if (cv::sum(binary.row(i))[0] > 0) {
            bottom = i;
            break;
        }
    }
    
    // 좌측에서 첫 번째 0이 아닌 열을 찾음
    for (int j = 0; j < binary.cols; ++j) {
        if (cv::sum(binary.col(j))[0] > 0) {
            left = j;
            break;
        }
    }

    // 우측에서 첫 번째 0이 아닌 열을 찾음
    for (int j = binary.cols - 1; j >= 0; --j) {
        if (cv::sum(binary.col(j))[0] > 0) {
            right = j;
            break;
        }
    }
    
    // 좌표 계산
    center_x = center_x - left;
    center_y = center_y - top;
    
    // 크롭된 이미지 반환
    return gray(cv::Rect(left, top, right - left + 1, bottom - top + 1));
}

}
}
