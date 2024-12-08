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
    
    // __config__.txt 제거
    std::cout << "  * 데이터셋의 모든 __config__.txt를 제거합니다." << std::endl;
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())            &&
            fs::is_regular_file(p.path())   &&
            p.path().filename().string()=="__config__.txt")
        {
            fs::remove(p.path());
        }
    }
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0_1() {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // __rule__.txt, __config__.txt 제거
    std::cout << "  * 데이터셋의 모든 __rule__.txt를 제거합니다." << std::endl;
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())            &&
            fs::is_regular_file(p.path())   &&
            p.path().filename().string()=="__rule__.txt")
        {
            fs::remove(p.path());
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
            MSIG::Algorithm::MusicalSymbol ms(p.path(), true);
        }
    }
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0_3() {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // __rule__.txt 생성
    std::cout << "  * 데이터셋의 각각의 폴더마다 __rule__.txt를 생성합니다." << std::endl;
    for (auto& p : fs::recursive_directory_iterator(datasetPath)) {
        if (fs::exists(p.path())        &&
            fs::is_directory(p.path()))
        {
            std::fstream((p.path()/fs::path("__rule__.txt")).string(), std::ios::out|std::ios::app).close();
        }
    }
    
    // __rule__.txt 기본값 채우기
    MSIG::Structure::Folder datasetFolder(datasetPath);
    
    // 완료 문구 출력
    std::cout << "  * 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_0_4() {
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
        "이미지 config 전부 제거",
        "이미지 rule   전부 제거",
        "이미지 config 생성",
        "이미지 rule   생성",
        "이미지 config 수정"
    }, "작업을 선택하세요"))
    {
        case 0: todo_0_0(); break;
        case 1: todo_0_1(); break;
        case 2: todo_0_2(); break;
        case 3: todo_0_3(); break;
        case 4: todo_0_4(); break;
    }
}
void todo_1()   {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // 캔버스 생성
    MSIG::Rendering::Canvas canvas(datasetName, true);
    
    // 악상기호 이미지 조합 계산
    canvas.calculate();
    
    // 사용자로부터 몇장을 생성할지 입력받기
    std::cout << std::endl << "  * 각 데이터셋마다 생성할 이미지 갯수를 지정하여주세요." << std::endl;
    size_t inputTrain = 0;
    size_t inputValidation = 0;
    size_t inputTest = 0;
    while (true)
    {
        // 출력
        std::cout << "  - train      : ";
        
        // 키 입력
        std::cin >> inputTrain;
        if (std::cin.fail()) {
            std::cin.clear();                                                   // 오류 플래그를 제거
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            continue;
        }
        
        // 1개 이상이어야 통과
        if (inputTrain > 0) {
            break;
        }
    }
    while (true)
    {
        // 출력
        std::cout << "  - validation : ";
        
        // 키 입력
        std::cin >> inputValidation;
        if (std::cin.fail()) {
            std::cin.clear();                                                   // 오류 플래그를 제거
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            continue;
        }
        
        // 1개 이상이어야 통과
        if (inputValidation > 0) {
            break;
        }
    }
    while (true)
    {
        // 출력
        std::cout << "  - test       : ";
        
        // 키 입력
        std::cin >> inputTest;
        if (std::cin.fail()) {
            std::cin.clear();                                                   // 오류 플래그를 제거
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            continue;
        }
        
        // 1개 이상이어야 통과
        if (inputTest > 0) {
            break;
        }
    }
    
    
    // 악상기호 이미지 생성
    std::cout << std::endl << "  * 악상기호 데이터셋을 생성합니다." << std::endl;
    canvas.draw(inputTrain, inputValidation, inputTest);
    
    // 완료
    std::cout << "  - 생성 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_2()   {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // 음표 머리 이미지 존재 확인
    std::cout << "  * 음표 머리 이미지들을 확인합니다." << std::endl;
    if (!fs::exists(datasetPath/fs::path("note-head-left-2.png")) ||
        !fs::exists(datasetPath/fs::path("note-head-right-2.png")) ||
        !fs::exists(datasetPath/fs::path("note-head-left-4.png")) ||
        !fs::exists(datasetPath/fs::path("note-head-right-4.png")))
    {
        std::cout << "  ! 해당 데이터셋에는 다음과 같은 음표 머리 이미지가 존재해야 합니다." << std::endl;
        std::cout << "  - note-head-left-2.png" << std::endl;
        std::cout << "  - note-head-right-2.png" << std::endl;
        std::cout << "  - note-head-left-4.png" << std::endl;
        std::cout << "  - note-head-right-4.png" << std::endl << std::endl;
        
        std::cout << "  ! 프로그램을 종료합니다." << std::endl << std::endl;
        return;
    }
    
    // 겹음 이미지들 생성
    std::cout << "  * 겹음 이미지들을 생성합니다." << std::endl;
    std::string input = "";
    while(true)
    {
        std::cout << "    생성할 데이터셋 이름을 입력하세요 : ";
        std::cin >> input;
        if (std::cin.fail()) {
            std::cin.clear();                                                   // 오류 플래그를 제거
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 입력 버퍼 비우기
            continue;
        }
        if (input != "") {
            std::cout << std::endl << std::endl;
            break;
        }
    }
    std::cout << "  - 2분음표 머리의 겹음 이미지들을 생성합니다." << std::endl;
    MSIG::API::make_multiple_notes((datasetPath/fs::path("note-head-left-2.png")).string(),
                                   (datasetPath/fs::path("note-head-right-2.png")).string(),
                                   input + "-2",
                                   input + "-2");
    std::cout << "  - 4분음표 머리의 겹음 이미지들을 생성합니다." << std::endl;
    MSIG::API::make_multiple_notes((datasetPath/fs::path("note-head-left-4.png")).string(),
                                   (datasetPath/fs::path("note-head-right-4.png")).string(),
                                   input + "-4",
                                   input + "-4");
    
    // 완료 문구 출력
    std::cout << "  - 생성 완료." << std::endl;
    
    // 프로그램 종료
    todo_end();
}
void todo_3()   {
    namespace fs = std::filesystem;
    
    // 데이터셋 경로 생성
    std::filesystem::path datasetPath(datasetName);
    
    // 폴더 생성
    MSIG::Structure::Folder root(datasetPath);
    
    // 폴더 구조 출력
    root.tree();
    
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
        "악상기호 데이터셋 생성",
        "겹음 이미지들 생성",
        "데이터셋 구조 확인"
    }, "작업을 선택하세요"))
    {
        case 0: todo_0(); break;
        case 1: todo_1(); break;
        case 2: todo_2(); break;
        case 3: todo_3(); break;
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

