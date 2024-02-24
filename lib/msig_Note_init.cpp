#include <msig.hpp>

namespace msig
{


// 악상 기호 이미지들을 불러오기(존재하는것만)
void Note::load_img_symbols(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 
    path d1(ds_dir);
    
    for (auto symbol_name : ds_cons){
        // 이미지 경로 생성
        path d2(symbol_name.substr(0, symbol_name.find("_")));
        path d3(symbol_name.substr(symbol_name.find("_")+1, symbol_name.length()) + ".png");
        path d = d1 / d2 / d3;
        string img_dir = d.string();
        
        // 이미지 재생성(이상한 형식의 이미지 제대로 저장)
        restore_image(img_dir);
        
        // 이미지 존재 여부 확인
        if (!exists(path(img_dir))) continue;
        
        // 이미지 불러오기
        Mat img = imread(img_dir, IMREAD_GRAYSCALE);
        if (img.data){
            img_symbols[symbol_name] = img.clone();
            img.release();
        }
    }
}

// 악상 기호 이미지설정값들을 불러오기(없으면 생성)
void Note::load_img_configs(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // symbol_dataset_config.txt 주소 생성
    path d = path(ds_dir) / path("symbol_dataset_config.txt");
    string config_dir = d.string();
    
    // symbol_dataset_config.txt 파일 생성
    if (!exists(d)){
        fstream(config_dir, ios::out|ios::app).close();
    }
    
    // symbol_dataset_config.txt 파일 열기
    fstream fin(config_dir, ios::in);
    if (!fin){
        cout << "msig: ";
        cout << "can't open " << config_dir << " for reading.";
        cout << endl;
        exit(1);
    }
    
    // config 정보 읽기
    while (true) {
        // 파일 읽기
        string line;
        if (!getline(fin, line)) break;
        
        // 케이지 리턴 문자 제거
        if (line[line.length()-1]=='\r')
            line = line.substr(0, line.length()-1);
        
        // 소문자로 변환
        string_to_lower(line);

        // 정규표현식 생성
        string re_dir = "([a-zA-Z0-9_-]+)";
        string re_i = "([-+]?[0-9]+)";
        string re_d = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
        regex reg("^"+re_dir+"="+re_i+"_"+re_i+"_"+re_d+"_"+re_d+"$");
        
        // 정규표현식 검사
        if (!regex_match(line, reg)) continue;
        
        // symbol_name 과 symbol_config 구하기
        string symbol_name = line.substr(0, line.find("="));
        string symbol_config = line.substr(line.find("=")+1, line.length());
        
        // img_configs에 저장
        img_configs[symbol_name] = symbol_config;
    }
    fin.close();
    
    // 무결성 유지
    integrity_symbols();
    integrity_configs();
    
    // config 정보 저장
    save_symbol_dataset_config();
}


}
