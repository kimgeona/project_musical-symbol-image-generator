// c++17
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

// 나의 라이브러리
#include <msig.hpp>

using namespace std;
using namespace std::filesystem;
using namespace cv;


// 프로그램 버전
#define MSIG_VERSION (std::to_string(MSIG_VERSION_MAJOR) + "."+ std::to_string(MSIG_VERSION_MINOR) + "." + std::to_string(MSIG_VERSION_PATCH))


// 데이터셋 주소
path dataset_dir = path("new-symbol-dataset");
path dataset_config_dir = dataset_dir / path("symbol_dataset_config.txt");
path dataset_create_dir = path(string("MusicalSymbol-v.") + MSIG_VERSION);


// 의존적 선택 알고리즘
msig::DSTree selector;

// 악상기호 조합 클래스
msig::Canvas canvas;


// 함수 원형들
void prepare_platform(void);
void prepare_dataset(void);
void prepare_DSTree(void);
void prepare_Canvas(void);
void start_program(void);
void edit_musical_symbol_image_config(string image_dir, string image_config_dir);
void edit_musical_symbol_image_config(string ds_dir);
std::string naming(const std::vector<std::filesystem::path>& v);


// 프로그램
int main(void)
{
    // 0. 플랫폼 준비
    prepare_platform();
    
    // 1. 데이터셋 준비
    prepare_dataset();
    
    // 2. 의존적 선택 알고리즘 준비
    prepare_DSTree();
    
    // 3. 악상기호 조합 준비
    prepare_Canvas();
    
    // 4. 악상기호 생성 프로그램 실행
    start_program();
    
    // *. 악상기호 config 데이터 조정
    //edit_musical_symbol_image_config(dataset_dir.string());
    
    return 0;
}


// 0. 플랫폼 준비
void prepare_platform(void)
{
#ifdef __MACH__
    cout << "Platform : MacOS" << endl << endl;
#endif
#ifdef _WIN32
    cout << "Platform : Windows" << endl << endl;
    system("chcp 65001");
#endif
}


// 1. 데이터셋 준비
void prepare_dataset(void)
{
    cout << endl << "1. 데이터셋 준비" << endl;
    for (auto& p : recursive_directory_iterator(dataset_dir)){
        // (존재하는지, 파일인지, .png 확장자인지) 확인
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png"){
            // 악상 기호 생성
            msig::MusicalSymbol ms(p, dataset_config_dir);
            // 악상 기호 상태 체크
            if (ms.status)  cout << "fail    : " << p << endl;
            else            cout << "success : " << p << endl;
        }
    }
    cout << "----완료." << endl;
}


// 2. 의존적 선택 알고리즘 준비
void prepare_DSTree(void)
{
    cout << endl << "2. 의존적 선택 알고리즘 준비" << endl;
    selector = msig::DSTree((dataset_dir/path("complete")).string(), {".png"});
    if (selector==msig::DSTree()){
        cout << "DSTree가 생성이 되지 않았습니다." << endl;
        exit(-1);
    }
    cout << "----완료." << endl;
}


// 3. 악상기호 조합 준비
void prepare_Canvas(void)
{
    cout << endl << "3. 악상기호 조합 준비" << endl;
    canvas = msig::Canvas(dataset_dir, 256, 512);
    if (canvas==msig::Canvas()){
        cout << "Canvas가 생성이 되지 않았습니다." << endl;
        exit(-1);
    }
    cout << "----완료." << endl;
}


// 4. 악상기호 생성 프로그램 실행
void start_program(void)
{
    cout << endl << "4. 악상기호 이미지를 생성합니다." << endl;
    
    // 제외할 악상기호 리스트
    vector<path> list_except = {
        path("new-symbol-dataset") / path("complete") / path("edge-left-@"),
        path("new-symbol-dataset") / path("complete") / path("edge-right-@"),
        path("new-symbol-dataset") / path("complete") / path("line-fixed-@"),
        path("new-symbol-dataset") / path("complete") / path("line-@") / path("note-up-@") / path("dynamic-#"),
        path("new-symbol-dataset") / path("complete") / path("line-@") / path("note-down-@") / path("dynamic-#"),
    };
    
    // 악상기호 제외(비활성화)하기
    selector.state(list_except, false);
    
    // 가능한 모든 조합의 경우의수 구하기
    vector<vector<path>> all_combination = selector.combination_list();
    
    // 이미지 생성 후 저장
    for (auto& v : all_combination)
    {
        // 벡터 안에 있는 path 순차적으로 canvas에 select
        for (auto& p : v) canvas.select(p);
        
        // 폴더 존재하는지 확인
        if (!exists(dataset_create_dir)) create_directory(dataset_create_dir);
        
        // 이미지 이름 생성
        path image_name = dataset_create_dir / path(naming(v));
        
        // 이미 존재하는 이미지는 건너뛰기
        if (exists(image_name))
        {
            // 건너뛰기
            cout << "pass : " << image_name << endl;
        }
        else
        {
            // 이미지 저장
            canvas.save(image_name.string());
            cout << "save : " << image_name << endl;
        }
        
        // canvas 선택 초기화
        canvas.select_celar();
    }
    
    cout << "----완료." << endl;
}


// *. 악상 기호 편집
void edit_musical_symbol_image_config(string image_dir, string image_config_dir)
{
    path dir(image_dir);
    path config_dir(image_config_dir);
    
    msig::MusicalSymbol ms(dir, config_dir);
    
    if (ms.status) return;
    else ms.edit_config();
}
void edit_musical_symbol_image_config(string ds_dir)
{
    path ds(ds_dir);
    path ds_config = ds / path("symbol_dataset_config.txt");
    
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


// *. 악상기호 이미지 이름 생성
std::string naming(const std::vector<std::filesystem::path>& v)
{
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
