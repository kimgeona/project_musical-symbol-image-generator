#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 1채널 이미지 생성
    Mat img(512, 256, CV_8UC1, Scalar(255));
    
    // 그리고 싶은 지점과 이미지의 중심점이 겹치도록 그리기
    
    
    // 생성된 이미지 리턴
    return img;
}


}
