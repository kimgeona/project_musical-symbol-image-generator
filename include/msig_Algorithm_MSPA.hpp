#pragma once
#ifndef msig_Algorithm_MSPA_hpp
#define msig_Algorithm_MSPA_hpp

// c++17
#include <iostream>

// 나의 라이브러리
#include <msig_MusicalSymbol.hpp>

namespace msig {
// Musical Symbol Positioning Algorithm
class MSPA
{
private:
    // 배치 정보
    MusicalSymbol               ms_buff;
    std::vector<std::string>    ms_except;
    
    // 악상기호 히스토리
    std::map<std::string, MusicalSymbol> ms_list;
    
public:
    
    void add(MusicalSymbol& ms, std::string position, std::vector<std::string> except=std::vector<std::string>());
    void regenerate(std::vector<std::string> except);
    MusicalSymbol get(void);
};
}


#endif /* msig_Algorithm_MSPA_hpp */
