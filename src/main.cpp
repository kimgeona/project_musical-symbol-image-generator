#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace msig;
    
    string s = "";
    cout << "enter the number that you want to test." << endl;
    cout << "---- Geona Kim --------------" << endl;
    cout << "(1) dst::DSTree()"             << endl;
    cout << "(2) msig::MusicalSymbol()"     << endl;
    cout << "(3) msig::Note()"              << endl;
    cout << "---- Junhyuk Lim ------------" << endl;
    cout << "(4) msig::remove_padding()"    << endl;
    cout << "-----------------------------" << endl;
    cout << ">>";
    cin >> s;
    cout << endl << endl << endl;
    
    
    
    if (s=="1"){
        cout << "--dst::DSTree()--------------" << endl;
        cout << "--done.----------------------" << endl;
    }
    
    
    
    else if (s=="2"){
        cout << "--msig::MusicalSymbol()------" << endl;
        cout << "--done.----------------------" << endl;
    }
    
    
    
    else if (s=="3"){
        cout << "--msig::Note()---------------" << endl;
        
        Note n = Note("new-symbol-dataset");
        n.set("line-@", "staff-0-0.png");
        n.set("rest-@", "rest-4-dot.png");
        //n.save_as_img("test.png");
        //n.show();
        destroyAllWindows();
        
        cout << "--done.----------------------" << endl;
    }
    
    
    
    else if (s=="4"){
        cout << "--msig::remove_padding()-----" << endl;
        
        Mat img = imread("이미지주소입력", IMREAD_GRAYSCALE);
        CV_Assert(img.data);
        Mat img_out = remove_padding(img);
        imshow("img", img);
        imshow("img_out", img_out);
        waitKey();
        
        cout << "--done.----------------------" << endl;
    }
    
    
    
    else {
        cout << "worng input." << endl;
        cout << "exit the program." << endl;
    }
    
    return 0;
}
