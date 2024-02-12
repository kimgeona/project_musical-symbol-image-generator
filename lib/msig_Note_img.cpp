#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


void Note::save_as_img(std::string file_name){
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}
void Note::show(){
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    string title = "note";
    
    // 윈도우 생성
    namedWindow(title, WINDOW_AUTOSIZE);
    moveWindow(title, 100, 100);
    
    // 윈도우에 이미지 그리기, ESC 입력시 종료
    imshow(title, image);
    while (true) {
        int key = waitKey();
        if (key==27) break;
        // key에 대한 작업 필요시 switch문 활용
    }
}
cv::Mat Note::cv_Mat(){
    using namespace std;
    using namespace cv;
    
    return draw();
}


}
