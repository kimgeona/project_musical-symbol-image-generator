#include <msig_Util.hpp>

namespace msig {



// 주소 변환
std::string                 my_dir(std::filesystem::path dir){
    using namespace std;
    using namespace std::filesystem;
    
    string out = "";
    for (const auto& e : dir){
        out += "_" + e.string();
    }
    out = out.substr(1, out.length());
    return out;
}


// 터미널 grep
std::string                 my_grep(std::filesystem::path dir, std::string s){
    using namespace std;
    using namespace std::filesystem;
    
    fstream fin(dir.string(), ios::in);
    if (!fin){
        cout << "my_grep(): ";
        cout << "can't open " << dir << " for reading.";
        cout << endl;
        exit(1);
    }
    string line;
    while (getline(fin, line)) {
        if (line.find(s)!=-1){
            fin.close();
            return line;
        }
    }
    fin.close();
    return "";
}


// 파이썬 split
std::vector<std::string>    my_split(std::string s1, std::string s2){
    using namespace std;
    using namespace std::filesystem;
    
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


// 파이썬 trim
std::string                 my_trim(std::string s1){
    using namespace std;
    using namespace std::filesystem;
    
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


// 파이썬 lower
std::string                 my_lower(std::string s1){
    using namespace std;
    using namespace std::filesystem;
    
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
