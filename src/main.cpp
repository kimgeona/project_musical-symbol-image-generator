// c++17
#include <iostream>
#include <filesystem>

// MSIG UI
void page();

// MSIG UI 실행
int main(int argc, char* argv[])
{
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
#endif
    
    // 메인 페이지 실행
    page();
    
    return 0;
}

