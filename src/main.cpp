// c++17
#include <iostream>

// 나의 라이브러리
#include <msig.hpp>

int main(int argc, char* argv[])
{
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
#endif
    
    // 프로그램 정보 출력
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
    
    // 정보 입력받기
    std::string datasetDirectory;
    std::cout << "  > 기본 데이터셋 경로 : " << std::endl;;
    //std::cin >> datasetDirectory;
    std::string newDatasetDirectory;
    std::cout << "  > 생성될 데이터셋 경로 : " << std::endl;;
    //std::cin >> newDatasetDirectory;
    std::cout << std::endl;

    // DependentSelectionTree 생성
    MSIG::Algorithm::DependentSelectionTree dst("./ms-datasets/note-recognition");

    // 이미지 조합 생성
    std::vector<std::vector<std::filesystem::path>> vvp;
    dst.pick(vvp, true);
    
    return 0;
}
