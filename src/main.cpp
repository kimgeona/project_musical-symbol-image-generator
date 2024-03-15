#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace msig;
    
    cout << "Making Muscial-Symbols Dataset" << endl;
    
    // create Note
    Note n = Note("new-symbol-dataset");
    
    // set
    //n.set("line", "staff-0-0");
    
    // save as image
    //n.save_as_img("test.png");
    
    // show Note
    //n.show();
    
    // return none error
    destroyAllWindows();
    return 0;
}
