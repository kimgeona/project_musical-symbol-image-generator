#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 3채널 하얀 이미지(Mat) 생성
    Mat img(512, 512, CV_8UC3, Scalar(255, 255, 255));
    // Point 생성
    Point2i pt1(50, 50), pt2(462, 462);
    // Mat에 rectangle 그리기
    rectangle(img, pt1, pt2, Scalar(0, 255, 0));
    // 생성된 이미지 리턴
    return img;
}


}
