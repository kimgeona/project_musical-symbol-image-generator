#include <msig_Algorithm_MSPA.hpp>

namespace msig
{


// 생성자
MSPA::MSPA()
{
    //
    staff = false;
    
    //
    pitch = 1;
    
    // 초기화
    for (int i=0; i<4; i++)
    {
        edge[i] = 0;
        in  [i] = 0;
        out [i] = 0;
        pad [i] = 0;
    }
}


// 수동 배치 함수
void MSPA::add(const MusicalSymbol& ms, MSPA_POSITION_TYPE position)
{
    // 입력 받은 악상기호 복사
    MusicalSymbol ms_copy(ms);
    
    // 악상 기호의 현재 scale과 degree를 기본값으로 설정
    ms_copy.set_default();
    
    // direction 검사
    if (position == MSPA_FIXED  ||
        position & MSPA_TOP     ||
        position & MSPA_BOTTOM  ||
        position & MSPA_LEFT    ||
        position & MSPA_RIGHT)
    {
        // 저장
        ms_inputs.push_back(ms_copy);
        ms_positions.push_back(position);
    }
    else if (position == MSPA_STAFF)
    {
        // 오선지 중복 확인
        if (staff)  throw std::runtime_error("MSPA::add() : 오선지는 하나만 배치할 수 있습니다.");
        else        staff = true;
        
        // pitch, ledger 정보 추출
        std::vector<std::string> staff_data = my_split(my_split(ms.dir.filename().string(), "~")[0], "-");
        
        // 숫자값으로 변환
        int p = this->pitch = pitch_to_number(staff_data[1]);
        int t = pitch_to_number("f5") + 2 * stoi(staff_data[2]);
        int b = pitch_to_number("e4") - 2 * stoi(staff_data[3]);
        
        // pitch, top_ledger_count, bottom_ledger_count 값 확인
        if (p < b-1 || p > t+1) throw std::runtime_error("MSPA::add() : 오선지의 pitch와 ledger 정보가 잘못되었습니다.");
        
        // 경계(edge) 위치 계산
        edge[0] = (t - p) / 2;
        edge[1] = (p - b) / 2;
        edge[2] = 100;
        edge[3] = 100;
        
        // 오선지 안(in) 위치 계산
        if (edge[0] > 0 && p % 2 == 0) {
            in[0] = 1;
            in[1] = 1;
        }
        
        // 오선지 밖(out) 위치 계산
        out[0] = edge[0];
        out[1] = edge[1];
        out[2] = edge[2];
        out[3] = edge[3];
        
        // 저장
        ms_inputs.push_back(ms_copy);
        ms_positions.push_back(MSPA_FIXED);
    }
    else throw std::runtime_error("MSPA::add() : 잘못된 direction 설정입니다.");
}


// 자동 배치 함수
void MSPA::add(const MusicalSymbol& ms)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 악상 기호 상태 확인
    if (ms.status==-1) throw std::runtime_error("MSPA::add() : 해당 악상기호는 비활성 상태이므로 배치할 수 없습니다.");
    
    // 악상 기호 배치 정보
    MSPA_POSITION_TYPE position = 0b00000000;
    
    // 악상 기호 배치 정보 추출
    vector<string> position_data = my_split(ms.dir.filename().string(), "~");
    int count = 0;
    for (auto& s : position_data)
    {
        if (count++)
        {
            // 소문자 변환
            for (auto& c : s) c = std::tolower(c);
            
            // 종류 감지
            if      (s.find("fixed")    !=std::string::npos) position |= MSPA_FIXED;
            else if (s.find("top")      !=std::string::npos) position |= MSPA_TOP;
            else if (s.find("bottom")   !=std::string::npos) position |= MSPA_BOTTOM;
            else if (s.find("left")     !=std::string::npos) position |= MSPA_LEFT;
            else if (s.find("right")    !=std::string::npos) position |= MSPA_RIGHT;
            else if (s.find("staff")    !=std::string::npos) position |= MSPA_STAFF;
            else if (s.find("in")       !=std::string::npos) position |= MSPA_IN;
            else if (s.find("out")      !=std::string::npos) position |= MSPA_OUT;
        }
    }
    
    // 악상 기호 배치
    add(ms, position);
}


// 연산
int MSPA::pitch_to_number(const std::string& pitch)
{
    // 음정(p)과 옥타브(o)값 추출
    char p = pitch[0];
    int  o = pitch[1] - 48;
    
    // 옥타브 계산
    int number = o * 7;
    
    // 음정 계산
    switch (p)
    {
        case 'c': number += 0; break;
        case 'd': number += 1; break;
        case 'e': number += 2; break;
        case 'f': number += 3; break;
        case 'g': number += 4; break;
        case 'a': number += 5; break;
        case 'b': number += 6; break;
    }
    
    return number;
}


// 배치 완료된 악상기호 구하기
MusicalSymbol MSPA::get(void)
{
    for (int i=0; i<ms_inputs.size(); i++)
    {
        // 배치할 악상기호, 배치정보 꺼내기
        MusicalSymbol       ms = ms_inputs[i];
        MSPA_POSITION_TYPE  position = ms_positions[i];
        
        // 악상기호 배치
        if (position == MSPA_FIXED)
        {
            ms.y += 0;
            ms.x += 0;
        }
        else if (position & MSPA_TOP)
        {
            // 안쪽 배치
            if ((position & MSPA_IN) && (edge[0] > in[0]))
            {
                ms.y += ms.pad * in[0]++;
                
                if (pitch % 2 == 0) ms.y += ms.pad / 2.0;
                else                ms.y += ms.pad;
            }
            
            // 바깥쪽 배치
            else
            {
                // 기존 악상기호의 여백을 제거한 크기의 검은 이미지 생성
                MusicalSymbol ms_copy(ms);
                ms_copy.img = remove_padding(ms_copy.img, ms_copy.x, ms_copy.y);
                ms_copy.img = cv::Mat(ms_copy.img.rows, ms_copy.img.cols, CV_8UC1, cv::Scalar(0));
                
                // 검은 이미지 좌표 이동
                ms_copy.y += ms_copy.pad * out[0];
                ms_copy.y += pad[0];
                
                if (pitch % 2 == 0) ms_copy.y += ms.pad / 2.0;
                else                ms_copy.y += ms.pad;
                
                if (ms_result & ms_copy)
                {
                    while (ms_result & ms_copy){
                        ms_copy.y += 1;
                        pad[0] += 1;
                    }
                    
                    ms_copy.y += ms.pad / 4.5;
                    pad[0] += ms.pad / 4.5;
                }
                
                // 악상기호 좌표 이동
                ms.y += ms.pad * out[0]++;
                ms.y += pad[0];
                
                if (pitch % 2 == 0) ms.y += ms.pad / 2.0;
                else                ms.y += ms.pad;
            }
        }
        else if (position & MSPA_BOTTOM)
        {
            // 안쪽 배치
            if ((position & MSPA_IN) && (edge[1] > in[1]))
            {
                ms.y -= ms.pad * in[1]--;
                
                if (pitch % 2 == 0) ms.y -= ms.pad / 2.0;
                else                ms.y -= ms.pad;
            }
            
            // 바깥쪽 배치
            else
            {
                // 기존 악상기호의 여백을 제거한 크기의 검은 이미지 생성
                MusicalSymbol ms_copy(ms);
                ms_copy.img = remove_padding(ms_copy.img, ms_copy.x, ms_copy.y);
                ms_copy.img = cv::Mat(ms_copy.img.rows, ms_copy.img.cols, CV_8UC1, cv::Scalar(0));
                
                // 검은 이미지 좌표 이동
                ms_copy.y -= ms_copy.pad * out[1];
                ms_copy.y -= pad[1];
                
                if (pitch % 2 == 0) ms_copy.y -= ms.pad / 2.0;
                else                ms_copy.y -= ms.pad;
                
                if (ms_result & ms_copy)
                {
                    while (ms_result & ms_copy){
                        ms_copy.y -= 1;
                        pad[1] += 1;
                    }
                    
                    ms_copy.y -= ms.pad / 4.5;
                    pad[1] += ms.pad / 4.5;
                }
                
                // 악상기호 좌표 이동
                ms.y -= ms.pad * out[1]++;
                ms.y -= pad[1];
                
                if (pitch % 2 == 0) ms.y -= ms.pad / 2.0;
                else                ms.y -= ms.pad;
            }
        }
        else if (position & MSPA_LEFT)
        {
            // 안쪽 배치
            if ((position & MSPA_IN) && (edge[2] > in[2]))
            {
                ms.x += ms.pad * ++(++in[2]);
            }
            
            // 바깥쪽 배치
            else
            {
                ms.x += ms.pad * ++(++out[2]);
            }
        }
        else if (position & MSPA_RIGHT)
        {
            // 안쪽 배치
            if ((position & MSPA_IN) && (edge[3] > in[3]))
            {
                ms.x -= ms.pad * ++(++in[3]);
            }
            
            // 바깥쪽 배치
            else
            {
                ms.x -= ms.pad * ++(++out[3]);
            }
        }
        else throw std::runtime_error("MSPA::get() : 일치하는 배치 정보가 없습니다.");
        
        // 악상기호 합성
        ms_result += ms;
    }
    return this->ms_result;
}


}
