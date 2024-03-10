#include <msig.hpp>

namespace msig
{


// 의존성 위치 알아내기
std::vector<std::string>    Note::get_dependent(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    vector<string>  list_folder;
    path            p = path(dir).parent_path();
    
    // @ 디렉토리인 경우 : 전부
    if (p.filename().string().find("@")!=-1){
        // dir 내에 있는 파일들
        for (auto& f : directory_iterator(p)){
            if (is_regular_file(f.path()) && f.path().extension()==".png")
                list_folder.push_back(f.path().string());
        }
        // 같은 레벨에 있는 파일들
        for (auto& d : directory_iterator(p.parent_path())){
            if (d.path().string() == p.string()) continue;
            if (is_directory(d.path())) list_folder.push_back(d.path().string());
        }
    }
    
    // # 디렉토리인 경우 : 자기자신만
    else if (p.filename().string().find("#")!=-1){
        // dir 내에 있는 파일들 + 하위 디렉토리들도
        list_folder.push_back(p.string());
    }
    
    return list_folder;
}
// 악상 기호 이름 알아내기
std::string                 Note::get_name(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    path file_dir(dir);
    return my_split(file_dir.filename(), ".")[0];
}
// config 정보 가져오기
std::vector<std::string>    Note::get_config(std::string line){
    using namespace std;
    using namespace cv;
    
    line = my_trim(line);
    line = my_lower(line);
    line = transform_to_write(line);
    
    vector<string> configs;
    
    string config_string = my_grep(dir_ds_config, line);
    
    if (config_string!=""){
        config_string = config_string.substr(line.length()+1, config_string.length());
        string re_i = "([-+]?[0-9]+)";
        string re_d = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
        regex config_reg("^"+re_i+"_"+re_i+"_"+re_d+"_"+re_d+"$");
        if (regex_match(config_string, config_reg))
            configs = my_split(config_string, "_");
    }
    
    return configs;
}
// 선택 가능한 악상기호 구하기
std::vector<MusicalSymbol>  Note::get_selectable(const std::vector<MusicalSymbol>& ls_ms_1,
                                                 const std::vector<MusicalSymbol>& ls_ms_2) {
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    return std::vector<MusicalSymbol>();
}



// 가지치기
void Note::do_pruning(std::vector<MusicalSymbol>& ls_ms_1, MusicalSymbol& ms) {
    
}
// 가지치기
void Note::do_pruning(std::vector<std::string>& ls1, std::string dir) {
    using namespace std;
    using namespace cv;

    // 벡터를 순회하면서 해당 문자열을 포함하는 요소를 제거
    ls1.erase(remove_if(ls1.begin(), ls1.end(), [&](const std::string& d) { return d.find(dir) != std::string::npos; }), ls1.end());
}



// 이미지 재생성
void Note::restore_image(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 존재 여부 확인
    if (!exists(path(dir))) return;
    
    // 이미지 읽기
    Mat img = imread(dir, IMREAD_GRAYSCALE);
    CV_Assert(img.data);
    
    // 이미지 저장
    imwrite(dir, img);
}
// 이미지 패딩 제거
void Note::remove_padding(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 존재 여부 확인
    if (!exists(path(dir))) return;
    
    // 이미지 읽기
    Mat img = imread(dir, IMREAD_GRAYSCALE);
    CV_Assert(img.data);

    // 흰색 픽셀을 0으로, 다른 색상의 픽셀을 1로 설정
    Mat non_white_img = img < 255;

    // 행과 열의 범위를 찾기 위한 변수
    int row_start = -1, row_end = -1, col_start = -1, col_end = -1;

    // 행의 범위를 찾기
    for (int i = 0; i < non_white_img.rows; i++) {
        if (row_start < 0 && countNonZero(non_white_img.row(i)) > 0) {
            row_start = i;
        }
        if (row_start >= 0 && countNonZero(non_white_img.row(i)) > 0) {
            row_end = i;
        }
    }

    // 열의 범위를 찾기
    for (int i = 0; i < non_white_img.cols; i++) {
        if (col_start < 0 && countNonZero(non_white_img.col(i)) > 0) {
            col_start = i;
        }
        if (col_start >= 0 && countNonZero(non_white_img.col(i)) > 0) {
            col_end = i;
        }
    }


    // 범위를 제한하여 이미지를 잘라냄
    Rect roi(col_start, row_start, col_end - col_start, row_end - row_start);
    Mat new_img = img(roi).clone();

    // 이미지 저장
    imwrite(dir, new_img);
}



// 문자열 분리
std::vector<std::string>    Note::my_split(std::string s1, std::string s2){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return std::vector<std::string>();
    
    // s2를 기준으로 s1 분리
    vector<string> s_out;
    size_t p = 0;
    size_t n = s1.find(s2);
    while (n != string::npos) {
        s_out.push_back(s1.substr(p, n - p));
        p = n + 1;
        n = s1.find(s2, p);
    }
    s_out.push_back(s1.substr(p, s1.length() - p));
    return s_out;
}
// 문자열 앞,뒤 공백 제거
std::string                 Note::my_trim(std::string s1){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return "";
    
    // 왼쪽 공백 조사
    int left = 0;
    while (left < s1.length() && std::isspace(s1[left])) left++;
    
    // 오른쪽 공백 조사
    int right = (int)s1.length();
    while (left < right && std::isspace(s1[right-1])) right--;
    
    // 공백이 제거된 문자열 범위 추출
    return s1.substr(left, right - left);
}
// 문자열 소문자로 변환
std::string                 Note::my_lower(std::string s1){
    using namespace std;
    using namespace cv;
    
    // 빈 문자열인 경우
    if (s1.empty()) return "";
    
    // 소문자로 변환
    string s1_copy = s1;
    for (int i=0; i<s1_copy.length(); i++)
        if ('A' <= s1_copy[i] && s1_copy[i] <= 'Z')
            s1_copy[i] += 32;
    return s1_copy;
}



// grep 명령어 구현
std::string Note::my_grep(std::string dir, std::string s){
    using namespace std;
    using namespace cv;
    
    fstream fin(dir, ios::in);
    if (!fin){
        cout << "msig: ";
        cout << "Note(): ";
        cout << "my_grep(): ";
        cout << "can't open " << dir << " for reading.";
        cout << endl;
        exit(1);
    }
    string line;
    while (getline(fin, line)) {
        if (line.find(s)!=-1){
            fin.close();
            return line;
        }
    }
    fin.close();
    return "";
}


// 쓰기 변환
std::string Note::transform_to_write(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    string out = "";
    for (const auto& e : dir){
        out += "_" + e.string();
    }
    out = out.substr(1, out.length());
    return out;
}
// 쓰기 변환
std::string Note::transform_to_write(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    return transform_to_write(path(dir));
}
// 읽기 변환
std::string Note::transform_to_read(std::string dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    path out("");
    for (auto& p : my_split(dir, "_")){
        out = out / path(p);
    }
    return out.string();
}


// config값 생성
std::string Note::make_config(std::string dir){
    using namespace std;
    using namespace cv;
    
    // 이미지 생성
    Mat     img(img_h, img_w, CV_8UC1, Scalar(255));
    Mat     img_symbol  = imread(dir, IMREAD_GRAYSCALE);
    string  img_name    = get_name(dir);
    
    // config 정보 생성
    int     x       = 0;
    int     y       = 0;
    double  degree  = 0.0;
    double  scale   = 1.0;

    // 창 생성
    namedWindow(img_name, WINDOW_AUTOSIZE);
    
    // 키보드 이벤트
    while (true) {
        Mat img1 = img.clone();
        Mat img2 = img_symbol.clone();
        string config = to_string(x)+"_"+to_string(y)+"_"+to_string(degree)+"_"+to_string(scale);
        
        // 이미지 편집
        img1 = draw_symbols(img1, img2, config, true);
        
        // 이미지 화면에 그리기
        imshow(img_name, img1);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);         // 키보드 레이턴시
        if (key=='\n'|| key=='\r') break;   // 완료에 의한 종료
        if (key==27){                       // 중단에 의한 종료
            destroyWindow(img_name);
            return "";
        }
        switch (key) {
            // 기본 속도
            case 'a': x-=1;         break;  // 좌로 이동
            case 'd': x+=1;         break;  // 우로 이동
            case 'w': y-=1;         break;  // 상으로 이동
            case 's': y+=1;         break;  // 하로 이동
            case 'q': degree+=1.0;  break;  // 반시계 회전
            case 'e': degree-=1.0;  break;  // 시계 회전
            case 'z': scale-=0.1;   break;  // 축소
            case 'c': scale+=0.1;   break;  // 확대
            // 빠르게
            case 'A': x-=3;         break;  // 좌로 이동
            case 'D': x+=3;         break;  // 우로 이동
            case 'W': y-=3;         break;  // 상으로 이동
            case 'S': y+=3;         break;  // 하로 이동
            case 'Q': degree+=3.0;  break;  // 반시계 회전
            case 'E': degree-=3.0;  break;  // 시계 회전
            case 'Z': scale-=0.2;   break;  // 축소
            case 'C': scale+=0.2;   break;  // 확대
            // 초기화
            case 'x':                       // 초기화
            case 'X':                       // 초기화
                x=0;
                y=0;
                degree=0.0;
                scale=1.0;
                break;
        }
        
        // 범위 값 처리
        if (degree<0.0) degree += 360.0;
        if (degree>360.0) degree -= 360.0;
        if (scale<0.1) scale = 0.1;
    }
    destroyWindow(img_name);
    
    // config 값 리턴
    return to_string(x)+"_"+to_string(y)+"_"+to_string(degree)+"_"+to_string(scale);
}


// config 값 저장(업데이트)
void Note::save_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 기존 symbol_dataset_config.txt 삭제
    if (exists(dir_ds_config)) remove(dir_ds_config);
        
    // 새로운 symbol_dataset_config.txt 생성
    fstream fout(dir_ds_config, ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << dir_ds_config << " for writing.";
        cout << endl;
        return;
    }
    
    // config 데이터 저장
    for (auto& ms : ds){
        string dir = transform_to_write(ms.dir);
        string config = to_string(ms.x)+"_"+to_string(ms.y)+"_"+to_string(ms.rotate)+"_"+to_string(ms.scale);
        fout << endl << dir + "=" + config;
    }
    for (auto& ms : ds_piece){
        string dir = ms.dir;
        string config = to_string(ms.x)+"_"+to_string(ms.y)+"_"+to_string(ms.rotate)+"_"+to_string(ms.scale);
        fout << endl << dir + "=" + config;
    }
    fout.close();
}


}
