// c++17
#include <iostream>

// 나의 라이브러리
#include <msig.hpp>

void clear()
{
#ifdef _WIN32
    system("cls");
#endif
#ifdef __MACH__
    system("clear");
#endif
}
void signboard()
{
    std::string MSIGLOGO;
    MSIGLOGO += std::string(R"(MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM)") + "\n";
    MSIGLOGO += std::string(R"(M"""""`'"""`YM MP""""""`MM M""M MM'"""""`MM)") + "\n";
    MSIGLOGO += std::string(R"(M  mm.  mm.  M M  mmmmm..M M  M M' .mmm. `M)") + "\n";
    MSIGLOGO += std::string(R"(M  MMM  MMM  M M.      `YM M  M M  MMMMMMMM)") + "\n";
    MSIGLOGO += std::string(R"(M  MMM  MMM  M MMMMMMM.  M M  M M  MMM   `M)") + "\n";
    MSIGLOGO += std::string(R"(M  MMM  MMM  M M. .MMM'  M M  M M. `MMM' .M)") + "\n";
    MSIGLOGO += std::string(R"(M  MMM  MMM  M Mb.     .dM M  M MM.     .MM)") + "\n";
    MSIGLOGO += std::string(R"(MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM)");
    std::cout << MSIGLOGO << std::endl << std::endl;
    std::cout << "  Musical Symbol Image Generator" << "(ver." << MSIG_VERSION << ")" << std::endl << std::endl;
}

int main(int argc, char* argv[]) {
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
#endif
    
    std::vector<MSIG::Algorithm::MusicalSymbol> mss;
    mss.emplace_back("./ms-datasets/test3/staff-a4-0-0.png", true);
    mss.emplace_back("./ms-datasets/test3/staff-b4-0-0.png", true);
    mss.emplace_back("./ms-datasets/test3/staff-c5-0-0.png", true);
    mss.emplace_back("./ms-datasets/test3/note-up-4.png", true);
    mss.emplace_back("./ms-datasets/test3/note-up-32.png", true);
    mss.emplace_back("./ms-datasets/test3/accidental-flat.png", true);
    mss.emplace_back("./ms-datasets/test3/accidental-natural.png", true);
    mss.emplace_back("./ms-datasets/test3/articulation-staccato.png", true);
    mss.emplace_back("./ms-datasets/test3/articulation-tenuto.png", true);
    mss.emplace_back("./ms-datasets/test3/dynamic-crescendo.png", true);
    mss.emplace_back("./ms-datasets/test3/dynamic-fortepiano.png", true);
    mss.emplace_back("./ms-datasets/test3/octave-down-end.png", true);
    
    MSIG::Algorithm::MusicalSymbol ms = mss[1] + mss[3] + mss[7] + mss[8] + mss[11] + mss[7] + mss[8] + mss[6] + mss[6];
    ms.show();
    
    /*
    // 1. 프로그램 간판 출력
    clear();
    signboard();
    
    // 2. 작업 선택
    std::cout << "  작업을 선택하세요." << std::endl;
    std::cout << "  -----------------" << std::endl;
    std::cout << "  1. 기본 데이터셋 관리" << std::endl;
    std::cout << "  2. 데이터셋 생성" << std::endl;
    std::cout << "  0. 종료" << std::endl;
    std::cout << "  -----------------" << std::endl;
    std::cout << "  >> ";
    
    // 3. 작업 입력 받기
    int select;
    std::cin >> select;
    std::cin.clear();
    std::cin.ignore(1000, '\n');
    */
    return 0;
}
