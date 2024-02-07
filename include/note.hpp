#include <vector>
#include <opencv2/opencv.hpp>

namespace note
{


using namespace std;
using namespace cv;

class Page{
private:
    // page(악보) 데이터 (작성중)
    string name;
    string number;
    int size_w;
    int size_h;
    
    // note 데이터 저장 벡터
    vector<string>  buffer_type;        // 음표 종류 : note, beat, clef, tone, time, bar
    vector<string>  buffer_pitch;       // 음정 : c4, a5, ...
    vector<string>  buffer_beat;        // 박자 : 2-3(2박자-3개로쪼개기), 1-1, 0.5-1, ...
    vector<string>  buffer_clef;        // 음자리표 : G(높은), F(낮은), C(가온)
    vector<string>  buffer_tonality;    // 조성 : A-M(A Major), A-m(A minor)
    vector<string>  buffer_time;        // 박자표 : 4/4 4/3
    
public:
    // 생성자 (작성중)
    Page();
    
    // note 데이터 추가 함수
    void add_type(string type);
    void add_pitch(string pitch);
    void add_beat(string beat);
    void add_clef(string clef);
    void add_tonality(string tonality);
    void add_time(string time);
    void add(string type, string pitch, string beat, string clef, string tonality, string time);
    
    // note 데이터 오류 확인 함수
    bool check_buffer_count();   // buffer에 저장된 갯수 서로 올바른지 확인
    bool check_measure();       // 마디 갯수 올바른지 확인
    bool check();               // 악보가 잘 작성되었는지 확인(전체 확인)
    
    // 악보 이미지 생성 함수
    Mat draw();                         // 악보 그리기
    void save_img(string file_name);    // 그린 내용을 저장
    void show_img();                    // 그린 내용을 화면에 보여주기
    Mat cv_img();                       // 그린 내용을 cv::Mat() 형식으로 반환
};


}
