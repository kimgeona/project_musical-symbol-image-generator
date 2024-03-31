#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
#ifdef __MACH__
    cout << "Platform : MacOS" << endl << endl;
#endif
#ifdef _WIN32
    cout << "Platform : Windows" << endl << endl;
    system("chcp 65001");
#endif
    
    string s = "";
    cout << "enter the number that you want to test." << endl;
    cout << "---- Geona Kim --------------" << endl;
    cout << "1)dst::DSTree()"               << endl;
    cout << "2)msig::MusicalSymbol()"       << endl;
    cout << "3)msig::Note()"                << endl;
    cout << "---- Junhyuck yim ------------" << endl;
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
            
            // 선택할 내용 입력받기
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
        // Note 생성
        msig::Note note = msig::Note("new-symbol-dataset");
        // 입력 받기
        while (true) {
            // 그릴 수 있는 음표 확인
            if (!note.is_setable()) break;
            
            // 보여주기
            note.print_setable();
            
            // 선택할 내용 입력받기
            string type, name;
            cout << "-----------------------------" << endl << "=>";
            cin >> type >> name;
            
            // 선택
            int error = note.set(type, name);
            switch (error) {
                case 1: cout << path(type)/path(name) << " 을(를) 찾을 수 없습니다." << endl << endl; break;
            }
            
            // 현재까지 그려진 이미지 보여주기
            note.show();
        }
        // 생성된 이미지 저장
        note.save_as_img("test.png");
        // 모든 창 종료
        destroyAllWindows();
        
        cout << "--done.----------------------" << endl;
    }
    else if (s=="4"){
        cout << "--msig::remove_padding()-----" << endl;
        
        Mat img = imread(R"(여따 경로 지정해서 테스트 하세용)", IMREAD_GRAYSCALE);
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
