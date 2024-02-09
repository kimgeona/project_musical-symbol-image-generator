#include <regex> // 정규 표현식 라이브러리
#include <msig.hpp>

namespace msig
{


void print_error(string function_name, string title, string explanation){
    cout << "msig: ";
    cout << function_name << "(), ";
    cout << title << " " << explanation << " ";
    cout << endl;
}


}
