#include <msig_MusicalSymbol.hpp>

namespace msig
{


// msig_MusicalSymbol_Utility.cpp
std::string                 MusicalSymbol::my_dir(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    string out = "";
    for (const auto& e : dir){
        out += "_" + e.string();
    }
    out = out.substr(1, out.length());
    return out;
}
std::string                 MusicalSymbol::my_grep(std::filesystem::path dir, std::string s){
    using namespace std;
    using namespace cv;
    
    fstream fin(dir.string(), ios::in);
    if (!fin){
        cout << "msig: ";
        cout << "Note(): ";
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
std::vector<std::string>    MusicalSymbol::my_split(std::string s1, std::string s2){
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


}
