#include <msig.hpp>

namespace msig
{


void Note::set(std::string type, std::string name){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 선택 가능한 악상 기호 구하기
    std::vector<MusicalSymbol> available_ms = get_selectable(ds, draw_list);
    
    // 찾기 : 찾을 객체 생성
    MusicalSymbol ms;
    ms.dir_simple = (path(type)/path(name)).string();
    
    // 찾기 : 수행
    auto it = find(available_ms.begin(), available_ms.end(), ms);
    
    // 성공
    if (it!=available_ms.end()){
        draw_list.push_back(*it);
    }
    // 실패
    else {
        cout << "msig : [" << path(type) / path(name) << "] 이미지는 선택할 수 없습니다." << endl;
    }
}


}
