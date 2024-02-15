#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 1. 1채널 이미지 생성
    Mat img(width, height, CV_8UC1, Scalar(255));
    
    // 2. draw_list에 있는 내용들을 화면에 그리기.
    //     2-1. draw_list에 있는 내용들 상호관계 파악하여 "x_y_각도_확대축소_대칭_전체길이_현재" 수정
    //         2-1-1. 같이 표기할 수 없는 내용들 조사.
    //         2-1-2. 위치 우선순위 조사. (+ 슬러 나중에)
    //         2-1-3. x_y 값 위치 조정.
    //     2-2. draw_list 수정이 완료되면, Mat이미지에 그리기
    //         2-2-1. img의 헤더를 만들고 그곳에 음표 이미지를 붙여넣기.
    
    // 3. 생성된 이미지 리턴
    return img;
}


}
