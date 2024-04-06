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
    
    // Canvas.set() 확인
    //
    //canvas.set(path("new-symbol-dataset/complete/line-@/staff-b4.png"));
    //canvas.set(path("new-symbol-dataset/complete/line-@/note-down-@/note-4.png"));
    //canvas.set(path("new-symbol-dataset/complete/line-@/note-down-@/articulation-#/staccato.png"));
    //canvas.set(path("new-symbol-dataset/complete/line-@/note-down-@/articulation-#/accent.png"));
    //canvas.set(path("new-symbol-dataset/complete/line-@/note-down-@/articulation-#/tenuto.png"));
    //canvas.set(path("new-symbol-dataset/complete/line-@/note-down-@/articulation-#/fermata.png"));
    //canvas.show();
    
    // MusicalSymbol.operator&() 확인
    //
    //msig::MusicalSymbol ms1(path("new-symbol-dataset/complete/line-@/note-up-@/note-4.png"), dataset_config_dir);
    //msig::MusicalSymbol ms2(path("new-symbol-dataset/complete/line-@/note-up-@/articulation-#/fermata.png"), dataset_config_dir);
    //cout << endl << endl << "ms1 & ms2 : " << (ms1 & ms2) << endl;
    
    // *. 악상 기호 편집
    //edit_musical_symbol_image_config("이미지 경로", "config 파일 경로");
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
    canvas = msig::Canvas(dataset_dir);
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
