// c++17
#include <iostream>
#include <filesystem>

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
    std::cout << "  Musical Symbol Image Generator" << "(ver." << MSIG_VERSION << ")" << std::endl << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
    namespace fs = std::filesystem;
    
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
#endif
    
    // 프로그램 이름 출력
    signboard();
    
    // 현재 디렉토리에 있는 이미지들 조사
    std::vector<fs::path> vp;
    for (auto& p : fs::directory_iterator(fs::path(".")))
    {
        if (fs::exists(p.path()) &&                 // 존재하는지
            fs::is_regular_file(p.path()) &&        // 파일인지
            p.path().extension().string()==".png")  // .png 파일인지
        {
            vp.push_back(p.path());
        }
    }
    // 작업 수행
    if (vp.empty())
    {
        std::cout << "  ! 현재 디렉토리에는 \".png\" 이미지가 존재하지 않습니다." << std::endl;
        std::cout << "  ! 프로그램을 종료합니다." << std::endl << std::endl;
        return 0;
    }
    
    // 작업 선택
    std::cout << "  수행할 작업을 선택하세요." << std::endl;
    std::cout << std::endl;
    std::cout << "  0 : 추가된 이미지들 config 데이터 생성" << std::endl;
    std::cout << "  1 : 모든 이미지 config 데이터 수정" << std::endl;
    std::cout << "  --------------------------------" << std::endl;
    std::cout << "  >> ";
    
    // 사용자 선택
    std::string select;
    std::cin >> select;
    std::cout << std::endl;
    
    // 작업 수행
    if (select=="0")
    {
        // 1. 새로 추가된 이미지들 __config__ 데이터 생성
        // 1.1 MusicalSymbol 객체 생성으로 config 생성. (이미 config 데이터가 존재하는 이미지들은 수정 화면이 뜨지 않음.)
        for (auto& p : vp) {
            MSIG::Algorithm::MusicalSymbol(p, true);
        }
        
        // 완료
        std::cout << "  - 작업 완료." << std::endl;
    }
    else if (select=="1")
    {
        // 2. 모든 이미지들 __config__ 데이터 수정
        for (auto& p : vp) {
            MSIG::Algorithm::MusicalSymbol ms(p, false);
            ms.edit_config();
        }
        
        // 완료
        std::cout << "  - 작업 완료." << std::endl;
    }
    
    // 프로그램 종료
    std::cout << std::endl << std::endl << "  - 프로그램을 종료합니다." << std::endl;
    
    return 0;
}
