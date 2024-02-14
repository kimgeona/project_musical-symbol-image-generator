#include <iostream>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


// 악상기호 정보 초기화
void Note::init_drawing_set(){
    using namespace std;
    using namespace cv;
    
    // 음표 정보
    drawing_set["type"] = "note";      // 종류 : <note|beat>
    drawing_set["pitch"] = "a4";       // 위치 : <음정>[_<bb|b|natural|#|##>]
    
    // 음표 형태
    drawing_set["head"] = "1";         // 머리 : <박자>
    drawing_set["stem"] = "false";     // 기둥 : false, <길이>
    drawing_set["tail"] = "false";     // 꼬리 : false, <박자>
    drawing_set["beam"] = "false";     // 지름대 : false, [-|+]<각도>_<start|mid|end>[_<front|back>]
    drawing_set["dot"] = "false";      // 점 : false, dot
    
    // 연주 방법 표시
    drawing_set["slur"] = "false";         // 이음줄 : false, <start|mid|0~100|end>, legato_t
    drawing_set["tie"] = "false";          // 붙임줄 : false, <start|mid|0~100|end>
    drawing_set["staccato"] = "false";     // 스타카토 : false, staccato, staccatissimo, mezzo_staccato
    drawing_set["tenuto"] = "false";       // 테누토 : false, tenuto, tenuto_t
    drawing_set["accent"] = "false";       // 악센트 : false, accent, marcato, marcato_t, rf, rfz, sf, sfz, fz
    drawing_set["fermata"] = "false";      // 늘임표 : false, fermata
    drawing_set["octave"] = "false";       // 옥타브 : false, <8|5>_<upper|lower>_<start|mid|end>
    
    // 꾸밈음 표시
    drawing_set["acciaccatura"] = "false";     // 짧은 앞꾸밈음 : false, <음정>[_<#|natural|b>]
    drawing_set["appoggiatura"] = "false";     // 긴 앞꾸밈음 : false, <음정>[_<#|natural|b>]
    drawing_set["mordent"] = "false";          // 잔결꾸밈음 : false, upper, lower
    drawing_set["trill"] = "false";            // 트릴 : false, trill
    drawing_set["turn"] = "false";             // 턴 : false, turn
    drawing_set["glissando"] = "false";        // 글리산도 : false, <start|mid|end>_[-|+]<각도>
    drawing_set["slid"] = "false";             // 슬라이드 : false, <start|mid|end>_[-|+]<각도>
    
    // 셈여림 표시
    drawing_set["dynamic"] = "false";         // 셈여림 : false, ppp, pp, p, mp, mf, f, ff, fff
    drawing_set["cresceondo"] = "false";      // 셈여림 변화 : false, <crescendo|decrescendo|diminuendo>_<start|mid|0~100|end>
    drawing_set["dynamic_change"] = "false";  // 셈여림 변화 : false, fp, pf, fi, fo
    
    // 빠르기 표시
    // ...(추후 작성)
}

// 악상기호 정규표현식 초기화
void Note::init_drawing_regexp(){
    using namespace std;
    using namespace cv;
    
    // 음표 정보
    drawing_regexp["type"] = "^(note|beat)$";
    drawing_regexp["pitch"] = "^([a-c][0-8]|[d-g][0-7])(_(bb|b|natural|#|##))?$";
    
    // 음표 형태
    drawing_regexp["head"] = "^(head_1|head_2|head_4)$";
    drawing_regexp["stem"] = "^(false|[0-9]{1,2}|[0-9]{0,2}.[0-9]{1,3}|[0-9]{1,2}.[0-9]{0,3})$";
    drawing_regexp["tail"] = "^(false|8|16|32|64|128)$";
    drawing_regexp["beam"] = "^(false|[-+]?[0-8]?[0-9]_(start|mid|end)(_(front|back))?)$";
    drawing_regexp["dot"] = "^(false|dot|.)$";
    
    // 연주 방법 표시
    drawing_regexp["slur"] = "^(false|start|mid|([0-9]?[0-9]|100)|end|legato_t)$";
    drawing_regexp["tie"] = "^(false|start|mid|([0-9]?[0-9]|100)|end)$";
    drawing_regexp["staccato"] = "^(false|staccato|staccatissimo|mezzo_staccato)$";
    drawing_regexp["tenuto"] = "^(false|tenuto|tenuto_t)$";
    drawing_regexp["accent"] = "^(false|accent|marcato|marcato_t|rf|rfz|sf|sfz|fz)$";
    drawing_regexp["fermata"] = "^(false|fermata)$";
    drawing_regexp["octave"] = "^(false|[85]_(upper|lower)_(start|mid|end))$";
    
    // 꾸밈음 표시
    drawing_regexp["acciaccatura"] = "^(false|([a-c][0-8]|[d-g][0-7])(_(b|natural|#))?)$";
    drawing_regexp["appoggiatura"] = "^(false|([a-c][0-8]|[d-g][0-7])(_(b|natural|#))?)$";
    drawing_regexp["mordent"] = "^(false|upper|lower)$";
    drawing_regexp["trill"] = "^(false|trill)$";
    drawing_regexp["turn"] = "^(false|turn)$";
    drawing_regexp["glissando"] = "^(false|(start|mid|end)_[-+]?[0-8]?[0-9])$";
    drawing_regexp["slid"] = "^(false|(start|mid|end)_[-+]?[0-8]?[0-9])$";
    
    // 셈여림 표시
    drawing_regexp["dynamic"] = "^(false|ppp|pp|p|mp|mf|f|ff|fff)$";
    drawing_regexp["cresceondo"] = "^(false)|(crescendo|decrescendo|diminuendo)_(start|mid|([0-9]?[0-9]|100)|end)$";
    drawing_regexp["dynamic_change"] = "^(false|fp|pf|fi|fo)$";
    
    // 빠르기 표시
    // ...(추후 작성)
}

// 악상기호 이미지 불러오기
void Note::init_note_imgs(){
    using namespace std;
    using namespace cv;
    using namespace std::__fs::filesystem;
    
    // file 경로 관련 변수
    path d1(".");
    path d2("note_img");
    
    // "저장되어 있는 폴더명", "이미지 이름"
    string key_value[] = {
        // line
        "ln", "staff",            // 오선
        "ln", "ledger",           // 오선 위 아래 추가적 선
        "ln", "bar",              // 단일 바
        "ln", "bar_double",       // 이중 바
        "ln", "bar_double_bold",  // 이중 바(볼드)
        "ln", "bar_dotted",       // 점선 바
        
        // note
        "nt", "1",          // 온음표
        "nt", "2",          // 2분음표
        "nt", "4",          // 4분음표
        "nt", "8",          // ...
        "nt", "16",         //
        "nt", "32",         //
        "nt", "64",         //
        "nt", "beam",       // 연속적인 꼬리
        "nt", "ghost",      // 비트 음표(?)
        
        "nt", "head_1",     // 온음표 머리
        "nt", "head_2",     // 2분음표 머리
        "nt", "head_4",     // 4분음표 머리
        "nt", "tail_1",
        "nt", "tail_2",
        "nt", "tail_4",
        "nt", "tail_8",
        "nt", "tail_16",
        "nt", "tail_32",
        "nt", "tail_64",
        "nt", "dot",

        
        // rest
        "rt", "1",
        "rt", "2",
        "rt", "4",
        "rt", "8",
        "rt", "16",
        "rt", "32",
        "rt", "64",
        "rt", "measure_rest",
        
        // breaks
        "br", "brath_mark",
        "br", "caesura",
        
        // accidental-signatures
        "as", "flat",
        "as", "flat_double",
        "as", "natural",
        "as", "sharp",
        "as", "sharp_double",
        
        // key-signatures
        "ks", "flat_1",
        "ks", "flat_2",
        "ks", "flat_3",
        "ks", "flat_4",
        "ks", "flat_5",
        "ks", "flat_6",
        "ks", "flat_7",
        "ks", "sharp_1",
        "ks", "sharp_2",
        "ks", "sharp_3",
        "ks", "sharp_4",
        "ks", "sharp_5",
        "ks", "sharp_6",
        "ks", "sharp_7",
        
        // time-signatures
        "ts", "2",
        "ts", "3",
        "ts", "4",
        "ts", "5",
        "ts", "6",
        "ts", "7",
        "ts", "8",
        "ts", "9",
        "ts", "11",
        "ts", "12",
        "ts", "16",
        "ts", "32",
        "ts", "common",
        "ts", "cut",
        
        // note-relationships
        "nr", "tie",
        "nr", "slur",
        "nr", "glissando",
        "nr", "tuplet",
        "nr", "arpeggio",
        
        // dynamics
        "dy", "pianississimo",
        "dy", "pianissimo",
        "dy", "piano",
        "dy", "mezzo_piano",
        "dy", "mezzo_forte",
        "dy", "forte",
        "dy", "fortissimo",
        "dy", "fortississimo",
        "dy", "sforzando",
        "dy", "fortepiano",
        "dy", "crescendo",
        "dy", "decrescendo",
        
        // articulation-marks
        "am", "staccato",
        "am", "staccatissimo",
        "am", "tenuto",
        "am", "fermata",
        "am", "accent",
        "am", "marcato",
        
        // ornaments
        "or", "tremolo",
        "or", "tril",
        "or", "tril_plus",
        "or", "mordent_uppter",
        "or", "mordent_lower",
        "or", "turn",
        "or", "turn_inverted",
        "or", "turn_slash",
        "or", "appoggiatura",
        "or", "acciaccatura",
        
        // octave-signs
        "os", "8_up",
        "os", "8_down",
        "os", "15_up",
        "os", "15_down",
        
        // repetitions
        "rp", "repeat_start",
        "rp", "repeat_end",
        "rp", "simile_start",
        "rp", "simile_end",
        "rp", "volta_1",
        "rp", "volta_2",
        "rp", "da-capo",
        "rp", "dal-segno",
        "rp", "segno",
        "rp", "coda",
    };
    
    // 이미지 불러오기
    for (int i=0; i<sizeof(key_value)/sizeof(string); i+=2){
        // 이미지 경로 생성
        path d3(key_value[i]);
        path d4(key_value[i+1] + ".png");
        path img_dir = d1 / d2 / d3 / d4;
        
        // key 이름 생성
        string key_name = key_value[i] + "_" + key_value[i+1];
        
        // 이미지 불러오기
        note_imgs[key_name] = imread(img_dir, IMREAD_GRAYSCALE);

        // 불러와지지 않았을때 에러
        CV_Assert(note_imgs[key_name].data);
    }
}

// 악상기호 중심좌표 설정
void Note::init_note_imgs_config(){
    using namespace std;
    using namespace cv;
}


}
