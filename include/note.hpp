#include <vector>
#include <opencv2/opencv.hpp>

namespace note {
// note 데이터 저장 벡터들
std::vector<bool>           buffer_type;   // 음표 종류 : note, beat
std::vector<std::string>    buffer_pitch;  // 음정 : c4, a5
std::vector<std::string>    buffer_beat;   // 박자 : 2-3 1-1 0.5-1 (박자-쪼개기(잇단 음표))

// note 데이터 추가 함수들
void add_type(bool);            // buffer에 음표 종류 추가
void add_pitch(std::string);    // buffer에 음정 추가
void add_beat(std::string);     // buffer에 박자 추가

// note 데이터 그리기 함수들
cv::Mat draw();             // buffer 내용 그리기
}
