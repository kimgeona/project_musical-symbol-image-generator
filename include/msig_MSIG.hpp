#pragma once
#ifndef msig_MSIG_hpp
#define msig_MSIG_hpp

// c++17
#include <iostream>     // 입출력 스트림
#include <fstream>      // 파일 입출력
#include <filesystem>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

// 나의 라이브러리
#include <msig_Algorithm_DST.hpp>
#include <msig_Algorithm_MSPA.hpp>
#include <msig_Canvas.hpp>
#include <msig_Matrix.hpp>
#include <msig_MusicalSymbol.hpp>
#include <msig_Util.hpp>

// 프로그램 버전
#define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))

namespace msig
{

class MSIG
{
private:
    // 데이터셋 주소
    std::filesystem::path dataset_dir;
    std::filesystem::path dataset_config_dir;
    std::filesystem::path dataset_create_dir;
    
    // 멀티 스레딩
    std::mutex      mtx_data;   // 데이터 접근 뮤텍스
    std::mutex      mtx_io;     // 입출력 접근 뮤텍스
    unsigned int    number_of_thread;       // 사용 가능 스레드 개수
    std::vector<std::thread>    threads;    // 스레드
    std::vector<msig::Canvas>   canvases;   // 캔버스
    
    // 알고리즘
    msig::DST dst;
    std::queue<std::vector<std::filesystem::path>> all_combination;    // 악상 기호 조합 저장 변수
    
public:
    // 생성자
    MSIG(std::filesystem::path default_dataset_dir = std::filesystem::path("symbol-dataset"), std::filesystem::path new_dataset_dir = std::filesystem::path(std::string("MusicalSymbol-v.") + MSIG_VERSION))
    {
        // 데이터셋 주소 저장
        dataset_dir = default_dataset_dir;
        dataset_config_dir = dataset_dir / std::filesystem::path("symbol_dataset_config.txt");
        dataset_create_dir = new_dataset_dir;
        
        set_plaform();
        print_info();
        print_platform();
    }
    
    // 플랫폼 설정
    void set_plaform()
    {
        // 플랫폼 설정
#ifdef __MACH__
#endif
#ifdef _WIN32
        system("chcp 65001");
#endif
    }
    
    // MSIG 시작 문구 출력
    void print_info()
    {
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
)" << std::endl;
        std::cout << "  Musical Symbol Image Generator(" << MSIG_VERSION << ")" << std::endl << std::endl;
        std::cout << "  - Version: " << MSIG_VERSION << std::endl;
    }
    
    // 플랫폼 출력
    void print_platform()
    {
        // 플랫폼 출력
#ifdef __MACH__
        std::cout << "  - Platform: MacOS" << std::endl << std::endl << std::endl;
#endif
#ifdef _WIN32
        std::cout << "  - Platform: Windows" << std::endl << std::endl << std::endl;
#endif
    }
    
    // 데이터셋 준비
    int  prepare_default_dataset()
    {
        using namespace std::filesystem;
        
        // 기본 데이터셋 이미지들 확인
        std::cout << "  기본 데이터셋의 이미지들을 확인합니다." << std::endl;
        
        // 기본 데이터셋 폴더 확인
        if (!exists(dataset_dir) || !is_directory(dataset_dir))
        {
            std::cout << "  ! 기본 데이터셋 폴더 " << dataset_dir << "가 존재하지 않습니다." << std::endl;
            return -1;
        }
        
        // 기본 데이터셋 폴더의 config 파일 확인
        if (!exists(dataset_config_dir) || !is_regular_file(dataset_config_dir))
        {
            std::cout << "  ! 기본 데이터셋 폴더의 config 파일 " << dataset_config_dir.filename() << "이 존재하지 않아 새로 생성합니다." << std::endl;
            std::fstream(dataset_config_dir.string(), std::ios::out|std::ios::app).close();
        }
        
        // 악상 기호 제대로 생성되는지 확인
        for (auto& p : recursive_directory_iterator(dataset_dir))
        {
            // dataset_dir에서 확인되는 이미지 파일들만
            if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png")
            {
                try
                {
                    // 악상 기호 생성
                    msig::MusicalSymbol ms(p, dataset_config_dir);
                }
                catch (const std::runtime_error& e)
                {
                    if (e.what() == std::string("msig::MusicalSymbol::MusicalSymbol() : 악상 기호 객체를 생성하는 과정에서 문제가 있습니다."))
                    {
                        std::cout << "  ! 기본 데이터셋의 악상 기호 이미지 확인 과정에서 문제가 있습니다." << std::endl;
                        return -1;
                    }
                    else throw;
                }
            }
        }
        
        std::cout << std::endl;
        return 0;
    }
    
    // DST 알고리즘 준비
    int  prepare_DST()
    {
        using namespace std::filesystem;
        
        // DST 알고리즘 준비
        std::cout << "  DST 알고리즘을 준비합니다." << std::endl;
        
        // 기본 데이터셋 폴더 확인
        if (!exists(dataset_dir) || !is_directory(dataset_dir))
        {
            std::cout << "  ! 기본 데이터셋 폴더 " << dataset_dir << "가 존재하지 않습니다." << std::endl;
            return -1;
        }
        
        // DST 생성
        try
        {
            dst = msig::DST((dataset_dir / path("complete")).string());
        }
        catch (const std::runtime_error& e)
        {
            if (e.what() == std::string("msig::DSTFolder::DSTFolder() : 비어있는 경로이거나 존재하지 않는 폴더로 DSTFolder 객체를 생성할 수 없습니다."))
            {
                std::cout << "  ! 기본 데이터셋 폴더 " << dataset_dir << "를 이용하여 DST를 생성할 수 없습니다." << std::endl;
                return -1;
            }
            else throw;
        }
        
        std::cout << std::endl;
        return 0;
    }
    
    // Canvas 준비
    int  prepare_Canvas()
    {
        using namespace std::filesystem;
        
        // Canvas 준비
        std::cout << "  악상 기호를 그릴 Canvas를 준비합니다." << std::endl;
        
        // 사용 가능한 코어 갯수 구하기
        number_of_thread = std::thread::hardware_concurrency(); // 사용 가능한 코어 수 구하기
        if (number_of_thread < 1) number_of_thread = 1;
        
        // 사용 가능한 코어 개수 만큼 Canvas 생성
        for (unsigned int i=0; i<number_of_thread; i++) // 사용 가능한 코어 수 만큼 캔버스 생성
        {
            // 기본 데이터셋 폴더 확인
            if (!exists(dataset_dir) || !is_directory(dataset_dir))
            {
                std::cout << "  ! 기본 데이터셋 폴더 " << dataset_dir << "가 존재하지 않습니다." << std::endl;
                return -1;
            }
            
            // 캔버스 생성
            try
            {
                canvases.emplace_back(dataset_dir, 192, 512); // (데이터셋 주소, 이미지 크기)
            }
            catch (const std::runtime_error& e)
            {
                if (e.what() == std::string("msig::Canvas::Canvas() : 캔버스를 생성하는 과정에서 문제가 있습니다."))
                {
                    std::cout << "  ! 캔버스를 생성하는 과정에서 문제가 있습니다." << std::endl;
                }
                else throw;
            }
        }
        
        std::cout << std::endl;
        return 0;
    }
    
    // 사용자 설정
    int  preference_setting()
    {
        // 악상기호를 그리기 위한 설정
        std::cout << "  사용자 설정을 진행합니다." << std::endl;
        
        // 각 악상 기호 조합 깊이별로 생성할 비율 입력받기
        std::vector<double> rate;
        while (true)
        {
            std::string input;
            std::cout << "  > 각 깊이별로 생성 비율 입력(종료는 exit): ";
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
        
        // 생성되는 이미지 개수 계산
        std::cout << "  - 생성되는 이미지 개수를 계산합니다. ";
        all_combination = dst.list(rate);
        
        // 진행 여부 확인
        while (true)
        {
            char c;
            std::cout << "  > 진행 하시겠습니까? (y/n): ";
            std::cin >> c;
            std::cin.ignore(100, '\n');
            if      (c == 'y')  break;
            else if (c == 'n')  return 0;
            else                continue;
        }
        
        std::cout << std::endl;
        return 1;
    }
    
    // 데이터셋 생성
    void making_dataset()
    {
        // 악상 기호 데이터셋 생성 시작
        std::cout << std::endl << "  악상 기호 이미지 데이터셋을 생성합니다. ";
        for (int i=5; i>=0; i--)
        {
            std::cout << i << " ";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << std::endl;
        for (unsigned int i=0; i<number_of_thread; i++) threads.emplace_back(&MSIG::thread_processing, this, i, all_combination.size());
        for (unsigned int i=0; i<number_of_thread; i++) threads[i].join();
        std::cout << "  - 생성 완료." << std::endl << std::endl;
    }
    
    // 스레드 함수
    void thread_processing(int canvas_number, const long data_size)
    {
        using namespace std;
        using namespace std::filesystem;
        
        while (true)
        {
            vector<path> vp;
            
            // 공유 자원 얻기
            {
                lock_guard<mutex> lock(mtx_data);
                
                // 남은 작업량 확인
                if (all_combination.empty()) break;
                
                // 작업할 내용 추출
                vp = all_combination.front();
                all_combination.pop();
            }
            
            
            // ---- 이미지 생성 작업 시작 ----
            
            // Canvas : 조합 선택
            for (auto& p : vp) canvases[canvas_number].select(p);
            
            // 폴더 존재 확인
            if (!exists(dataset_create_dir)) create_directory(dataset_create_dir);
            
            // 생성할 이미지 이름 생성
            path image_name = dataset_create_dir / path(naming(vp));
            
            // 공유 자원 얻기
            int pre=0;
            {
                lock_guard<mutex> lock(mtx_data);
                
                // 진행정도 계산
                pre = (int)(((double)(data_size - all_combination.size()) / (double)data_size) * 100.0);
            }
            
            // 이미 존재하는 이미지는 건너뛰기
            if (exists(image_name))
            {
                // 건너뛰기
                printf("(%d%%) pass : %s\n", pre, image_name.string().c_str());
            }
            else
            {
                // 이미지 저장
                canvases[canvas_number].save(image_name.string());
                printf("(%d%%) save : %s\n", pre, image_name.string().c_str());
            }
            
            // 선택 초기화
            canvases[canvas_number].select_celar();
        }
    }
    
    // 이미지 이름 생성 함수
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
};

}

#endif /* msig_MSIG_hpp */
