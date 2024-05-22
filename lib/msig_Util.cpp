#include <msig_Util.hpp>

namespace msig
{


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


// 특정 문자열을 포함하는 라인 제거
void                        my_filter_out(std::filesystem::path dir, std::string s){
    using namespace std;
    using namespace std::filesystem;
    
    // 파일 내용들
    vector<string> file;
    
    // 파일 읽기
    fstream fin(dir.string(), ios::in);
    if (!fin){
        cout << "my_filter_out(): ";
        cout << "can't open " << dir << " for reading.";
        cout << endl;
        exit(1);
    }
    string line;
    while (getline(fin, line)) {
        if (line.find(s)!=std::string::npos)    continue;
        else                                    file.push_back(line);
    }
    fin.close();
    
    // 기존 파일 제거
    remove(dir);
    
    // 파일 쓰기
    fstream fout(dir.string(), ios::out|ios::app);
    if (!fout){
        cout << "my_filter_out(): ";
        cout << "can't open " << dir << " for writing.";
        cout << endl;
        exit(1);
    }
    for(auto& s : file){
        if (s=="") continue;
        fout << endl << s;
    }
    fout.close();
}


// 특정 문자열을 파일 맨 뒤에다 붙이기
void                        my_attach(std::filesystem::path dir, std::string s){
    using namespace std;
    using namespace std::filesystem;
    
    // 파일 쓰기
    fstream fout(dir.string(), ios::out|ios::app);
    if (!fout){
        cout << "my_filter_out(): ";
        cout << "can't open " << dir << " for writing.";
        cout << endl;
        exit(1);
    }
    fout << endl << s;
    fout.close();
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
