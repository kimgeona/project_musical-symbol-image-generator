#include <msig_Canvas.hpp>

namespace msig
{


// 그릴 음표 선택
int     Canvas::select(std::filesystem::path dir)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 path를 ds_complete에서 찾기
    if (this->ds_complete.find(dir)==ds_complete.end())
    {
        // 못찾은 경우
        cout << "Canvas::select() : 그리려고 하는 이미지 " << dir << "가 데이터셋에 없습니다." << endl;
        return -1;
    }
    else
    {
        // 찾았을 경우
        this->draw_list.push_back(this->ds_complete[dir]);  // draw_list에 해당 이미지 추가
        return 0;
    }
}


// 음표 선택 초기화
void    Canvas::select_celar()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 선택한 음표 내역 비우기
    this->draw_list = vector<MusicalSymbol>();
}


// 그린 이미지 확인
void    Canvas::show()
{
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    string title = "Canvas";
    
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


// 그린 이미지 저장
void    Canvas::save(std::string file_name)
{
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}


}
