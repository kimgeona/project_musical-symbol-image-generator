#include <msig_Note.hpp>

namespace msig
{


// msig_Note_Utility.cpp
std::vector<std::string>        Note::my_split(std::string s1, std::string s2){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return std::vector<std::string>();
    
    // s2를 기준으로 s1 분리
    vector<string> s_out;
    size_t p = 0;
    size_t n = s1.find(s2);
    while (n != string::npos) {
        s_out.push_back(s1.substr(p, n - p));
        p = n + 1;
        n = s1.find(s2, p);
    }
    s_out.push_back(s1.substr(p, s1.length() - p));
    return s_out;
}
std::string                     Note::my_trim(std::string s1){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return "";
    
    // 왼쪽 공백 조사
    int left = 0;
    while (left < s1.length() && std::isspace(s1[left])) left++;
    
    // 오른쪽 공백 조사
    int right = (int)s1.length();
    while (left < right && std::isspace(s1[right-1])) right--;
    
    // 공백이 제거된 문자열 범위 추출
    return s1.substr(left, right - left);
}
std::string                     Note::my_lower(std::string s1){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return "";
    
    // 소문자로 변환
    string s1_copy = s1;
    for (int i=0; i<s1_copy.length(); i++)
        if ('A' <= s1_copy[i] && s1_copy[i] <= 'Z')
            s1_copy[i] += 32;
    return s1_copy;
}


}
