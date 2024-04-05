// c++17
#include <iostream>
#include <opencv2/opencv.hpp>

// 나의 라이브러리
#include <msig.hpp>

int main(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
#ifdef __MACH__
    cout << "Platform : MacOS" << endl << endl;
#endif
#ifdef _WIN32
    cout << "Platform : Windows" << endl << endl;
    system("chcp 65001");
#endif
    
    // 데이터셋 주소
    path dataset_dir = path("new-symbol-dataset");
    path dataset_config_dir = dataset_dir / path("symbol_dataset_config.txt");
    
    
    // 데이터셋 준비
    for (auto& p : recursive_directory_iterator(dataset_dir))
    {
        // 존재하는 파일이고, .png 형식이라면 생성
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png")
        {
            // 악상 기호 생성
            msig::MusicalSymbol ms(p, dataset_config_dir);
            
            // 악상 기호 상태 체크
            if (ms.status){
                cout << "fail    : " << p << endl;
                continue;
            }
            else
                cout << "success : " << p << endl;
        }
    }
    
    // 악상기호 조합하는 코드 작성
    
    return 0;
}
