// c++17
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

// 나의 라이브러리
#include <msig.hpp>

// 프로그램 버전
#define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))

// 데이터셋 주소
std::filesystem::path dataset_dir = std::filesystem::path("new-symbol-dataset");
std::filesystem::path dataset_config_dir = dataset_dir / std::filesystem::path("symbol_dataset_config.txt");
std::filesystem::path dataset_create_dir = std::filesystem::path(std::string("MusicalSymbol-v.") + MSIG_VERSION);

// 멀티 스레딩
unsigned int                number_of_thread;   // 사용 가능 스레드 갯수
std::vector<std::thread>    threads;            // 스레드 벡터
std::mutex                  mtx;                // 뮤텍스
std::vector<msig::Canvas>   canvas;             // 캔버스 벡터



// 악상기호 이미지 이름 생성
std::string naming(const std::vector<std::filesystem::path>& v)
{
    using namespace std;
    
    // 변수
    string name;
    
    // 파일 이름 연결
    for (auto&p : v)
        name = name + msig::my_split(p.filename().string(), ".")[0] + "_";
    
    // 마지막 문자 "_" 제거
    if (!name.empty())
        name.erase(name.end()-1);
    
    // 이미지 확장자 추가
    name = name + ".png";
    
    return name;
}

// 스레드 프로세싱 함수
void processing(int canvas_number, std::queue<std::vector<std::filesystem::path>>& data)
{
    using namespace std;
    using namespace std::filesystem;
    
    while (true)
    {
        vector<path> vp;
        
        // 공유 자원 얻기
        {
            lock_guard<mutex> lock(mtx);
            // 남은 작업량 확인
            if (data.empty()) break;
            
            // 작업할 내용 추출
            vp = data.front();
            data.pop();
        }
        
        
        // ---- 이미지 생성 작업 시작 ----
        
        // Canvas : 조합 선택
        for (auto& p : vp) canvas[canvas_number].select(p);
        
        // 폴더 존재 확인
        if (!exists(dataset_create_dir)) create_directory(dataset_create_dir);
        
        // 생성할 이미지 이름 생성
        path image_name = dataset_create_dir / path(naming(vp));
        
        // 이미 존재하는 이미지는 건너뛰기
        if (exists(image_name))
        {
            // 건너뛰기
            printf("pass : %s\n", image_name.string().c_str());
        }
        else
        {
            // 이미지 저장
            canvas[canvas_number].save(image_name.string());
            printf("save : %s\n", image_name.string().c_str());
        }
        
        // 선택 초기화
        canvas[canvas_number].select_celar();
    }
}

int main(int argc, char* argv[])
{
    using namespace std;
    using namespace std::filesystem;
    using namespace cv;
    
    // 플랫폼 설정
#ifdef __MACH__
#endif
#ifdef _WIN32
    system("chcp 65001");
#endif


    // 프로그램 정보 출력
    std::cout << R"(
MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM
M"""""`'"""`YM MP""""""`MM M""M MM'"""""`MM
M  mm.  mm.  M M  mmmmm..M M  M M' .mmm. `M
M  MMM  MMM  M M.      `YM M  M M  MMMMMMMM
M  MMM  MMM  M MMMMMMM.  M M  M M  MMM   `M
M  MMM  MMM  M M. .MMM'  M M  M M. `MMM' .M
M  MMM  MMM  M Mb.     .dM M  M MM.     .MM
MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM
)" << endl;
    std::cout << "  Musical Symbol Image Generator(" << MSIG_VERSION << ")" << std::endl << std::endl;
    std::cout << "  - Version: " << MSIG_VERSION << std::endl;
    

    // 플랫폼 출력
#ifdef __MACH__
    std::cout << "  - Platform: MacOS" << std::endl << std::endl << std::endl;
#endif
#ifdef _WIN32
    std::cout << "  - Platform: Windows" << std::endl << std::endl << std::endl;
#endif

    
    // 기본 데이터셋 이미지들 확인
    std::cout << "  기본 데이터셋의 이미지들을 확인합니다." << std::endl;
    if (!std::filesystem::exists(dataset_dir) || !std::filesystem::is_directory(dataset_dir))
    {
        std::cout << "  ! 기본 데이터셋 폴더 " << dataset_dir << "가 존재하지 않습니다." << std::endl;
        return -1;
    }
    if (!std::filesystem::exists(dataset_config_dir) || !std::filesystem::is_regular_file(dataset_config_dir))
    {
        std::cout << "  ! 기본 데이터셋 폴더의 이미지들 정보 파일 " << dataset_config_dir.filename() << "가 존재하지 않습니다." << std::endl;
        return -1;
    }
    for (auto& p : recursive_directory_iterator(dataset_dir))
    {
        // 이미지(png) 파일 확인
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png")
        {
            // 악상 기호 생성
            msig::MusicalSymbol ms(p, dataset_config_dir);
            
            // 악상 기호 상태 체크
            if (ms.status)  cout << "fail    : " << p << endl;
            //else            cout << "success : " << p << endl;
        }
    }
    std::cout << "  - 악상 기호 데이터셋 생성 위치: " << dataset_create_dir << std::endl << std::endl;
    

    // 이미지들을 생성하기 위한 준비
    std::cout << "  이미지 생성을 위한 준비를 합니다." << std::endl;
    msig::DST dst((dataset_dir / path("complete")).string());
    std::cout << "  - DST 준비 완료." << std::endl;
    number_of_thread = std::thread::hardware_concurrency();                                     // 사용 가능한 코어 수 구하기
    if (number_of_thread)   std::cout << "  - Thread: " << number_of_thread << std::endl;    // 사용 가능한 코어 수 확인
    else                    std::cout << "  - Thread: " << ++number_of_thread << std::endl;
    for (unsigned int i=0; i<number_of_thread; i++)                                             // 사용 가능한 코어 수 만큼 캔버스 생성
    {
        // 캔버스 생성
        canvas.emplace_back(dataset_dir, 192, 512); // (데이터셋 주소, 이미지 크기)
        
        // 캔버스 생성 확인
        if (canvas.back()==msig::Canvas())
        {
            std::cout << "  ! 악상기호를 그릴 캔버스가 생성이 되지 않습니다." << std::endl;
            return -1;
        }
    }
    std::cout << std::endl;
    
    
    // 악상기호를 그리기 위한 설정
    std::cout << "  그릴 악상 기호에 대한 설정을 합니다." << std::endl;
    std::vector<double> rate;
    while (true)
    {
        string input;
        std::cout << "  - 각 깊이별로 생성할 이미지의 비율 입력(종료는 exit): ";
        std::getline(std::cin, input);
        
        // 종료 조건 확인
        if (input == "exit") return 0;
        
        // 비어있는지 확인
        if (input == "") continue;
        
        // 입력받은 문자열 전처리
        input = msig::my_trim(input);           // 좌우 공백 제거
        auto vs = msig::my_split(input, " ");   // " "을 기준으로 문자열 분리
        
        // rate 벡터에 입력받은 값들 추가
        try
        {
            for (auto& s : vs)
            {
                double tmp = std::stod(s);
                if (tmp < 0.0 || tmp > 1.0) throw std::runtime_error("  ! 입력되는 값들은 0~1 사이의 값이어야 합니다.");
                rate.push_back(tmp);
            }
        }
        catch (const std::exception& e)
        {
            rate.clear();   // 입력 받은 rate 초기화
            continue;       // 다시 입력 받기
        }
        
        break;
    }
    std::cout << "  - 생성되는 이미지 개수를 계산합니다. ";
    queue<vector<path>> all_combination = dst.list(rate);
    std::cout << "총 " << all_combination.size() << "개." << std::endl;
    while (true)
    {
        char c;
        cout << "  - 진행 하시겠습니까? (y/n): ";
        cin >> c;
        cin.ignore(100, '\n');
        if      (c == 'y')  break;
        else if (c == 'n')  return 0;
        else                continue;
    }
    std::cout << std::endl << std::endl;
    
    
    // 악상 기호 데이터셋 생성 시작
    std::cout << "  악상 기호 이미지 데이터셋을 생성합니다. " << std::endl;
    for (int i=5; i>=0; i--)
    {
        cout << i << " ";
        std::this_thread::sleep_for(chrono::seconds(1));
    }
    cout << endl;
    for (unsigned int i=0; i<number_of_thread; i++) threads.emplace_back(processing, i, std::ref(all_combination));
    for (unsigned int i=0; i<number_of_thread; i++) threads[i].join();
    std::cout << "  - 생성 완료." << std::endl << std::endl;
    
    // *. 악상기호 config 데이터 조정
    //edit_musical_symbol_image_config(dataset_dir.string());
    
    // 프로그램 종료
    std::cout << std::endl << "  - 프로그램을 종료합니다." << std::endl;
    
    return 0;
}

// *. 악상 기호 편집
void edit_musical_symbol_image_config(std::string image_dir, std::string image_config_dir)
{
    std::filesystem::path dir(image_dir);
    std::filesystem::path config_dir(image_config_dir);
    
    msig::MusicalSymbol ms(dir, config_dir);
    
    if (ms.status) return;
    else ms.edit_config();
}
void edit_musical_symbol_image_config(std::string ds_dir)
{
    std::filesystem::path ds(ds_dir);
    std::filesystem::path ds_config = ds / std::filesystem::path("symbol_dataset_config.txt");
    
    for (auto& p : std::filesystem::recursive_directory_iterator(ds))
    {
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension() == ".png")
        {
            msig::MusicalSymbol ms(p, ds_config);
            if (ms.status) continue;
            else ms.edit_config();
        }
    }
}



