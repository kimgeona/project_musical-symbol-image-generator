#pragma once
#ifndef msig_Algorithm_MSPA_hpp
#define msig_Algorithm_MSPA_hpp

#include <iostream>


namespace msig {


// Musical Symbol Positioning Algorithm
class MSPA
{
    // 고정에 의한 지정
    // 규칙에 의한 지정
    size_t top      = -1;
    size_t bottom   = -1;
    size_t left     = -1;
    size_t right    = -1;
    
    // 크기에 의한 지정
    
public:
    
    MSPA();
    MSPA(int t, int b, int l, int r){
        // 공간 확보
        this->top       << t;
        this->bottom    << b;
        this->left      << l;
        this->right     << r;
        this->top       = ~this->top;
        this->bottom    = ~this->bottom;
        this->left      = ~this->left;
        this->right     = ~this->right;
    }
    
    // 자리가 남았는지 확인하는 함수들..?
    // 자리가 꽉찼는지 확인하는 함수들..?
};


}


#endif /* msig_Algorithm_MSPA_hpp */