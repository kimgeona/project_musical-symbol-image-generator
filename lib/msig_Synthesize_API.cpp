#include <msig_Synthesize.hpp>

namespace msig
{


// 그릴 수 있는 음표들 출력
void    Note::print_setable(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    symbol_selector.print_selectable();
}


// 음표를 그릴 수 있는지 확인
bool    Note::is_setable(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    return symbol_selector.is_selectable();
}


// 그릴 음표 설정
int     Note::set(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 해당 이미지(주소) 의존적 선택
    int error = symbol_selector.select(dir);
    
    // 의존적 선택 성공시
    if (error==0){
        // ds_complete에서 주소가 p인 ms를 찾아서 draw_list에 추가
        this->draw_list.push_back(this->ds_complete[path(dir)]);
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
    if (list_file.empty()) return -1;
    
    // 찾기
    for (auto& p : list_file){
        if (p.parent_path().filename().string()==type && p.filename().string()==name){
            // 찾은 경우,
            // 해당 이미지(주소) 의존적 선택
            int error = symbol_selector.select(p.string());
            if (error==0){
                // ds_complete에서 주소가 p인 ms를 찾아서 draw_list에 추가
                this->draw_list.push_back(this->ds_complete[p]);
            }

            return error;
        }
    }

    // 찾지 못한 경우
    return 1;
}


// 그린 내용을 file_name 이름으로 저장
void    Note::save_as_img(std::string file_name){
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}


// 그린 내용을 화면에 보여주기
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


// 그린 내용을 cv::Mat() 형식으로 반환
cv::Mat Note::cv_Mat(){
    using namespace std;
    using namespace cv;
    
    return draw();
}


}
