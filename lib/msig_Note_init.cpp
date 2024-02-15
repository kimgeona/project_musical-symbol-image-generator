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

std::string Note::make_config(std::string key){
    using namespace std;
    using namespace cv;
    
    // 1. 하얀 배경에 가운데 + 가 박혀져 있는 이미지 생성
    // 2. 해당 이미지 창에 띄움. 창 이름은 key로 설정
    // 3. config 정보를 생성하고자 하는 악상기호 이미지 띄우기.
    //     3-1. 방향키로 이미지 위치 조정
    //     3-2. 마우스 스크롤로 이미지 확대 및 축소
    //     3-3. 엔터시 저장 및 나가기, esc 입력시 나가기
    
    return "0_0_0.0_0.0";
}


}
