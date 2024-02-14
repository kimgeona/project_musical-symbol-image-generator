#include <iostream>
#include <algorithm>
#include <regex>    // 정규 표현식 라이브러리
#include <cctype>   // 소문자 변환
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


void Note::add(std::string key, std::string value){
    using namespace std;
    using namespace cv;
    
    // key 존재 확인
    if (imgs.find(key)==imgs.end()){
        cout << "msig: ";
        cout << __FUNCTION__ << "("+key+", "+value +") : ";
        cout << "("+key+") is not exist.";
        cout << endl;
        return;
    }
    
    // 정규표현식 생성
    string uint_reg = "([+]?[0-9]+)";
    string double_reg = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
    string my_reg = "^(false|"+uint_reg+"_"+uint_reg+"_"+double_reg+"_"+double_reg+")$";
    regex re(my_reg);
    
    // value 소문자로 변환
    for (int i=0; i<value.length(); i++)
        value[i] = tolower(value[i]);
    
    // value 작성문 확인 후 저장
    if (regex_match(value, re)){
        draw_list["key"] = value;
        return;
    }
    else {
        cout << "msig: ";
        cout << __FUNCTION__ << "("+key+", "+value +") : ";
        cout << "("+value+") is not in the expected format.";
        cout << endl;
        return;
    }
}


}
