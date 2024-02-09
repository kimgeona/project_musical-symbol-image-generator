#include <regex> // 정규 표현식 라이브러리
#include <msig.hpp>

namespace msig
{


void Note::set(string key, string value){
    // 정규 표현식 저장
    regex re(data_reg_exp[key]);
    
    // key bad.
    if (data.find(key) == data.end()){
        print_error(__FUNCTION__, "Bad access.", "[key:"+key+"] is not exist.");
        return;
    }
    // key good, value good.
    else if (regex_match(value, re)){
        data["key"] = value;
        return;
    }
    // key good, value bad.
    else {
        print_error(__FUNCTION__, "Invalid argument input.", "Please check the input format according to the [key:"+key+"].");
        return;
    }
}


}
