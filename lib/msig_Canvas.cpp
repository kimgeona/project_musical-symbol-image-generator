#include <msig_Canvas.hpp>

namespace msig
{


Canvas::Canvas()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    this->width             = 0;
    this->height            = 0;
    this->dir_ds            = path();
    this->dir_ds_config     = path();
    this->dir_ds_complete   = path();
    this->dir_ds_piece      = path();
    this->ds_complete       = map<path, MusicalSymbol>();
    this->ds_piece          = map<path, MusicalSymbol>();
    this->draw_list         = vector<MusicalSymbol>();
}

Canvas::Canvas(std::filesystem::path dataset_dir, int width, int height)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 아래 과정에서 이상이 있을 경우
    if (set_size(width, height) ||
        set_dataset_dirs(dataset_dir))
    {
        std::runtime_error("msig::Canvas::Canvas() : 캔버스를 생성하는 과정에서 문제가 있습니다.");
    }
}

Canvas::Canvas(const Canvas& other)
{
    this->width             = other.width;
    this->height            = other.height;
    this->dir_ds            = other.dir_ds;
    this->dir_ds_config     = other.dir_ds_config;
    this->dir_ds_complete   = other.dir_ds_complete;
    this->dir_ds_piece      = other.dir_ds_piece;
    this->ds_complete       = other.ds_complete;
    this->ds_piece          = other.ds_piece;
    this->draw_list         = other.draw_list;
}

Canvas& Canvas::operator=(const Canvas& other)
{
    this->width             = other.width;
    this->height            = other.height;
    this->dir_ds            = other.dir_ds;
    this->dir_ds_config     = other.dir_ds_config;
    this->dir_ds_complete   = other.dir_ds_complete;
    this->dir_ds_piece      = other.dir_ds_piece;
    this->ds_complete       = other.ds_complete;
    this->ds_piece          = other.ds_piece;
    this->draw_list         = other.draw_list;
    return *this;
}

bool    Canvas::operator==(const Canvas& other) const 
{
    if (this->width             == other.width              &&
        this->height            == other.height             &&
        this->dir_ds            == other.dir_ds             &&
        this->dir_ds_config     == other.dir_ds_config      &&
        this->dir_ds_complete   == other.dir_ds_complete    &&
        this->dir_ds_piece      == other.dir_ds_piece       &&
        this->ds_complete       == other.ds_complete        &&
        this->ds_piece          == other.ds_piece           &&
        this->draw_list         == other.draw_list) return true;
    else                                            return false;
}


int     Canvas::set_size(int width, int height)
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

int     Canvas::set_dataset_dirs(std::filesystem::path dir)
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

int     Canvas::load_dataset_complete()
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

int     Canvas::load_dataset_piece()
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

int     Canvas::make_piece_to_complete()
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


cv::Mat Canvas::draw()
{
    using namespace std;
    using namespace cv;
    
    // 흰 이미지 생성
    Mat img = Mat(height, width, CV_8UC1, Scalar(255));
    
    // 악상기호 배치 알고리즘(MSPA)
    MSPA mspa;
    
    // 그리기 목록(draw_list)에 있는 악상 기호들 배치
    for(auto& ms : draw_list)
    {
        // 악상기호 패딩 제거
        //remove_padding(ms.img, ms.x, ms.y);
        
        // 악상기호 종류에 따라 적절한 위치에 배치
        mspa.add(ms);
    }
    
    // 배치가 완료된 악상기호 이미지에 그리기
    img = draw_symbols(img, mspa.get());
    
    // 생성된 이미지 리턴
    return img;
}

cv::Mat Canvas::draw_symbols(const cv::Mat& img, const MusicalSymbol& ms, bool auxiliary_line)
{
    using namespace std;
    using namespace cv;
    
    // 배경 이미지 준비
    Mat img1 = img.clone();
    
    // 악상 기호 이미지 준비
    Mat img2 = ms.img.clone();
    int cx = ms.x;
    int cy = ms.y;
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    img2 = mat_rotate(img2, ms.rotate, cx, cy);     // 이미지 회전
    img2 = mat_scale(img2, ms.scale, cx, cy);       // 이미지 확대
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true)
    {
        for (auto h : vector<int>({-ms.pad*2, -ms.pad, 0, ms.pad, ms.pad*2}))
        {
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
    }
    
    // 이미지 합성 좌표 계산
    int x = (img.cols)/2.0 - cx;   // 이미지 중심 - sub 이미지 중심 + config 값
    int y = (img.rows)/2.0 - cy;
    
    // 이미지 합성
    img1 = mat_attach(img1, img2, x, y);
    
    // 보조선 그리기 (십자선)
    if (auxiliary_line==true)
    {
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
    }
    
    return img1;
}


int     Canvas::select(std::filesystem::path dir)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 path를 ds_complete에서 찾기
    if (this->ds_complete.find(dir)==ds_complete.end())
    {
        // 못찾은 경우
        cout << "Canvas::select() : 그리려고 하는 이미지 " << dir << "가 데이터셋에 없습니다." << endl;
        return -1;
    }
    else
    {
        // 찾았을 경우
        this->draw_list.push_back(this->ds_complete[dir]);  // draw_list에 해당 이미지 추가
        return 0;
    }
}

void    Canvas::select_celar()
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 선택한 음표 내역 비우기
    this->draw_list = vector<MusicalSymbol>();
}

void    Canvas::show()
{
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    string title = "Canvas";
    
    // 윈도우 생성
    namedWindow(title, WINDOW_AUTOSIZE);
    moveWindow(title, 100, 100);
    
    // 윈도우에 이미지 그리기
    imshow(title, image);
    
    // 키보드 입력시 종료
    waitKey();
    
    // 윈도우 없애기
    destroyWindow(title);
}

void    Canvas::save(std::string file_name)
{
    using namespace std;
    using namespace cv;
    
    // 데이터 준비
    Mat image = draw();
    
    // 이미지 저장
    imwrite(file_name, image);
}


}
