#include <regex> // 정규 표현식 라이브러리
#include <msig.hpp>

namespace msig
{


void Note::set(string key, string value){
    // 정규 표현식 저장
    regex re(data_reg_exp[key]);
    
    // key bad.
    if (data.find(key) == data.end()){
        // 에러 출력
        cout << "msig: ";
        cout << __FUNCTION__ << "("+key+", "+value +") : ";
        cout << "("+key+") is not exist.";
        cout << endl;
        return;
    }
    // key good.
    else {
        // value good.
        if (regex_match("crescendo_start", re)){
            // 값 저장
            data["key"] = value;
            return;
        }
        // value bad.
        else {
            // 에러 출력
            cout << "msig: ";
            cout << __FUNCTION__ << "("+key+", "+value +") : ";
            cout << "("+value+") is not in the expected format.";
            cout << endl;
            return;
        }
    }
}


}
