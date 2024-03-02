#include <msig.hpp>

namespace msig
{


void Note::set(std::string note,
         std::string relationship,
         std::string dynamic,
         std::string articulations_mark,
         std::string ornament,
         std::string octavesign,
         std::string kegetition_and_codas)
{
    using namespace std;
    using namespace cv;
    
    // 인자값 화인 : note가 잘 작성되었는지
    // 인자값 확인 : 존재하는 이미지인지
    
    // draw_list에 ln_staff 추가
    draw_list["ln_staff"] = img_configs["ln_staff"];
    
    // 상호관계 파악하여 전체적으로 "x_y_각도_확대축소_대칭" 수정 후 draw_list에 추가
    draw_list["nt_8"] = img_configs["nt_8"]; // 확인을 위한 임시 코드
}


}
