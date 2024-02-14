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
    
    // add
    n.add("ln_staff", "0_0_0.0_0.0");   // 5선지
    n.add("nt_4", "0_0_0.0_0.0");       // 4분음표
    n.add("nt_dot", "0_0_0.0_0.0");     // 점 표시
    
    // save as image
    n.save_as_img("test.png");
    
    // show Note
    n.show();
    
    // return none error
    destroyAllWindows();
    return 0;
}
