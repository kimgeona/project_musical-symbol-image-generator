#include <msig_Note.hpp>

namespace msig
{


// msig_Note_API.cpp
void    Note::print_setable(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    symbol_selector.print_selectable();
}
bool    Note::is_setable(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    return symbol_selector.is_selectable();
}
int     Note::set(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    int error = symbol_selector.select(dir);
    if (error==0){
        // ds_complete에서 주소가 p인 ms를 찾아서 draw_list에 추가
    }
    
    return error;
}
int     Note::set(std::string type, std::string name){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 선택 가능 목록 가져오기
    vector<path> list_file = symbol_selector.get_selectable();
    
    // 선택 가능한 목록이 없다면
    if (list_file.empty()){
        cout << "Note : error. there is no left to choose." << endl;
        return -1;
    }
    
    // 찾기
    for (auto& p : list_file){
        if (p.parent_path().filename().string()==type && p.filename().string()==name){
            // 찾은 경우
            int error = symbol_selector.select(p.string());
            if (error==0){
                // ds_complete에서 주소가 p인 ms를 찾아서 draw_list에 추가
            }
            
            return error;
        }
    }

    // 찾지 못한 경우
    cout << "Note : error. wrong input." << endl;
    return 1;
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
    
    // 윈도우에 이미지 그리기
    imshow(title, image);
    
    // 키보드 입력시 종료
    waitKey();
    
    // 윈도우 없애기
    destroyWindow(title);
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
    for (auto& m : this->ds_complete){
        if (m.second.edit_config()) break;  // esc 입력시 break
        save_config();                      // ds_complete, ds_piece에 저장되어 있는 모든 config들 저장.
    }
    
    // config 수정
    for (auto& m : this->ds_piece){
        if (m.second.edit_config()) break;  // esc 입력시 break
        save_config();                      // ds_complete, ds_piece에 저장되어 있는 모든 config들 저장.
    }
}


}
