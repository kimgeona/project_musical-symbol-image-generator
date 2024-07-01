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



