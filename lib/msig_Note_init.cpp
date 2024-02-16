#include <msig.hpp>

namespace msig
{


void Note::load_imgs(){
    using namespace std;
    using namespace cv;
    using namespace std::__fs::filesystem;
    
    path d1(".");
    path d2(dataset_dir);
    
    for (int i=0; i<dirs.size(); i+=2){
        // 이미지 경로 생성
        path d3(dirs[i]), d4(dirs[i+1] + ".png");
        path img_dir = d1 / d2 / d3 / d4;
        
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
    using namespace std::__fs::filesystem;
    
    // 변수들
    path config_dir = path(dataset_dir) / path("symbol_dataset_config.txt");
    string line, line_key, line_value;
    int n = -1;
    
    // imgs_config 불러오기
    fstream fin(config_dir, ios::in|ios::out|ios::app);
    if (!fin){
        // 에러 메시지 출력
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
        string reg_dir = "([a-zA-Z0-9_.]+)";
        string reg_u = "([+]?[0-9]+)";
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
        // imgs_config에 저장
        imgs_config[key] = line_value;
        // imgs_config에 내용 추가
        fout << key + "=" + line_value << endl;
    }
    fout.close();
}

std::string Note::make_config(std::string symbol_name){
    using namespace std;
    using namespace cv;
    
    // 이미지
    Mat img(512, 256, CV_8UC1, Scalar(255));    // 흰 배경
    Mat img_key = imgs[symbol_name].clone();    // 악상 기호 이미지
    
    // 기본 config 정보
    int x=128;
    int y=256;
    double degree=0.0;
    double scale=1.0;
    
    // 창 생성
    namedWindow(symbol_name, WINDOW_AUTOSIZE);
    
    // 키보드 이벤트
    while (true) {
        // 중앙 표시 변수
        int cx = (int)(img_key.cols/2.0)*scale;
        int cy = (int)(img_key.rows/2.0)*scale;
        
        // 폅집할 이미지들
        Mat img1 = img.clone();
        Mat img2 = img_key.clone();
        
        // 이미지 편집
        img2 = rotate_mat(img2, degree);            // 이미지 회전
        img2 = scale_mat(img2, scale);              // 이미지 확대 및 축소
        img1 = combine_mat(img1, x-cx, y-cy, img2); // 이미지 합치기
        
        // 십자선 그리기
        line(img1, Point(128-10,256), Point(128+10,256), Scalar(100), 1, LINE_AA);
        line(img1, Point(128,256-10), Point(128,256+10), Scalar(100), 1, LINE_AA);
        
        // 이미지 화면에 그리기
        imshow(symbol_name, img1);
        
        // 키보드 이벤트
        int key = waitKey(1000/30);
        if (key==27) break; // 종료
        switch (key) {
            case 'a': x-=1;         break;  // 좌로 이동
            case 'd': x+=1;         break;  // 우로 이동
            case 'w': y-=1;         break;  // 상으로 이동
            case 's': y+=1;         break;  // 하로 이동
            case 'q': degree+=1.0;  break;  // 반시계 회전
            case 'e': degree-=1.0;  break;  // 시계 회전
            case 'z': scale-=0.1;   break;  // 축소
            case 'c': scale+=0.1;   break;  // 확대
        }
        if (degree<0.0) degree += 360.0;
        if (scale<0.1) scale = 0.1;
    }
    destroyWindow(symbol_name);
    
    // symbol_name에 대한 config 정보 반환
    return to_string(x-128)+"_"+to_string(y-256)+"_"+to_string(degree)+"_"+to_string(scale);
}

cv::Mat Note::combine_mat(const cv::Mat& img, int x, int y, const cv::Mat& img_sub){
    using namespace std;
    using namespace cv;
    
    // img_result 생성
    Mat img_result = img.clone();
    
    // img, img_sub 시작 좌표
    int img_x = 0;
    int img_y = 0;
    int img_sub_x = img_x + x;
    int img_sub_y = img_y + y;
    
    // 겹치는 영역의 좌상단 점 계산
    int x1 = max(img_x, img_sub_x);
    int y1 = max(img_y, img_sub_y);
    
    // 겹치는 영역의 우하단 점 계산
    int x2 = min(img_x+img.cols, img_sub_x+img_sub.cols);
    int y2 = min(img_y+img.rows, img_sub_y+img_sub.rows);
    
    // 겹치는 영역 있는지 확인
    if (x1 < x2 and y1 < y2){
        // ROI 범위 생성
        Range r1(y1-img_y, y2-img_y);           // img의 y(row) 범위
        Range r2(x1-img_x, x2-img_x);           // img의 x(col) 범위
        Range r3(y1-img_sub_y, y2-img_sub_y);   // img_sub의 y(row) 범위
        Range r4(x1-img_sub_x, x2-img_sub_x);   // img_sub의 x(col) 범위
        
        // ROI 생성
        Mat roi1 = img(r1, r2);
        Mat roi2 = img_sub(r3, r4);
        Mat roi_result = img_result(r1, r2);
        
        // 흑백 이미지 합치기(min)
        // 알파 채널 분리 고려 하기...!
        min(roi1, roi2, roi_result);
        
        // 겹치는 영역 찾기 성공, img_result 반환
        return img_result;
    }
    else {
        // 겹치는 영역 찾기 실패, img 반환
        return img.clone();
    }
}
cv::Mat Note::rotate_mat(const cv::Mat& img, double degree){
    using namespace std;
    using namespace cv;
    return img.clone();
}
cv::Mat Note::scale_mat(const cv::Mat& img, double scale){
    using namespace std;
    using namespace cv;
    
    // img_result 생성
    Mat img_result;
    
    // 변경되고자 하는 크기
    Size_<int> size = (Size_<double>)img.size()*scale;
    
    // 이미지 확대 및 축소
    if (scale > 1.0) {
        resize(img, img_result, size, 0, 0, INTER_LINEAR);
    }
    else if (scale < 1.0) {
        resize(img, img_result, size, 0, 0, INTER_AREA);
    }
    else return img.clone();
    
    // scale 처리된 이미지 반환
    return img_result;
}

}
