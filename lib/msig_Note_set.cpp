#include <msig.hpp>

namespace msig
{


void Note::set(std::string note,
         std::string relationship,
         std::string dynamic,
         std::string articulations_mark,
         std::string ornament,
         std::string octavesign,
         std::string kegetition_and_codas)
{
    using namespace std;
    using namespace cv;
    
    // 인자값 소문자로 변환
    string_to_lower(note);
    string_to_lower(relationship);
    string_to_lower(dynamic);
    string_to_lower(articulations_mark);
    string_to_lower(ornament);
    string_to_lower(octavesign);
    string_to_lower(kegetition_and_codas);
    
    // 인자값 화인 : note가 잘 작성되었는지
    if (!regex_match(note, regex("^[cdefgab][12345678]_(false|bb|b|natural|#|##)_(1|2|4|8|16|32|64)$"))){
        cout << "msig: ";
        cout << "Note::set(), Wrong argument format. please check \"note="+note+"\".";
        cout << endl;
        return;
    }
    
    // 인자값 확인 : 존재하는 이미지인지
    for (auto symbol_name : vector<string>({relationship, dynamic, articulations_mark, ornament, octavesign, kegetition_and_codas})){
        if (symbol_name==""||symbol_name=="false") continue;
        if (img_symbols.find(symbol_name)==img_symbols.end()){
            cout << "msig: ";
            cout << "Note::set(), musical_symbol name \""+symbol_name+"\" not found.";
            cout << endl;
            return;
        }
    }
    
    // 오선 그리기
    draw_list["ln_staff"] = img_configs["ln_staff"];    // 오선(staff)
                                                        // 선(ledger)
    
    // 상호관계 파악하여 전체적으로 "x_y_각도_확대축소_대칭" 수정 후 draw_list에 추가
    draw_list["nt_8"] = img_configs["nt_8"]; // 확인을 위한 임시 코드
}


}
