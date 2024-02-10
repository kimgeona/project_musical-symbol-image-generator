#include <regex>    // 정규 표현식 라이브러리
#include <cctype>   // 소문자 변환
#include <msig.hpp>

namespace msig
{


void Note::set(string key, string value){
    // 정규 표현식 저장
    regex re(data_reg_exp[key]);
    
    // value 소문자로 변환
    for (int i=0; i<value.length(); i++)
        value[i] = tolower(value[i]);
    
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
        if (regex_match(value, re)){
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
