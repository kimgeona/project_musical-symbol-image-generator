
#include <msig_Utility.hpp>

namespace MSIG {
namespace Utility {

std::string
grep(std::filesystem::path filePath, std::string grepString) {
    namespace fs = std::filesystem;
    
    // 1. 파일 열기
    std::fstream fin(filePath.string(), std::ios::in);
    if (!fin)
        throw std::runtime_error("MSIG::Utility::grep() : 파일을 열 수 없습니다.");
    
    // 2. grepString가 포함된 첫 문자열 찾아서 반환
    std::string line;
    while (getline(fin, line)) {
        if (line=="")
            continue;
        if (split(line, "=")[0]==grepString) {
            fin.close();
            return line;
        }
    }
    return "";
}

void
attach(std::filesystem::path filePath, std::string attachString) {
    namespace fs = std::filesystem;
    
    // 1. 파일 열기
    std::fstream fout(filePath.string(), std::ios::out|std::ios::app);
    if (!fout)
        throw std::runtime_error("MSIG::Utility::attach() : 파일을 열 수 없습니다.");
    
    // 2. 파일의 맨 뒤에 해당 문자열 추가
    fout << std::endl << attachString;
    fout.close();
}

std::vector<std::string>
split(std::string str, std::string splitStr) {
    namespace fs = std::filesystem;
    
    // 1. 빈 문자열 검사
    if (str.empty()) return std::vector<std::string>();
    
    // 2. str을 splitStr로 분할하여 벡터에 저장 후 반환.
    std::vector<std::string> result;
    size_t p = 0;
    size_t n = str.find(splitStr);
    while (n != std::string::npos) {
        result.push_back(str.substr(p, n-p));
        p = n + 1;
        n = str.find(splitStr, p);
    }
    result.push_back(str.substr(p, str.length()-p));
    
    // 3. 문자열 벡터의 문자열들 좌우 공백 제거
    for (auto& str : result) {
        str = trim(str);
    }
    return result;
}

std::vector<std::vector<std::string>>
split(std::string str, std::string splitStr, std::string splitStr2) {
    namespace fs = std::filesystem;
    
    // 1. 빈 문자열 검사
    if (str.empty()) return std::vector<std::vector<std::string>>();
    
    // 2. str을 splitStr로 분할하여 벡터에 저장 후 반환.
    std::vector<std::string> strs;
    size_t p = 0;
    size_t n = str.find(splitStr);
    while (n != std::string::npos) {
        strs.push_back(str.substr(p, n-p));
        p = n + 1;
        n = str.find(splitStr, p);
    }
    strs.push_back(str.substr(p, str.length()-p));
    
    // 3. strs을 splitStr2로 분할하여 벡터에 저장 후 반환.
    std::vector<std::vector<std::string>> result;
    for (auto& s : strs) {
        result.push_back(split(s, splitStr2));
    }
    
    return result;
}

std::string
trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();

    // 좌측 공백 제거
    while (start < end && std::isspace(str[start])) {
        ++start;
    }

    // 우측 공백 제거
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    // 새로운 문자열 반환
    return str.substr(start, end - start);
}

}
}
