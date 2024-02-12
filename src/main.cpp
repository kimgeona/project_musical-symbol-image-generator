#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace msig;
    
    cout << "Making Muscial-Symbols Dataset" << endl;
    
    // create Note
    Note n = Note();
    
    // set
    n.set("pitch", "a4_#");     // 음정 = 라#
    n.set("head", "4");         // 머리 = 4분음표
    n.set("stem", "3.");        // 기둥 = 머리크기의 3.0배
    n.set("tail", "8");         // 꼬리 종류 = 일반
    n.set("dot", "dot");        // 점 표시
    
    // save as image
    n.save_as_img("test.png");
    
    // show Note
    n.show();
    
    // return none error
    destroyAllWindows();
    return 0;
}
