#include <msig.hpp>

namespace msig
{


void Note::load_imgs(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    path d1(".");
    path d2(dataset_dir);
    
    for (int i=0; i<dirs.size(); i+=2){
        // 이미지 경로 생성
        path d3(dirs[i]), d4(dirs[i+1] + ".png");
        path d = d1 / d2 / d3 / d4;
        string img_dir = d.string();
        
        // key 이름 생성
        string key_name = dirs[i] + "_" + dirs[i+1];
        
        // 이미지 불러오기
        Mat img = imread(img_dir, IMREAD_GRAYSCALE);
        if (img.data){
            imgs[key_name] = img.clone();
            img.release();
        }
    }
}

void Note::load_imgs_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 변수들
    path d = path(dataset_dir) / path("symbol_dataset_config.txt");
    string config_dir = d.string();
    string line, line_key, line_value;
    int n = -1;
    
    // imgs_config 없으면 파일 생성
    fstream make_file(config_dir, ios::out|ios::app);
    make_file.close();
    
    // imgs_config 불러오기
    fstream fin(config_dir, ios::in);
    if (!fin){
        cout << "msig: ";
        cout << "can't open " << config_dir << " for reading.";
        cout << endl;
        return;
    }
    while (getline(fin, line)) {
        // 소문자로 변환
        for (int i=0; i<line.length(); i++)
            line[i] = tolower(line[i]);

        // 정규표현식 생성
        string reg_dir = "([a-zA-Z0-9_-]+)";
        string reg_u = "([-+]?[0-9]+)";
        string reg_d = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
        regex reg("^"+reg_dir+"="+reg_u+"_"+reg_u+"_"+reg_d+"_"+reg_d+"$");
        
        // 정규표현식 검사
        if (!regex_match(line, reg)) continue;
        
        // "="를 기준으로 단어 나누기
        line_key = line.substr(0, line.find("="));
        line_value = line.substr(line.find("=")+1);
        
        // imgs_key에 존재하는 key인지 검사
        if (imgs.find(line_key)==imgs.end()) continue;
        
        // imgs_config에 저장
        imgs_config[line_key] = line_value;
    }
    fin.close();
    
    // 존재하지 않는 imgs_config 조사
    vector<string> not_in_imgs_config;
    for (auto key : imgs){
        if (imgs_config.find(key.first)==imgs_config.end())
            not_in_imgs_config.push_back(key.first);
    }
    
    // imgs_config 내용 추가하기
    fstream fout(config_dir, ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << config_dir << " for writing.";
        cout << endl;
        return;
    }
    for (auto key : not_in_imgs_config){
        // key에 해당하는 config 값 생성
        line_value = make_config(key);
        
        // line_value 내용 확인 후 저장
        if (line_value=="") break;
        else imgs_config[key] = line_value;
        
        // imgs_config, symbol_dataset_config.txt에 내용 추가
        imgs_config[key] = line_value;
        fout << key + "=" + line_value << endl;
    }
    fout.close();
}

std::string Note::make_config(std::string symbol_name){
    using namespace std;
    using namespace cv;
    
    // 이미지
    Mat img(height, width, CV_8UC1, Scalar(255));   // 흰 배경
    Mat img_key = imgs[symbol_name].clone();        // 악상 기호 이미지
    
    // 기본 config 정보
    int x=width/2.0;
    int y=height/2.0;
    double degree=0.0;
    double scale=1.0;
    
    // 이미지 버퍼
    // ..기존의 config 정보와 달라진게 있을 경우에 이미지를 그리기 위한 mat저장 버퍼
    
    // 창 생성
    namedWindow(symbol_name, WINDOW_AUTOSIZE);
    
    // 키보드 이벤트
    while (true) {
        // 폅집할 이미지들
        Mat img1 = img.clone();
        Mat img2 = img_key.clone();
        
        // 이미지 중심 점
        int cx = img2.cols/2.0;
        int cy = img2.rows/2.0;
        
        // 이미지 편집
        img2 = rotate_mat(img2, degree, cx, cy);    // 이미지 회전
        img2 = scale_mat(img2, scale);              // 이미지 확대 및 축소
        
        // 이미지 크기 변동에 의한 중심점 업데이트
        cx = img2.cols/2.0;
        cy = img2.rows/2.0;
        
        // config 생성 보조 그림 : 오선지
        for (auto h : vector<int>({-50, -25, 0, 25, 50})){
            int n = img.rows/2.0;
            line(img1, Point(0,n+h), Point(img.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
        // config 생성 보조 그림 : 2분음표
        // ...
        
        // 이미지 편집 2
        img1 = combine_mat(img1, x-cx, y-cy, img2); // 이미지 합치기
        
        // 십자선 그리기
        Point cross(width/2.0, height/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
        
        // 이미지 화면에 그리기
        imshow(symbol_name, img1);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);
        
        if (key=='\n'|| key=='\r') break;     // 완료에 의한 종료
        if (key==27){                       // 중단에 의한 종료
            destroyWindow(symbol_name);
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
            case 'x':                       // 초기화
                x=width/2.0; y=height/2.0;
                degree=0.0;
                scale=1.0;
                break;
            // 빠르게
            case 'A': x-=3;         break;  // 좌로 이동
            case 'D': x+=3;         break;  // 우로 이동
            case 'W': y-=3;         break;  // 상으로 이동
            case 'S': y+=3;         break;  // 하로 이동
            case 'Q': degree+=3.0;  break;  // 반시계 회전
            case 'E': degree-=3.0;  break;  // 시계 회전
            case 'Z': scale-=0.2;   break;  // 축소
            case 'C': scale+=0.2;   break;  // 확대
            case 'X':                       // 초기화
                x=width/2.0; y=height/2.0;
                degree=0.0;
                scale=1.0;
                break;
        }
        if (degree<0.0) degree += 360.0;
        if (degree>360.0) degree -= 360.0;
        if (scale<0.1) scale = 0.1;
    }
    destroyWindow(symbol_name);
    
    // symbol_name에 대한 config 정보 반환
    return to_string(x-width/2)+"_"+to_string(y-height/2)+"_"+to_string(degree)+"_"+to_string(scale);
}

}
