#include <msig_Note.hpp>

namespace msig
{


// msig_Note_API.cpp
int     Note::set(std::string type, std::string name){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    int error = symbol_selector.select(type, name);
    
    switch (error) {
        case 1:
            cout << "Note : 폴더명을 잘못 입력하셨습니다." << endl;
            break;
        case 2:
            cout << "Note : 파일명을 잘못 입력하셨습니다." << endl;
            break;
        case -1:
            cout << "Note : good. 이제 더이상 선택할 수 있는 악상기호가 없습니다." << endl;
            break;
        case 0:
            cout << "Note : good." << endl;
            break;
    }
    
    return error;
}
void    Note::save_as_img(std::string file_name){
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}
void    Note::show(){
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
void    Note::edit_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // config 수정
    for (auto& ms : this->ds_complete){
        if (ms.edit_config()) break;    // esc 입력시 break
        save_config();                  // ds_complete, ds_piece에 저장되어 있는 모든 config들 저장.
    }
    
    // config 수정
    for (auto& ms : this->ds_piece){
        if (ms.edit_config()) break;    // esc 입력시 break
        save_config();                  // ds_complete, ds_piece에 저장되어 있는 모든 config들 저장.
    }
}


}
