#include <msig_Canvas.hpp>

namespace msig
{


// 그릴 음표 설정
int     Canvas::set(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 해당 이미지(주소)가 dir_ds_complete에 존재하는지 확인
    
    // 찾았을 경우
    if (this->ds_complete.find(dir)!=ds_complete.end()){
        // draw_list에 해당 이미지 추가
        this->draw_list.push_back(this->ds_complete[dir]);
    }
    // 찾지 못했을 경우
    else {
        // 해당 이미지(주소)를 조합으로 생성 가능한지 확인
        cout << "해당 이미지 : " << dir << "은 생성해서 draw_list에 추가합니다." << endl;
    }
    
    return 0;
}


// 그린 내용을 file_name 이름으로 저장
void    Canvas::save_as_img(std::string file_name){
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}


// 그린 내용을 화면에 보여주기
void    Canvas::show(){
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
cv::Mat Canvas::cv_Mat(){
    using namespace std;
    using namespace cv;
    
    return draw();
}


}
