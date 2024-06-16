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

// 멀티 스레딩
unsigned int                number_of_thread;   // 사용 가능 스레드 갯수
std::vector<std::thread>    threads;            // 스레드 벡터
std::mutex                  mtx;                // 뮤텍스
std::vector<msig::Canvas>   canvas;             // 캔버스 벡터

// 데이터셋 주소
std::filesystem::path dataset_dir = std::filesystem::path("new-symbol-dataset");
std::filesystem::path dataset_config_dir = dataset_dir / std::filesystem::path("symbol_dataset_config.txt");
std::filesystem::path dataset_create_dir = std::filesystem::path(std::string("MusicalSymbol-v.") + MSIG_VERSION);

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
    
    // 0. 플랫폼 준비
    {
    #ifdef __MACH__
        cout << "Platform : MacOS" << endl << endl;
    #endif
    #ifdef _WIN32
        cout << "Platform : Windows" << endl << endl;
        system("chcp 65001");
    #endif
    }
    
    // 1. 기본 데이터셋 이미지들 확인
    {
        cout << endl << "1. 기본 데이터셋 이미지들 확인" << endl;
        
        // 주소 출력
        cout << "----기본 데이터셋 주소 : " << dataset_dir << endl;
        cout << "----기본 데이터셋 config 파일 : " << dataset_config_dir << endl;
        cout << "----악상기호 데이터셋 생성 주소 : " << dataset_create_dir << endl;
        
        // dataset_dir 에서 악상기호 이미지 리스트 구하기
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
        cout << "----완료." << endl;
    }
    
    // 2. 의존적 선택 알고리즘 준비
    cout << endl << "2. 의존적 선택 알고리즘 준비" << endl;
    msig::DST dst(dataset_dir / path("complete"));  // 의존적 선택 트리 생성
    cout << "----완료." << endl;
    
    // 3. 악상기호 조합 준비
    {
        cout << endl << "3. 악상기호 조합 준비" << endl;
        
        // 사용가능한 코어 수 구하기
        number_of_thread = std::thread::hardware_concurrency();
        
        // 사용가능한 코어 수 확인
        if (number_of_thread)   cout << "----thread : " << number_of_thread << endl;
        else                    cout << "----thread : " << ++number_of_thread << endl;
        
        // 사용가능한 코어 수 만큼 캔버스 생성
        for (unsigned int i=0; i<number_of_thread; i++)
        {
            // 캔버스 생성
            canvas.emplace_back(dataset_dir, 192, 512); // (데이터셋 주소, 이미지 크기)
            
            // 캔버스 생성 확인
            if (canvas.back()==msig::Canvas())
            {
                cout << "Canvas가 생성이 되지 않았습니다." << endl;
                return -1;
            }
        }
        cout << "----완료." << endl;
    }
    
    // 4. 악상기호 생성 프로그램 실행
    {
        cout << endl << "4. 악상기호 이미지 생성" << endl;
        
        /*
        // 특정 악상기호 그리기에서 제외
        selector.set_activation({
            path("new-symbol-dataset") / path("complete") / path("edge-left-@"),
            path("new-symbol-dataset") / path("complete") / path("edge-right-@"),
            path("new-symbol-dataset") / path("complete") / path("line-fixed-@"),
        }, false);
        */
        
        //
        int depth = 0;
        while (true)
        {
            cout << "----악상기호 중첩 수 : ";
            cin >> depth;
            cin.ignore(100, '\n');
            
            if (depth > 0) break;
            
            cout << "----중첩 수는 0보다 커야 합니다." << endl;
        }
        
        //
        double rate = 0.0;
        while (true)
        {
            cout << "----중첩 수 이상 조합 건너뛸 비율 : ";
            cin >> rate;
            cin.ignore(100, '\n');
            
            if (rate > 0.0) break;
            
            cout << "----비율 값은 0.0보다 커야 합니다." << endl;
        }
        
        // 악상기호 조합 구하기
        cout << "----가능한 악상기호 조합들을 계산합니다. ";
        queue<vector<path>> all_combination = dst.list(depth, rate);
        cout << "총 " << all_combination.size() << "개" << endl;
        
        // 진행 확인
        while (true)
        {
            char c;
            cout << "----진행 하시겠습니까? (y/n) : ";
            cin >> c;
            cin.ignore(100, '\n');
            if      (c == 'y')  break;
            else if (c == 'n')  return 0;
            else                continue;
        }

        // thread : 이미지 생성 스레드 처리
        cout << "----악상기호 이미지 생성을 시작합니다. ";
        for (int i=5; i>=0; i--) {
            cout << i << " ";
            std::this_thread::sleep_for(chrono::seconds(1));
        }
        cout << endl;
        
        for (unsigned int i=0; i<number_of_thread; i++) threads.emplace_back(processing, i, std::ref(all_combination));
        for (unsigned int i=0; i<number_of_thread; i++) threads[i].join();
        
        cout << "----완료." << endl;
    }
    
    // *. 악상기호 config 데이터 조정
    //edit_musical_symbol_image_config(dataset_dir.string());
    
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



