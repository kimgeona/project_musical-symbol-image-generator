#include <msig_Canvas.hpp>

namespace msig
{


// 캔버스 크기 설정
int Canvas::set_size(int width, int height)
{
    // 크기가 양수인지 확인
    if (width > 0 && height > 0)
    {
        this->width = width;
        this->height = height;
    }
    else return -1;
    
    return 0;
}


// 주소 초기화
int Canvas::set_dataset_dirs(std::filesystem::path dir)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 입력받은 데이터셋 주소 확인
    if (exists(dir) && is_directory(dir))   this->dir_ds = dir;     // 존재하는지, 디렉토리인지
    else                                    return -1;
    
    // 주소 생성
    this->dir_ds_config   = dir/path("symbol_dataset_config.txt");  // config 파일 주소
    this->dir_ds_complete = dir/path("complete");                   // complete 데이터셋 주소
    this->dir_ds_piece    = dir/path("piece");                      // piece 데이터셋 주소
    
    // config 파일이 존재하지 않으면 새로 생성
    if (!exists(this->dir_ds_config))
    {
        fstream(dir_ds_config.string(), ios::out|ios::app).close();  // 파일 생성 후 닫기
    }
    
    // 주소 확인
    if (!exists(dir_ds_config)          ||
        !exists(dir_ds_complete)        ||
        !exists(dir_ds_piece)           ||
        !is_regular_file(dir_ds_config) ||
        !is_directory(dir_ds_complete)  ||
        !is_directory(dir_ds_piece))
    {
        return -1;
    }
    
    // 데이터셋 불러오기
    if (load_dataset_complete())    return -1;
    if (load_dataset_piece())       return -1;
    
    return 0;
}


// 완성형 데이터셋 불러오기
int Canvas::load_dataset_complete()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 초기화
    this->ds_complete = map<path, MusicalSymbol>();
    
    // 변수
    vector<path> list_png;
    
    // 불러올 이미지 목록 구하기
    for (auto& p : recursive_directory_iterator(this->dir_ds_complete))
    {
        if (exists(p.path())                        &&  // 존재해야 함
            is_regular_file(p.path())               &&  // 파일 이어야 함
            p.path().extension().string()==".png")      // 확장자가 .png 이어야 함
        {
            list_png.push_back(p.path()); // list_png에 추가
        }
    }
    
    // 이미지 불러오기
    for (auto& p : list_png)
    {
        // 악상 기호 생성
        MusicalSymbol ms(p, this->dir_ds_config);
        
        // 악상 기호 상태 체크
        if (ms.status)
        {
            cout << "악상 기호 : " << p << "가 로드되지 않음." << endl;
            continue;
        }
        
        // 악상 기호 추가
        this->ds_complete[p] = ms;
    }
    return 0;
}


// 조합형 데이터셋 불러오기
int Canvas::load_dataset_piece()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 초기화
    this->ds_piece = map<path, MusicalSymbol>();
    
    // 변수
    vector<path> list_png;
    
    // 불러올 이미지 목록 구하기
    for (auto& p : recursive_directory_iterator(this->dir_ds_piece))
    {
        if (exists(p.path())                        &&  // 존재해야 함
            is_regular_file(p.path())               &&  // 파일 이어야 함
            p.path().extension().string()==".png")      // 확장자가 .png 이어야 함
        {
            list_png.push_back(p.path()); // list_png에 추가
        }
    }
    
    // 악상기호 불러오기
    for (auto& p : list_png)
    {
        // 악상 기호 생성
        MusicalSymbol ms(p, this->dir_ds_config);
        
        // 악상 기호 상태 체크
        if (ms.status)
        {
            cout << "악상 기호 : " << p << "가 로드되지 않음." << endl;
            continue;
        }
        
        // 악상 기호 추가
        this->ds_piece[p] = ms;
    }
    
    // 조합형 악상기호로 완성형 악상기호 만들기
    return make_piece_to_complete();
}


// 조합형 악상기호로 완성형 악상기호 만들기
int Canvas::make_piece_to_complete()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    /*
     this->ds_complete는 map<path, MusicalSymbol> 컨테이너 클래스임.
     this->ds_piece는    map<path, MusicalSymbol> 컨테이너 클래스임.
     
     여기서 path는 파일시스템의 주소체계를 나타내는 클래스이고, MusicalSymbol는 악상기호를 관리하는 클래스임.
     
     make_piece_to_complete()의 목표는 ds_piece 안에 있는 악상기호 이미지들을 조합하여 완성형 악상기호를 만들어내고,
     ds_complete 안에다 해당 완성형 악상기호를 저장하는 것임.
     */
    
    //for (auto& pair : this->ds_piece) cout << "key : " << pair.first << endl;
    
    // 1. ds_piece안에 있는 조합형 악상기호(MusicalSymbol)들을 조합하여 완성형 악상기호(MusicalSymbol)로 만들기.
    // 2. 완성형 악상기호를 ds_complete에 저장하기.
    
    // 조합형 악상기호 만들기에 성공하면 0, 실패하면 1 리턴.
    return 0;
}


}
