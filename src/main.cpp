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

// 의존적 선택 알고리즘
msig::DSTree selector;

// 악상기호 조합 클래스
msig::Canvas canvas;

// 함수 원형들
void prepare_platform(void);
void prepare_dataset(void);
void prepare_DSTree(void);
void prepare_Canvas(void);
void edit_musical_symbol_image_config(string image_dir, string image_config_dir);


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
    
    
    /*
    // Canvas 테스트 코드
    canvas.select(path("new-symbol-dataset/complete/line-@/staff-a4-0-0.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/note-4.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/articulation-#/staccato.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/articulation-#/accent.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/articulation-#/tenuto.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/articulation-#/fermata.png"));
    canvas.select(path("new-symbol-dataset/complete/line-@/note-up-@/accidental-#/flat.png"));
    canvas.show();
    */
    
    /*
    // 데이터셋 악상 기호 전부 편집
    for (auto& p : std::filesystem::recursive_directory_iterator(std::filesystem::path("new-symbol-dataset")))
    {
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension() == ".png")
        {
            msig::MusicalSymbol ms(p, dataset_config_dir);
            ms.edit_config();
        }
    }
    */
    
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


// *. 악상 기호 편집
void edit_musical_symbol_image_config(string image_dir, string image_config_dir)
{
    path dir(image_dir);
    path config_dir(image_config_dir);
    
    msig::MusicalSymbol ms(dir, config_dir);
    
    if (ms.status) return;
    else ms.edit_config();
}
