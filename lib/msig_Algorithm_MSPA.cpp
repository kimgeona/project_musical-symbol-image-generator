#include <msig_Algorithm_MSPA.hpp>

namespace msig {


void MSPA::add(MusicalSymbol& ms, std::string position, std::vector<std::string> except)
{
    using namespace std;
    
    // 처음 배치하는 경우
    if (ms_buff==MusicalSymbol())
    {
        for (auto& s : except)
        {
            if (ms.dir.parent_path().filename().string().find(s)!=std::string::npos)
            {
                cout << "msig::MSPA::add() : 자기 자신을 제외하여 그릴 순 없습니다." << endl;
                return;
            }
        }
        this->ms_buff = ms;                                                                 // 이미지 저장
        this->ms_except = except;                                                           // 이미지 예외 저장
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
        return;
    }
    
    // 해당 except를 제외하여 ms_buff 다시 생성
    if (this->ms_except != except)
    {
        regenerate(except);
    }
    
    // ms_buff를 와 해당 position을 참고하여 배치
    if (position=="fixed")
    {
        this->ms_buff += ms;                                                                // 이미지 합치기
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
    }
    else if (position=="top")
    {
        while (true) // 이미지 위치 계산
        {
            // 겹치지 않을 때 까지 올려보내기
            while (ms_buff & ms)
            {
                ms.y = ms.y + 1;
            }
            
            // 추가 간격 조정
            ms.y = ms.y + (ms.pad / 4.5);
            
            // 겹치는지 확인
            if (ms_buff & ms)   continue;
            else                break;
        }
        this->ms_buff += ms;                                                                // 이미지 합치기
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
    }
    else if (position=="bottom")
    {
        while (true) // 이미지 위치 계산
        {
            // 겹치지 않을 때 까지 내려보내기
            while (ms_buff & ms)
            {
                ms.y = ms.y - 1;
            }
            
            // 추가 간격 조정
            ms.y = ms.y - (ms.pad / 4.5);
            
            // 겹치는지 확인
            if (ms_buff & ms)   continue;
            else                break;
        }
        this->ms_buff += ms;                                                                // 이미지 합치기
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
    }
    else if (position=="left")
    {
        while (true) // 이미지 위치 계산
        {
            // 겹치지 않을 때 까지 왼쪽으로보내기
            while (ms_buff & ms)
            {
                ms.x = ms.x + 1;
            }
            
            // 추가 간격 조정
            ms.x = ms.x + (ms.pad / 4.5);
            
            // 겹치는지 확인
            if (ms_buff & ms)   continue;
            else                break;
        }
        this->ms_buff += ms;                                                                // 이미지 합치기
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
    }
    else if (position=="right")
    {
        while (true) // 이미지 위치 계산
        {
            // 겹치지 않을 때 까지 왼쪽으로보내기
            while (ms_buff & ms)
            {
                ms.x = ms.x - 1;
            }
            
            // 추가 간격 조정
            ms.x = ms.x - (ms.pad / 4.5);
            
            // 겹치는지 확인
            if (ms_buff & ms)   continue;
            else                break;
        }
        this->ms_buff += ms;                                                                // 이미지 합치기
        this->ms_list[(ms.dir.parent_path().filename()/ms.dir.filename()).string()] = ms;   // 이미지 기록
    }
    else
    {
        cout << "msig::MSPA::add() : 잘못된 position 값입니다." << endl;
        return;
    }
}

void MSPA::regenerate(std::vector<std::string> except)
{
    // 기존 값 초기화
    this->ms_buff = MusicalSymbol();
    this->ms_except = except;
    
    // ms_list안에 내용들을 ms_buff에 다시 그리기
    for (auto& e : this->ms_list)
    {
        // ms_except 이미지들 제외
        bool pass = false;
        for (auto& s : this->ms_except)
        {
            if (e.first.find(s)!=std::string::npos)
            {
                pass = true;
                break;
            }
        }
        if (pass) continue;
        
        // 그리기
        if (ms_buff == MusicalSymbol()) ms_buff = e.second;     // 처음일 경우
        else                            ms_buff += e.second;
    }
}

MusicalSymbol MSPA::get(void)
{
    regenerate(std::vector<std::string>());
    return this->ms_buff;
}


}
