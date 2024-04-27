// c++17
#include <iostream>
#include <opencv2/opencv.hpp>

// 나의 라이브러리
#include <msig.hpp>

using namespace std;
using namespace std::filesystem;
using namespace cv;


// 데이터셋 주소
path dataset_dir = path("new-symbol-dataset");
path dataset_config_dir = dataset_dir / path("symbol_dataset_config.txt");
path dataset_create_dir;

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
    // 제외할 악상기호 리스트
    vector<path> list_except = {
        "new-symbol-dataset/complete/edge-@",
    };
    
    // 악상기호 제외(비활성화)하기
    selector.state(list_except, false);
    
    // 가능한 모든 조합의 경우의수 구하기
    vector<vector<path>> all_combination = selector.get_list();
    
    // 이미지 생성 후 저장
    for (auto& v : selector.get_list())
    {
        // 벡터 안에 있는 path 순차적으로 canvas에 select
        for (auto& p : v) canvas.select(p);
        
        // 보여주기 (나중에 사진 저장으로 대체하기)
        canvas.show();
        
        // canvas 선택 초기화
        canvas.select_celar();
    }
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
