#include <msig.hpp>

namespace msig
{


void Note::add(std::string key, std::string config_symmetry, std::string config_progress){
    using namespace std;
    using namespace cv;
    
    // 추가적인 config
    string additional_config = "";
    
    // key 존재 확인
    if (imgs.find(key)==imgs.end()){
        cout << "msig: ";
        cout << __FUNCTION__ << "("+key+") : ";
        cout << "("+key+") is not exist.";
        cout << endl;
        return;
    }
    
    // config_symmetry(대칭(x축 또는 y축)) 정규표현식 확인 후 추가
    if (regex_match(config_symmetry, regex("^(false)|[xXyY]$")))
        additional_config.append("_"+config_symmetry);
    else additional_config.append("_false");
    
    // config_progress(전체길이_현재지점) 정규표현식 확인 후 추가
    if (regex_match(config_progress, regex("^(false_false)|([0-9]+_([0-9]?[0-9]|100))$")))
        additional_config.append("_"+config_progress);
    else additional_config.append("_false_false");
    
    // key-value 저장
    draw_list[key] = imgs_config[key] + additional_config;
}


}
