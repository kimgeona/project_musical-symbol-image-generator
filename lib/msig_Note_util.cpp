#include <msig.hpp>

namespace msig
{


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
    
    // 이미지 존재 여부 확인
    if (!exists(path(dir))) return;
    
    // 이미지 읽기
    Mat img = imread(dir, IMREAD_GRAYSCALE);
    CV_Assert(img.data);
    
    // img의 여백을 제외한 부분을 참조하여 새로 이미지를 생성하고 이를 저장하면 됨.
    // 행렬 헤더와 .clone() 개념을 이용할것.
    
    // 이미지 저장
    //imwrite(dir, new_img);
}

// symbols 무결성 유지
void Note::integrity_symbols(){
    using namespace std;
    using namespace cv;
    
    // img_symbols에는 있는데 img_configs에는 없는것 -> config 생성
    for (auto e : img_symbols){
        string symbol_name = e.first;
        string symbol_config;
        
        // img_configs에서 존재 확인
        if (img_configs.find(symbol_name)!=img_configs.end()) continue;
        
        // config 생성
        make_config(symbol_name, symbol_config);
        
        // symbol_config 확인
        if (symbol_config=="") break;
        
        // config 생성
        img_configs[symbol_name] = symbol_config;
    }
    
    // img_symbols에는 있는데 img_configs에는 없는것 -> symbols 삭제
    vector<string> remove_list;
    for (auto e : img_symbols){
        string symbol_name = e.first;
        
        // img_configs에서 존재 확인
        if (img_configs.find(symbol_name)!=img_configs.end()) continue;
        
        // symbol 삭제 목록에 추가
        remove_list.push_back(symbol_name);
    }
    for (auto e : remove_list){
        img_symbols.erase(e);
    }
}

// configs 무결성 유지
void Note::integrity_configs(){
    using namespace std;
    using namespace cv;
    
    // img_configs에는 있는데 img_symbols에는 없는것 -> symbol 생성
    // ..(악상 기호 이미지 생성은 불가능함)
    
    // img_configs에는 있는데 img_symbols에는 없는것 -> config 삭제
    vector<string> remove_list;
    for (auto e : img_configs){
        string symbol_name = e.first;
        
        // img_symbols에서 존재 확인
        if (img_symbols.find(symbol_name)!=img_symbols.end()) continue;
        
        // config 삭제 목록에 추가
        remove_list.push_back(symbol_name);
    }
    for (auto e : remove_list){
        img_configs.erase(e);
    }
}

// symbol_dataset_config 저장
void Note::save_symbol_dataset_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // symbol_dataset_config.txt 경로
    path config_dir = path(ds_dir)/path("symbol_dataset_config.txt");
    
    // 기존 symbol_dataset_config.txt 삭제
    if (exists(config_dir)) remove(config_dir);
        
    // 새로운 symbol_dataset_config.txt 생성
    fstream fout(config_dir.string(), ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << config_dir << " for writing.";
        cout << endl;
        return;
    }
    
    // config 데이터 저장
    for (auto e : img_configs){
        string symbol_name = e.first;
        string symbol_config = e.second;
        fout << endl << symbol_name + "=" + symbol_config;
    }
    fout.close();
}

// 문자열 앞,뒤 공백 제거
void Note::string_trim(std::string& str){
    // 문자열이 비어있을경우 그냥 종료
    if (str.empty()) return;
    
    // 왼쪽 공백 조사
    int left = 0;
    while (left < str.length() && std::isspace(str[left])) left++;
    
    // 오른쪽 공백 조사
    int right = (int)str.length();
    while (left < right && std::isspace(str[right-1])) right--;
    
    // 공백이 제거된 문자열 범위 추출
    str = str.substr(left, right - left);
}

// 문자열 소문자로 변환
void Note::string_to_lower(std::string& str){
    using namespace std;
    using namespace cv;
    
    // 소문자로 변환
    for (int i=0; i<str.length(); i++)
        str[i] = tolower(str[i]);
}

// 문자열 분리
std::vector<std::string> Note::split(std::string s, std::string c){
    using namespace std;
    using namespace cv;
    
    vector<string> s_out;
    size_t p = 0;
    size_t n = s.find("_");
    while (n != string::npos) {
        s_out.push_back(s.substr(p, n - p));
        p = n + 1;
        n = s.find("_", p);
    }
    s_out.push_back(s.substr(p, s.length() - p));
    return s_out;
}

// config 값 생성
void Note::make_config(std::string symbol_name, std::string& symbol_config){
    using namespace std;
    using namespace cv;
    
    // 기본 config 정보
    int x=img_w/2.0;
    int y=img_h/2.0;
    double degree=0.0;
    double scale=1.0;
    
    // 이미지 버퍼
    // ..기존의 config 정보와 달라진게 있을 경우에 이미지를 그리기 위한 mat저장 버퍼
    
    // 창 생성
    namedWindow(symbol_name, WINDOW_AUTOSIZE);
    
    // 키보드 이벤트
    while (true) {
        // 폅집할 이미지들
        Mat img(img_h, img_w, CV_8UC1, Scalar(255));
        Mat img_symbol = img_symbols[symbol_name].clone();
        string img_config = to_string(x-img_w/2)+"_"+to_string(y-img_h/2)+"_"+to_string(degree)+"_"+to_string(scale)+"_false";
        
        // 이미지 편집
        img = draw_symbols(img, img_symbol, img_config, true);
        
        // 이미지 화면에 그리기
        imshow(symbol_name, img);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);
        if (key=='\n'|| key=='\r') break;   // 완료에 의한 종료
        if (key==27){                       // 중단에 의한 종료
            destroyWindow(symbol_name);
            symbol_config = "";
            return;
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
                x=img_w/2.0;
                y=img_h/2.0;
                degree=0.0;
                scale=1.0;
                break;
        }
        
        // 범위 값 처리
        if (degree<0.0) degree += 360.0;
        if (degree>360.0) degree -= 360.0;
        if (scale<0.1) scale = 0.1;
    }
    destroyWindow(symbol_name);
    
    // config 값 저장
    symbol_config = to_string(x-img_w/2)+"_"+to_string(y-img_h/2)+"_"+to_string(degree)+"_"+to_string(scale)+"_false";
}


}
