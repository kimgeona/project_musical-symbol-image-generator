#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


// 악상기호 정보 초기화
void Note::init_data(){
    using namespace std;
    using namespace cv;
    
    // 음표 정보
    data["type"] = "note";      // 종류 : <note|beat>
    data["pitch"] = "a4";       // 위치 : <음정>[_<bb|b|natural|#|##>]
    
    // 음표 형태
    data["head"] = "1";         // 머리 : <박자>
    data["stem"] = "false";     // 기둥 : false, <길이>
    data["tail"] = "false";     // 꼬리 : false, <박자>
    data["beam"] = "false";     // 지름대 : false, [-|+]<각도>_<start|mid|end>[_<front|back>]
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
    data["glissando"] = "false";        // 글리산도 : false, <start|mid|end>_[-|+]<각도>
    data["slid"] = "false";             // 슬라이드 : false, <start|mid|end>_[-|+]<각도>
    
    // 셈여림 표시
    data["dynamic"] = "false";         // 셈여림 : false, ppp, pp, p, mp, mf, f, ff, fff
    data["cresceondo"] = "false";      // 셈여림 변화 : false, <crescendo|decrescendo|diminuendo>_<start|mid|0~100|end>
    data["dynamic_change"] = "false";  // 셈여림 변화 : false, fp, pf, fi, fo
    
    // 빠르기 표시
    // ...(추후 작성)
}

// 악상기호 정규표현식 초기화
void Note::init_data_reg_exp(){
    using namespace std;
    using namespace cv;
    
    // 음표 정보
    data_reg_exp["type"] = "^(note|beat)$";
    data_reg_exp["pitch"] = "^([a-c][0-8]|[d-g][0-7])(_(bb|b|natural|#|##))?$";
    
    // 음표 형태
    data_reg_exp["head"] = "^(head_1|head_2|head_4)$";
    data_reg_exp["stem"] = "^(false|[0-9]{1,2}|[0-9]{0,2}.[0-9]{1,3}|[0-9]{1,2}.[0-9]{0,3})$";
    data_reg_exp["tail"] = "^(false|8|16|32|64|128)$";
    data_reg_exp["beam"] = "^(false|[-+]?[0-8]?[0-9]_(start|mid|end)(_(front|back))?)$";
    data_reg_exp["dot"] = "^(false|dot|.)$";
    
    // 연주 방법 표시
    data_reg_exp["slur"] = "^(false|start|mid|([0-9]?[0-9]|100)|end|legato_t)$";
    data_reg_exp["tie"] = "^(false|start|mid|([0-9]?[0-9]|100)|end)$";
    data_reg_exp["staccato"] = "^(false|staccato|staccatissimo|mezzo_staccato)$";
    data_reg_exp["tenuto"] = "^(false|tenuto|tenuto_t)$";
    data_reg_exp["accent"] = "^(false|accent|marcato|marcato_t|rf|rfz|sf|sfz|fz)$";
    data_reg_exp["fermata"] = "^(false|fermata)$";
    data_reg_exp["octave"] = "^(false|[85]_(upper|lower)_(start|mid|end))$";
    
    // 꾸밈음 표시
    data_reg_exp["acciaccatura"] = "^(false|([a-c][0-8]|[d-g][0-7])(_(b|natural|#))?)$";
    data_reg_exp["appoggiatura"] = "^(false|([a-c][0-8]|[d-g][0-7])(_(b|natural|#))?)$";
    data_reg_exp["mordent"] = "^(false|upper|lower)$";
    data_reg_exp["trill"] = "^(false|trill)$";
    data_reg_exp["turn"] = "^(false|turn)$";
    data_reg_exp["glissando"] = "^(false|(start|mid|end)_[-+]?[0-8]?[0-9])$";
    data_reg_exp["slid"] = "^(false|(start|mid|end)_[-+]?[0-8]?[0-9])$";
    
    // 셈여림 표시
    data_reg_exp["dynamic"] = "^(false|ppp|pp|p|mp|mf|f|ff|fff)$";
    data_reg_exp["cresceondo"] = "^(false)|(crescendo|decrescendo|diminuendo)_(start|mid|([0-9]?[0-9]|100)|end)$";
    data_reg_exp["dynamic_change"] = "^(false|fp|pf|fi|fo)$";
    
    // 빠르기 표시
    // ...(추후 작성)
}

// 악상기호 이미지 불러오기
void Note::init_data_img(){
    using namespace std;
    using namespace cv;
    using namespace std::__fs::filesystem;
    
    path d1(".");
    path d2("note_img");
    path d5(".png");
    
    // "저장되어 있는 폴더명", "이미지 이름"
    // https://en.wikipedia.org/wiki/List_of_musical_symbols
    string key_value[] = {
        // 선 이미지
        "line", "staff",    // 오선
        "line", "ledger",   // 오선 위 아래 추가적 선
        //"line", "bar",      // 단일 바
        //"line", "bar_2",    // 이중 바
        //"line", "bar_3",    // 이중 바(볼드)
        //"line", "bar_4",    // 점선 바
        // ...
        
        // 음표 머리 이미지
        "head", "head_1",   // 온음표
        "head", "head_2",   // 2분음표
        "head", "head_4",   // 4분음표
        
        // 기둥
        // ...(직접 그릴 것)
        
        // 꼬리
        "tail", "tail_8",       // 8분음표 꼬리
        "tail", "tail_16",      // 16분음표 꼬리
        "tail", "tail_32",      // 32분음표 꼬리
        "tail", "tail_64",      // 64분음표 꼬리
        "tail", "tail_128",     // 128분음표 꼬리
        
        // beam
        // ...(직접 그릴 것)
        
        // 점
        "dot", "dot"
        
        // 조표
        "accidental", "flat",       // b
        "accidental", "flat_2",     // bb
        "accidental", "natural",    // natural
        "accidental", "sharp",      // #
        "accidental", "sharp_2",    // ##(=x)
        
        // 꾸밈음
        "ornaments", "mordent_1"    // 높은 모르덴트
        "ornaments", "mordent_2"    // 낮은 모르덴트
        "ornaments", "trill"        // 트릴
        "ornaments", "trill_t"      // 트릴(텍스트)
        "ornaments", "turn"         // 턴
        "ornaments", "glissando_1"  // 글리산도: 두꺼운 실선
        "ornaments", "glissando_2"  // 글리산도: 물결
        "ornaments", "glissando_3"  // 글리산도: 실선
        "ornaments", "glissando_4"  // 글리산도: 실선 위에 텍스트
        
        
    };
    
    // 이미지 불러오기
    for (int i=0; i<sizeof(key_value)/sizeof(string); i+=2){
        // 이미지 경로 생성
        path d3(key_value[i]);
        path d4(key_value[i+1]);
        path img_dir = d1 / d2 / d3 / d4 / d5;
        
        // 이미지 불러오기
        data_img[key_value[i+1]] = imread(img_dir, IMREAD_GRAYSCALE);
        
        // 이미지 불러와졌는지 확인
        // 불러와지지 않았을때 에러 처리하는 구문 추가하기
        //CV_Assert(data_img[key_value[i]].data);
    }
}

// 악상기호 중심좌표 설정
void Note::init_data_img_center(){
    using namespace std;
    using namespace cv;
}


}
