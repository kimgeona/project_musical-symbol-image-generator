#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    
    string s = "";
    cout << "enter the number that you want to test." << endl;
    cout << "---- Geona Kim --------------" << endl;
    cout << "1)dst::DSTree()"               << endl;
    cout << "2)msig::MusicalSymbol()"       << endl;
    cout << "3)msig::Note()"                << endl;
    cout << "---- Junhyuk Lim ------------" << endl;
    cout << "4)msig::remove_padding()"      << endl;
    cout << "-----------------------------" << endl;
    cout << ">>";
    cin >> s;
    cout << endl << endl << endl;
    
    
    
    if      (s=="1"){
        cout << "--dst::DSTree()--------------" << endl << endl;
        // 자료구조 위치
        path p1("new-symbol-dataset"), p2("complete");
        path p = p1 / p2;
        
        // 자료구조 접근 알고리즘 생성
        dst::DSTree my_ds(p.string(), {".png"});
        
        // 반복
        while(true){
            // 선택 가능한지 확인
            if (!my_ds.is_selectable()) break;
            
            // 보여주기
            my_ds.print_selectable();
            
            // 선택하기
            string folder, name;
            cout << "-----------------------------" << endl << "=>";
            cin >> folder;
            cout << endl;
            
            if (folder=="exit"){
                cout << "프로그램을 종료합니다." << endl;
                break;
            }
            cin >> name;
            if (name=="exit"){
                cout << "프로그램을 종료합니다." << endl;
                break;
            }
            
            // 선택
            int error = my_ds.select(folder, name);
            switch (error) {
                case 1: cout << path(folder)/path(name) << " 을(를) 찾을 수 없습니다." << endl << endl; break;
            }
        }
        cout << "--done.----------------------" << endl;
    }
    else if (s=="2"){
        cout << "--msig::MusicalSymbol()------" << endl;
        cout << "--done.----------------------" << endl;
    }
    else if (s=="3"){
        cout << "--msig::Note()---------------" << endl;
        
        msig::Note n = msig::Note("new-symbol-dataset");
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
        Mat img_out = msig::remove_padding(img);
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
