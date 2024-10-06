// c++17
#include <iostream>
#include <filesystem>

// 외부 라이브러리
#include <opencv2/core/utils/logger.hpp>

// 나의 라이브러리
#include <msig.hpp>

// 데이터셋 이름
std::string datasetName = "";

// 콘솔 창 전부 지움
void clear() {
#ifdef _WIN32
    system("cls");
#endif
#ifdef __MACH__
    system("clear");
#endif
}

// 프로그램 정보 출력
void signboard() {
    std::string MSIGLOGO;
    MSIGLOGO += std::string(R"(   ____  ____  ____  ____)") + "\n";
    MSIGLOGO += std::string(R"(  ||M ||||S ||||I ||||G ||)") + "\n";
    MSIGLOGO += std::string(R"(  ||__||||__||||__||||__||)") + "\n";
    MSIGLOGO += std::string(R"(  |/__\||/__\||/__\||/__\|)");
    std::cout << MSIGLOGO << std::endl << std::endl;
    std::cout << "  Musical Symbol Image Generator" << "(ver." << MSIG_VERSION << ")" << std::endl << std::endl;
}

// 사용자로부터 메뉴 선택을 입력받음
size_t menu(const std::vector<std::string>& items, const std::string& selectMessage) {
    while (true)
    {
        // 현재 폴더 출력
        if (!datasetName.empty())
        {
            //std::cout << "  현재 데이터셋: " << datasetName << "" << std::endl << std::endl;
        }
        
        // 메뉴 항목 출력
        std::cout << "  >------------------------------<" << std::endl;
        for (size_t i = 0; i < items.size(); ++i)
        {
            std::cout << "   " << i << ". " << items[i] << std::endl;
        }
        std::cout << "  >------------------------------<" << std::endl;
        std::cout << "   " << selectMessage << ": ";
        
        // 키 입력
        size_t input;
        std::cin >> input;
        if (std::cin.fail()) {
            std::cin.clear();                                                   // 오류 플래그를 제거
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            continue;
        }
        
        if (0 <= input && input <= items.size()) {
            std::cout << std::endl << std::endl;
            return input;
        }
    }
}

// MSIG UI
void todo_end() {
    // 프로그램 종료
    std::cout << std::endl << std::endl << "  프로그램을 종료합니다." << std::endl;
}
void todo_0_0() {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // __rule__.txt, __config__.txt 제거
    std::cout << "  * 데이터셋의 모든 __rule__.txt와 __config__.txt를 제거합니다." << std::endl;
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())            &&
            fs::is_regular_file(p.path())   &&
            (p.path().filename().string()=="__rule__.txt" ||
             p.path().filename().string()=="__config__.txt"))
        {
            fs::remove(p.path());
        }
    }
    
    // __rule__.txt 생성
    std::cout << "  * 데이터셋의 각각의 폴더마다 __rule__.txt를 생성합니다." << std::endl;
    std::ofstream((datasetPath/fs::path("__rule__.txt")).string()).close();
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())        &&
            fs::is_directory(p.path()))
        {
            std::ofstream((p.path()/fs::path("__rule__.txt")).string()).close();
        }
    }
    
    // __rule__.txt 기본값 채우기
    MSIG::Structure::Folder datasetFolder(datasetPath);
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0_1() {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // config 데이터가 없는 이미지 config 생성
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())            &&
            fs::is_regular_file(p.path())   &&
            p.path().extension() ==".png")
        {
            MSIG::Algorithm::MusicalSymbol ms(p.path(), true);
        }
    }
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0_2() {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // config 데이터가 없는 이미지 config 생성
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())            &&
            fs::is_regular_file(p.path())   &&
            p.path().extension() ==".png")
        {
            MSIG::Algorithm::MusicalSymbol ms(p.path(), false);
            ms.edit_config();
        }
    }
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0()   {
    // 작업 선택
    switch (menu({
        "데이터셋 초기화",
        "새로운 이미지들 config 생성",
        "모든 이미지 config 수정"
    }, "작업을 선택하세요"))
    {
        case 0: todo_0_0(); break;
        case 1: todo_0_1(); break;
        case 2: todo_0_2(); break;
    }
}
void todo_1()   {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // 캔버스 생성
    MSIG::Rendering::Canvas canvas(datasetName, 30000, 3000, 3000, true);
    
    // 악상기호 이미지 생성
    std::cout << "  * 악상기호 데이터셋을 생성합니다." << std::endl;
    canvas.draw();
    
    // 완료
    std::cout << "  * 생성을 완료하였습니다." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void page()     {
    namespace fs = std::filesystem;
    
    // 프로그램 정보 출력
    signboard();
    
    // 현재 디렉토리에 존재하는 폴더들 조사
    std::vector<std::string> vs;
    for (auto& p : fs::directory_iterator(fs::path(".")))
    {
        if (fs::exists(p.path())        &&  // 존재하는지
            fs::is_directory(p.path())  &&  // 폴더인지
            p.path().filename().string().find("MSIG_")==std::string::npos)   // 해당 프로그램으로 생성된 데이터셋 폴더가 아닌지
        {
            vs.push_back(p.path().filename().string());
        }
    }
    if (vs.empty())
    {
        std::cout << "  ! 현재 디렉토리에 폴더가 존재하지 않습니다." << std::endl;
        std::cout << "  ! 프로그램을 종료합니다." << std::endl << std::endl;
        return;
    }
    
    // 폴더 이름 저장
    size_t selectedFolderIndex = menu(vs, "데이터셋 폴더를 선택하세요");
    datasetName = vs[selectedFolderIndex];
    
    // 작업 선택
    switch (menu({
        "데이터셋 관리",
        "악상기호 데이터셋 생성"
    }, "작업을 선택하세요"))
    {
        case 0: todo_0(); break;
        case 1: todo_1(); break;
    }
}

// 프로그램 실행
int main(int argc, char* argv[])
{
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
    system("cls");
#endif
    
    // OpenCV 로그 출력 설정 : 에러만 출력
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    
    // 메인 페이지 실행
    page();
    
    return 0;
}

