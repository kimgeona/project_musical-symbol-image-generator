#include <map>
#include <opencv2/opencv.hpp>

namespace msig
{


using namespace std;
using namespace cv;

class Note{
private:
    // 참고 : https://geumse.tistory.com/14
    // 참고 : https://www.masterclass.com/articles/music-ornaments-guide
    
    // note 데이터
    map<string, string> data;
    map<string, string> data_reg_exp;
    
    // 악보 그리기
    Mat draw();
    
public:
    // 생성자
    Note(){
        // ========= 데이터 저장 =========
        
        // 음표 정보
        data["type"] = "note";      // 종류 : <note|beat>
        data["place"] = "a4";       // 위치 : <음정>[_<bb|b|natural|#|##>]
        data["head"] = "1";         // 머리 : <박자>
        data["stem"] = "false";     // 기둥 : false, <길이>
        data["tail"] = "false";     // 꼬리 : false, <박자>
        data["beam"] = "false";     // 지름대 : false, <각도>
        data["dot"] = "false";      // 점 : false, dot
        
        // 연주 방법 표시
        data["slur"] = "false";         // 이음줄 : false, <start|mid|0~100|end>, legato_t
        data["tie"] = "false";          // 붙임줄 : false, <start|mid|0~100|end>
        data["staccato"] = "false";     // 스타카토 : false, staccato, staccatissimo, mezzo_staccato
        data["tenuto"] = "false";       // 테누토 : false, tenuto, tenuto_t
        data["accent"] = "false";       // 악센트 : false, accent, marcato, marcato_t, rf, rfz, sf, sfz, fz
        data["fermata"] = "false";      // 늘임표 : false, fermata
        data["octave"] = "false";       // 옥타브 : false, <8|5>_<upper|lower>_<start|mid|end>
        
        // 꾸밈음 표시
        data["acciaccatura"] = "false";     // 짧은 앞꾸밈음 : false, <음정>[_<#|natural|b>]
        data["appoggiatura"] = "false";     // 긴 앞꾸밈음 : false, <음정>[_<#|natural|b>]
        data["mordent"] = "false";          // 잔결꾸밈음 : false, upper, lower
        data["trill"] = "false";            // 트릴 : false, trill
        data["turn"] = "false";             // 턴 : false, turn
        data["glissando"] = "false";        // 글리산도 : false, <start|mid|0~100|end>_<각도>
        data["slid"] = "false";             // 슬라이드 : false, <start|mid|0~100|end>_<각도>
        
        // 셈여림 표시
        data["dynamic"] = "false";         // 셈여림 : false, ppp, pp, p, mp, mf, f, ff, fff
        data["cresceondo"] = "false";      // 셈여림 변화 : false, <cresendo|cresendo|diminuendo>_<start|mid|0~100|end>
        data["dynamic_change"] = "false";  // 셈여림 변화 : false, fp, pf, fi, fo
        
        // 빠르기 표시
        // ...(추후 작성)
        
        
        // ========= 데이터 확인 정규표현식 =========
        
        // 음표 정보
        data["type"] = "^(note|beat)$";
        data["place"] = "";
        data["head"] = "";
        data["stem"] = "";
        data["tail"] = "";
        data["beam"] = "";
        data["dot"] = "";
        
        // 연주 방법 표시
        data["slur"] = "";
        data["tie"] = "";
        data["staccato"] = "";
        data["tenuto"] = "";
        data["accent"] = "";
        data["fermata"] = "";
        data["octave"] = "";
        
        // 꾸밈음 표시
        data["acciaccatura"] = "";
        data["appoggiatura"] = "";
        data["mordent"] = "";
        data["trill"] = "";
        data["turn"] = "";
        data["glissando"] = "";
        data["slid"] = "";
        
        // 셈여림 표시
        data["dynamic"] = "";
        data["cresceondo"] = "^(false)|(crescendo|decrescendo|diminuendo)_(start|mid|(?:100|\\d{1,2})|end)$";
        data["dynamic_change"] = "";
    }
    
    // note 데이터 설정
    void set(string key, string value);
    
    // 악보 이미지 생성 함수
    void save_as_img(string file_name);     // 그린 내용을 file_name 이름으로 저장
    void show();                            // 그린 내용을 화면에 보여주기
    Mat cv_Mat();                           // 그린 내용을 cv::Mat() 형식으로 반환
};


}
