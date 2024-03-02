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
    n.set("a4_#_8");
    
    // save as image
    n.save_as_img("test.png");
    
    // show Note
    n.show();
    
    // return none error
    destroyAllWindows();
    return 0;
}
