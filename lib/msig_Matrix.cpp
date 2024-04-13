#include <msig_Matrix.hpp>

namespace msig
{


// 행렬 합성
cv::Mat mat_attach      (const cv::Mat& img, const cv::Mat& img_sub, int x, int y, bool trim){
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


// 행렬 회전
cv::Mat mat_rotate      (const cv::Mat& img, double degree, int& cx, int& cy){
    using namespace std;
    using namespace cv;
    
    //
    if (degree==0.0) return img.clone();
    
    // 이미지 준비
    Mat img1 = img.clone();
    Mat img2 = Mat(img.size(), img.type(), Scalar(255));
    
    // radian, sin, cos 값 계산
    double radian = degree * CV_PI / 180.0 ;
    double sin_value = sin(radian);
    double cos_value = cos(radian);
    
    // 역방향사상 범위 제한
    Rect rect(Point(0, 0), img1.size());
    
    // 역방향 사상
    for (int y_new=0; y_new<img2.rows; y_new++){
        for (int x_new=0; x_new<img2.cols; x_new++){
            // 좌표 계산
            double x_old = (x_new - cx) * cos_value + (y_new - cy) * sin_value + cx;
            double y_old = -(x_new - cx) * sin_value + (y_new - cy) * cos_value + cy;
            
            // 올림, 내림 처리
            x_old>=0 ? x_old=ceil(x_old) : x_old=floor(x_old);
            y_old>=0 ? y_old=ceil(y_old) : y_old=floor(y_old);
            
            // 범위 제한 확인
            if (rect.contains(Point(x_old, y_old))){
                img2.at<uchar>((int)y_new, (int)x_new) = img1.at<uchar>(y_old, x_old);
            }
        }
    }
    return img2;
}


// 행렬 확대
cv::Mat mat_scale       (const cv::Mat& img, double scale, int& cx, int& cy){
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


// 이미지 재생성(이미지 복구)
void    restore_img(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 존재 여부 확인
    if (exists(dir) && is_regular_file(dir) && dir.extension().string()==".png"){
       
        Mat img = imread(dir.string(), IMREAD_GRAYSCALE);   // 흑백 영상으로 이미지 불러오기
        CV_Assert(img.data);                                // 잘 불러와졌는지 확인
        imwrite(dir.string(), img);                         // 불러온 이미지 저장
    }
    else return;
}


// 흑백 영상 패딩 제거
cv::Mat remove_padding(const cv::Mat& img) {
    using namespace std;
    using namespace cv;
    
    int x = 0, y = 0;
    return remove_padding(img, x, y);
}
cv::Mat remove_padding(const cv::Mat& img, int& center_x, int& center_y) {
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
    threshold(inverted, binary, 220, 255, THRESH_BINARY);

    // Contour를 찾아 가장 큰 영역의 바운딩 박스를 찾음
    vector<vector<Point>> contours;//윤곽선을 나타내는 점들의 집합
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Rect boundingBox;// 윤곽선을 하나에 벡터에 모아서 바운딩박스 계산
    if (!contours.empty()) {
        vector<Point> allPoints;
        for (auto& contour : contours) {
            allPoints.insert(allPoints.end(), contour.begin(), contour.end());
        }
        boundingBox = boundingRect(allPoints);
    }
    else {// 패딩을 제거할 영역이 없으면 원본 이미지 반환
        center_x = img.cols / 2;
        center_y = img.rows / 2;
        return img.clone();
    }

    // 이미지에서 패딩 제거
    Mat  padding_img = img(boundingBox).clone();

    // 부드럽게 만들어주는 알고리즘 처리 (Gaussian Blur 예시)
    GaussianBlur(padding_img, padding_img, Size(5, 5), 0);

    // 이미지 중심 좌표 다시 계산
    center_x = boundingBox.x + boundingBox.width / 2;
    center_y = boundingBox.y + boundingBox.height / 2;

    return padding_img;
}


}
