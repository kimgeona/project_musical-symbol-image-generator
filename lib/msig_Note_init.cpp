#include <msig.hpp>

namespace msig
{


// 데이터셋 불러오기
void Note::load_ds(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 1. dir_ds/complete에 있는 모든 .png 파일 경로 조사
    path p_complete = path(dir_ds) / path("complete");
    for (const auto& p : recursive_directory_iterator(p_complete)){
        if (is_regular_file(p.path()) && p.path().extension() == ".png")
            ds_list.push_back(p.path().string());
    }
    
    // 2. 절대경로를 기반으로 완성형(독립형, 종속성), 조합형 확인하여 MusicalSymbol 생성, ds에 저장
    for (const auto& dir : ds_list){
        restore_image(dir);     // 이미지 재생성(이상한 형식의 이미지 제대로 저장)
        //remove_padding(dir);    // 이미지 여백 제거
        
        Mat img = imread(dir);                      // 이미지 불러오기
        CV_Assert(img.data);
        vector<string> configs = get_config(dir);   // config 불러오기
        
        MusicalSymbol ms;
        ms.dir              = dir;                      // 저장 위치
        ms.dir_dependent    = get_dependent(dir);       // 의존성 위치
        ms.name             = get_name(dir);            // 악상 기호 이름
        ms.img              = img.clone();              // 악상 기호 이미지
        
        if (configs.empty()){
            string new_config = make_config(ms.dir);
            if (new_config=="") continue;
            configs = my_split(new_config, "_");
        }
        ms.x                = stoi(configs[0]);
        ms.y                = stoi(configs[1]);
        ms.scale            = stoi(configs[2]);
        ms.rotate           = stoi(configs[3]);
        
        ds.push_back(ms);   // ds에 저장
        save_config();      // config 업데이트
    }
}
void Note::load_ds_piece(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 1. dir_ds/complete에 있는 모든 .png 파일 경로 조사
    path p_piece = path(dir_ds) / path("piece");
    for (const auto& p : recursive_directory_iterator(p_piece)){
        if (is_regular_file(p.path()) && p.path().extension() == ".png")
            ds_piece_list.push_back(p.path().string());
    }
    
    // 2. MusicalSymbol 생성, ds_piece에 저장
    for (const auto& dir : ds_piece_list){
        restore_image(dir);     // 이미지 재생성(이상한 형식의 이미지 제대로 저장)
        //remove_padding(dir);    // 이미지 여백 제거
        
        Mat img = imread(dir);                      // 이미지 불러오기
        CV_Assert(img.data);
        vector<string> configs = get_config(dir);   // config 불러오기
        
        MusicalSymbol ms;
        ms.dir              = dir;                      // 저장 위치
        ms.dir_dependent    = vector<string>();         // 의존성 위치
        ms.name             = get_name(dir);            // 악상 기호 이름
        ms.img              = img.clone();              // 악상 기호 이미지
        
        if (configs.empty()){
            string new_config = make_config(ms.dir);
            if (new_config=="") continue;
            configs = my_split(new_config, "_");
        }
        ms.x                = stoi(configs[0]);
        ms.y                = stoi(configs[1]);
        ms.scale            = stoi(configs[2]);
        ms.rotate           = stoi(configs[3]);
        
        ds_piece.push_back(ms);     // ds_piece에 저장
        save_config();              // config 업데이트
    }
}


}
