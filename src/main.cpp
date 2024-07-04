// c++17
#include <iostream>

// 나의 라이브러리
#include <msig.hpp>

int main(int argc, char* argv[])
{
    // Musical Symbol Image Generator 생성
    msig::MSIG my_msig;
    
    // 프로그램 실행
    my_msig.prepare_default_dataset()   ||  // 데이터셋 준비
    my_msig.prepare_DST()               ||  // DST 알고리즘 준비
    my_msig.prepare_Canvas()            ||  // Canvas 준비
    my_msig.making_dataset();               // 데이터셋 생성
    
    // 프로그램 종료
    std::cout << "  - 프로그램을 종료합니다." << std::endl;
    
    return 0;
}
