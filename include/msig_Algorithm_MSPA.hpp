#pragma once
#ifndef msig_Algorithm_MSPA_hpp
#define msig_Algorithm_MSPA_hpp

// c++17
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

// 나의 라이브러리
#include <msig_MusicalSymbol.hpp>

namespace msig {
// Musical Symbol Positioning Algorithm
class MSPA
{
private:
    // 오선 이미지 포함 여부
    bool staff;
    
    // 배치 정보
    int pitch;
    
    // 배치 정보 [top, bottom, left, right]
    int edge[4];    // 오선지 경계
    int in  [4];    // 오선지 안
    int out [4];    // 오선지 밖
    int pad [4];    // 부가적인 패딩값
    
    // 입력받은 악상기호들
    std::vector<MusicalSymbol>  ms_inputs;      // 입력받은 악상기호들
    std::vector<std::string>    ms_positions;   // 입력받은 배치정보들
    
    // 완성된 악상기호
    MusicalSymbol               ms_result;
    
public:
    // 생성자
    MSPA();
    
    // 배치 함수
    void add_staff(const MusicalSymbol& ms, std::string pitch, int top_ledger_number, int bottom_ledger_number);
    void add_fixed(const MusicalSymbol& ms);
    void add_in(const MusicalSymbol& ms, std::string direction);
    void add_out(const MusicalSymbol& ms, std::string direction);
    
    // 자동 배치 함수
    void add(const MusicalSymbol& ms);
    
    // 연산
    int pitch_to_number(const std::string& pitch);
    
    // 배치 완료된 악상기호 구하기
    MusicalSymbol get();
    
    // 초기화
    void reset();
};
}


#endif /* msig_Algorithm_MSPA_hpp */
