/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_Processing.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개별 헤더 파일들 중 하나로, 이미지 처리에 관한 클래스에 대한 정의가 기술되어 있습니다.
 -------------------------------------------------------------------------------
 포함된 클래스:
 - Matrix() : 영상 처리 클래스.
 - Brush()  : 데이터 증강을 위한 영상 처리 클래스.
 -------------------------------------------------------------------------------
 */

#ifndef msig_Processing_hpp
#define msig_Processing_hpp

// c++17
#include <opencv2/opencv.hpp>

namespace MSIG
{
namespace Processing
{


/*
 -------------------------------------------------------------------------------
 이름: Matrix()
 -------------------------------------------------------------------------------
 설명:
 영상 처리 클래스.
 
 Matrix는 중심 좌표가 있는 이미지(영상)의 영상 처리를 위해 작성된 클래스로, 클래스 내의 모든 함수는 정적
 (객체 생성 필요없이 클래스 함수를 사용)으로 제공됩니다.
 -------------------------------------------------------------------------------
 주요 기능:
 - 이미지 합성
 - 이미지 회전
 - 이미지 확대
 - 이미지 대칭
 - 이미지 배경 제거
 -------------------------------------------------------------------------------
 사용 방법:
 해당 클래스의 함수들은 모두 정적(static) 함수로 작성이 되어있기 때문에 객체 생성 없이 인자값을 주어 바로
 사용하시면 됩니다.
 -------------------------------------------------------------------------------
 주의 사항:
 - 영상 처리는 1채널(명암) 영상에 대해서만 수행됩니다.
 -------------------------------------------------------------------------------
 */
class Matrix
{
public:
    static cv::Mat mat_attach   (const cv::Mat& img, const cv::Mat& img_sub, int x, int y, bool trim=true)  // 합성
    {
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
    static cv::Mat mat_rotate   (const cv::Mat& img, double& degree, int& cx, int& cy)                       // 회전
    {
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
    static cv::Mat mat_scale    (const cv::Mat& img, double& scale, int& cx, int& cy)                        // 확대
    {
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
    static cv::Mat mat_symmetry (const cv::Mat& img, std::string symmetry)                                  // 대칭
    {
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
    static cv::Mat remove_padding(const cv::Mat& img, int& center_x, int& center_y)                         // 배경 제거
    {
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
        vector<vector<Point>> contours;
        findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        Rect boundingBox;
        if (!contours.empty()) {// 윤곽선을 하나에 벡터에 모아서 바운딩박스 계산
            vector<Point> allPoints;
            for (auto& contour : contours) {
                allPoints.insert(allPoints.end(), contour.begin(), contour.end());
            }
            boundingBox = boundingRect(allPoints);
        }
        else {// 패딩을 제거할 영역이 없으면 원본 이미지 반환
            center_x = img.cols / 2;
            center_y = img.rows / 2;
            Mat result = img.clone();
            
            return result;
        }

        // 이미지에서 패딩 제거
        Mat padding_img = img(boundingBox).clone();

        // 부드럽게 만들어주는 알고리즘 처리 (Gaussian Blur 예시)
        GaussianBlur(padding_img, padding_img, Size(5, 5), 0);

        // 이미지 중심 좌표 다시 계산
        center_x = boundingBox.x + boundingBox.width / 2;
        center_y = boundingBox.y + boundingBox.height / 2;

        // 원본 이미지에서 이전 및 새로운 중심 좌표를 표시
        Mat result = img.clone();

        return result;
    }
};


/*
 -------------------------------------------------------------------------------
 이름: Brush()
 -------------------------------------------------------------------------------
 설명:
 데이터 증강을 위한 영상 처리 클래스.
 
 Brush는 머신러닝 과대적합 방지를 위한 목적으로 작성된 클래스로, 이미지에 잡음 추가와 같은 이미지 변질
 작업을 수행하는 클래스입니다. 클래스 내의 모든 함수는 정적으로 제공됩니다.
 -------------------------------------------------------------------------------
 주요 기능:
 - 이미지 확대 및 축소
 - 이미지 회전
 - 이미지 이동
 - 이미지 잡음 추가
 - 이미지 흔들림
 - 이미지 흐림
 - 이미지 자르기
 - 이미지 워터마킹
 - 이미지 크기 확대 및 축소
 -------------------------------------------------------------------------------
 사용 방법:
 해당 클래스의 함수들은 모두 정적(static) 함수로 작성이 되어있기 때문에 객체 생성 없이 인자값을 주어 바로
 사용하시면 됩니다.
 -------------------------------------------------------------------------------
 주의 사항:
 - 데이터 증강을 위한 영상 처리는 1채널(명암) 영상에 대해서만 수행됩니다.
 -------------------------------------------------------------------------------
 */
class Brush
{
    static void scale(cv::Mat& image);
    static void rotate(cv::Mat& image);
    static void move(cv::Mat& image);
    static void noising(cv::Mat& image);
    static void wobble(cv::Mat& image);
    static void blur(cv::Mat& image);
    static void cut(cv::Mat& image);
    static void watermark(cv::Mat& image);
    static void size(cv::Mat& image);
};


}
}

#endif /* msig_Processing_hpp */
