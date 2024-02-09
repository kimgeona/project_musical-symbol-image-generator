#include <regex> // 정규 표현식 라이브러리
#include <msig.hpp>

namespace msig
{


void Note::set(string key, string value){
    // key bad.
    if (data.find(key) == data.end()){
        cout << "msig: Bad access. key:" << key << " is not exist." << endl;
        return;
    }
    // key good, value good.
    else if (regex_match(value, data_reg_exp[key])){
        data['key'] = value;
        return;
    }
    // key good, value bad.
    else {
        cout << "msig: Invalid argument input. Please check the input format according to the key:" << key << "." << endl;
        return;
    }
}


}
