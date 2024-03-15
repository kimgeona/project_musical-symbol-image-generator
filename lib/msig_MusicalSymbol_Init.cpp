#include <msig_MusicalSymbol.hpp>

namespace msig
{


// msig_MusicalSymbol_Init.cpp
int MusicalSymbol::init_dir         (std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // .png 파일인지 확인 후 저장
    if (exists(dir) && is_regular_file(dir)){
        this->dir = dir;
        return 0;
    }
    else return 1;
}
int MusicalSymbol::init_dir_config  (std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // .txt 파일인지 확인 후 저장
    if (exists(dir) && is_regular_file(dir)){
        this->dir_config = dir;
        return 0;
    }
    else{
        this->dir = path();
        return 1;
    }
}
int MusicalSymbol::init_img         (){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 전처리
    restore_img(this->dir);
    //remove_padding(this->dir);
    
    // 이미지 불러오기
    this->img = imread(this->dir.string(), IMREAD_GRAYSCALE);
    
    // 불러온 이미지 확인
    if (this->img.data){
        return 0;
    }
    else{
        this->img = Mat();
        return 1;
    }
}
int MusicalSymbol::init_config      (){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 나의 디렉토리 표기법으로 주소 변환
    string search_dir = my_dir(this->dir);
    
    // config 파일에서 dir 찾기
    string find = my_grep(this->dir_config, search_dir);
    
    // config 정보 저장
    if (find==""){
        return make_config();
    }
    else {
        // key=value 분리
        find = find.substr(search_dir.length()+1, find.length());
        // value 정규표현식 검사
        string re_i = "([-+]?[0-9]+)";
        string re_d = "([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))";
        regex reg("^"+re_i+"_"+re_i+"_"+re_d+"_"+re_d+"$");
        // 정규표현식 매칭 확인 후 저장
        if (regex_match(find, reg)){
            vector<string> configs = my_split(find, "_");
            this->x         = stoi(configs[0]);
            this->y         = stoi(configs[1]);
            this->scale     = stod(configs[2]);
            this->rotate    = stod(configs[3]);
            return 0;
        }
        else {
            return make_config();
        }
    }
}


}
