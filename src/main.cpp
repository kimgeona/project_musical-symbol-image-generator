#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace msig;
    
    cout << "Making Muscial-Symbols Dataset" << endl;
    
    // 준혁 테스트
    /*
     Mat img = imread("이미지주소입력", IMREAD_GRAYSCALE);
     Mat img_out = remove_padding(img);
     imshow("img", img);
     imshow("img_out", img_out);
     waitKey();
     */
    
    // 건아 테스트
    Note n = Note("new-symbol-dataset");
    n.set("line-@", "staff-0-0.png");
    //n.save_as_img("test.png");
    //n.show();
    destroyAllWindows();
    
    return 0;
}
