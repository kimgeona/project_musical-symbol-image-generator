#include <msig_Algorithm_MSPA.hpp>

namespace msig {


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


// 배치 함수
void MSPA::add_staff(const MusicalSymbol& ms, std::string pitch, int top_ledger_number, int bottom_ledger_number)
{
    //
    MusicalSymbol ms_copy(ms);
    ms_copy.set_default();
    
    // 오선지 중복 확인
    if (staff) throw std::runtime_error("MSPA::add_staff() : 오선지는 하나만 배치할 수 있습니다.");
    else staff = true;
    
    // pitch, top_ledger_count, bottom_ledger_count 를 숫자값 위치로 변환
    int p = this->pitch = pitch_to_number(pitch);
    int t = pitch_to_number("f5") + 2 * top_ledger_number;
    int b = pitch_to_number("e4") - 2 * bottom_ledger_number;
    
    // pitch, top_ledger_count, bottom_ledger_count 값 확인
    if (p < b-1 || p > t+1) throw std::runtime_error("MSPA::add_staff() : 서로 올바르지 못한 관계의 인자값이 입력되었습니다.");
    
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
    out[0] = edge[0] + 1;
    out[1] = edge[1] + 1;
    out[2] = edge[2] + 1;
    out[3] = edge[3] + 1;
    
    // 저장
    ms_inputs.push_back(ms_copy);
    ms_positions.push_back("fixed");
}
void MSPA::add_fixed(const MusicalSymbol& ms)
{
    //
    MusicalSymbol ms_copy(ms);
    ms_copy.set_default();
    
    // 저장
    ms_inputs.push_back(ms_copy);
    ms_positions.push_back("fixed");
}
void MSPA::add_in(const MusicalSymbol& ms, std::string direction)
{
    std::string position;
    
    //
    MusicalSymbol ms_copy(ms);
    ms_copy.set_default();
    
    // direction 검사
    if (direction == "top"    ||
        direction == "bottom" ||
        direction == "left"   ||
        direction == "right")
        position = direction;
    else
        throw std::runtime_error("MSPA::add_in() : 잘못된 direction 설정입니다.");
    
    // 저장
    ms_inputs.push_back(ms_copy);
    ms_positions.push_back("in_" + position);
}
void MSPA::add_out(const MusicalSymbol& ms, std::string direction)
{
    std::string position;
    
    //
    MusicalSymbol ms_copy(ms);
    ms_copy.set_default();
    
    // direction 검사
    if (direction == "top"    ||
        direction == "bottom" ||
        direction == "left"   ||
        direction == "right")
        position = direction;
    else
        throw std::runtime_error("MSPA::add_in() : 잘못된 direction 설정입니다.");
    
    // 저장
    ms_inputs.push_back(ms_copy);
    ms_positions.push_back("out_" + position);
}


// 자동 배치 함수
void MSPA::add(const MusicalSymbol& ms)
{
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    /*
---- 폴더 ----
     
     edge-left-@                                    : fixed
     edge-left-@    measure-@                       : fixed
     edge-left-@    measure-@       octave-@        : --
     edge-left-@    measure-@       repetition-@    : out_top

     edge-right-@                                   : fixed
     edge-right-@   measure-@                       : fixed
     edge-right-@   measure-@       octave-@        : --
     edge-right-@   measure-@       repetition-@    : out_top

     line-@                                         : staff
     line-@         note-down-@                     : fixed
     line-@         note-down-@     accidental-#    : in_left
     line-@         note-down-@     articulation-#  : --
     line-@         note-down-@     dynamic-#       : out_bottom
     line-@         note-down-@     octave-#        : --
     line-@         note-down-@     ornament-#      : out_top
     line-@         note-down-@     repetition-#    : out_top
     line-@         note-up-@                       : fixed
     line-@         note-up-@       accidental-#    : in_left
     line-@         note-up-@       articulation-#  : --
     line-@         note-up-@       dynamic-#       : out_bottom
     line-@         note-up-@       octave-#        : --
     line-@         note-up-@       ornament-#      : out_top
     line-@         note-up-@       repetition-#    : out_top

     line-fixed-@                                   : fixed
     line-fixed-@   clef-@                          : fixed
     line-fixed-@   key-@                           : fixed
     line-fixed-@   measure-@                       : fixed
     line-fixed-@   repetition-@                    : fixed
     line-fixed-@   rest-@                          : fixed
     line-fixed-@   time-@                          : fixed
     
---- 끝 ----
     */
    vector<path> folder_staff       = {
        path("new-symbol-dataset")/path("complete")/path("line-@")
    };
    vector<path> folder_fixed       = {
        path("new-symbol-dataset")/path("complete")/path("edge-left-@"),
        path("new-symbol-dataset")/path("complete")/path("edge-left-@")/path("measure-@"),
        path("new-symbol-dataset")/path("complete")/path("edge-right-@"),
        path("new-symbol-dataset")/path("complete")/path("edge-right-@")/path("measure-@"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("clef-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("key-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("measure-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("repetition-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("rest-@"),
        path("new-symbol-dataset")/path("complete")/path("line-fixed-@")/path("time-@")
    };
    vector<path> folder_out_top     = {
        path("new-symbol-dataset")/path("complete")/path("edge-left-@")/path("measure-@")/path("repetition-@"),
        path("new-symbol-dataset")/path("complete")/path("edge-right-@")/path("measure-@")/path("repetition-@"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("ornament-#"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("repetition-#"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("ornament-#"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("repetition-#")
    };
    vector<path> folder_out_bottom  = {
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("dynamic-#"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("dynamic-#")
    };
    vector<path> folder_in_left     = {
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("accidental-#"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("accidental-#")
    };
    
    /*
---- 파일 ----
  
     edge-left-@    measure-@       octave-@        : --
     |- octave-down-middle.png                      : out_bottom
     |- octave-up-middle.png                        : out_top

     edge-right-@   measure-@       octave-@        : --
     |- octave-down-middle.png                      : out_bottom
     |- octave-up-middle.png                        : out_top

     line-@         note-down-@     articulation-#  : --
     |- accent.png                                  : out_top
     |- fermata.png                                 : out_top
     |- marcato.png                                 : out_top
     |- portato.png                                 : out_top
     |- staccatissimo.png                           : out_top
     |- staccato.png                                : in_top
     |- tenuto.png                                  : in_top
     
     line-@         note-up-@       articulation-#  : --
     |- accent.png                                  : out_bottom
     |- fermata.png                                 : out_bottom
     |- marcato.png                                 : out_bottom
     |- portato.png                                 : out_bottom
     |- staccatissimo.png                           : out_bottom
     |- staccato.png                                : in_bottom
     |- tenuto.png                                  : in_bottom

     line-@         note-down-@     octave-#        : --
     |- octave-down-end.png                         : out_bottom
     |- octave-down-middle.png                      : out_bottom
     |- octave-down-start-8.png                     : out_bottom
     |- octave-down-start-15.png                    : out_bottom
     |- octave-up-end.png                           : out_top
     |- octave-up-middle.png                        : out_top
     |- octave-up-start-8.png                       : out_top
     |- octave-up-start-15.png                      : out_top

     line-@         note-up-@       octave-#        : --
     |- octave-down-end.png                         : out_bottom
     |- octave-down-middle.png                      : out_bottom
     |- octave-down-start-8.png                     : out_bottom
     |- octave-down-start-15.png                    : out_bottom
     |- octave-up-end.png                           : out_top
     |- octave-up-middle.png                        : out_top
     |- octave-up-start-8.png                       : out_top
     |- octave-up-start-15.png                      : out_top
     
---- 끝 ----
     */
    vector<path> file_out_top       = {
        path("octave-up-end.png"),
        path("octave-up-middle.png"),
        path("octave-up-start-8.png"),
        path("octave-up-start-15.png"),
        path("accent.png"),
        path("fermata.png"),
        path("marcato.png"),
        path("portato.png"),
        path("staccatissimo.png"),
    };
    vector<path> file_out_bottom    = {
        path("octave-down-end.png"),
        path("octave-down-middle.png"),
        path("octave-down-start-8.png"),
        path("octave-down-start-15.png"),
    };
    vector<path> file_in_top        = {
        path("staccato.png"),
        path("tenuto.png"),
    };
    
    // 악상기호 상태 확인
    if (ms.status==-1) throw std::runtime_error("MSPA::add() : 해당 악상기호는 비활성 상태이므로 배치할 수 없습니다.");
    
    // 파일 목록 일치하는것 찾기
    else if (find(file_out_top.begin(),     file_out_top.end(),     ms.dir.filename()) != file_out_top.end())       add_out(ms, "top");
    else if (find(file_out_bottom.begin(),  file_out_bottom.end(),  ms.dir.filename()) != file_out_bottom.end())    add_out(ms, "bottom");
    else if (find(file_in_top.begin(),      file_in_top.end(),      ms.dir.filename()) != file_in_top.end())        add_in(ms, "top");
    
    // 폴더 목록 일치하는것 찾기
    else if (find(folder_staff.begin(),         folder_staff.end(),         ms.dir.parent_path()) != folder_staff.end()){
        vector<string> tmp = my_split(ms.dir.filename(), "-");
        add_staff(ms, tmp[1], stoi(tmp[2]), stoi(tmp[3]));
    }
    else if (find(folder_fixed.begin(),         folder_fixed.end(),         ms.dir.parent_path()) != folder_fixed.end())        add_fixed(ms);
    else if (find(folder_out_top.begin(),       folder_out_top.end(),       ms.dir.parent_path()) != folder_out_top.end())      add_out(ms, "top");
    else if (find(folder_out_bottom.begin(),    folder_out_bottom.end(),    ms.dir.parent_path()) != folder_out_bottom.end())   add_out(ms, "bottom");
    else if (find(folder_in_left.begin(),       folder_in_left.end(),       ms.dir.parent_path()) != folder_in_left.end())      add_in(ms, "left");
    
    // 매칭 되지 않은 악상기호들
    else
    {
        cout << "MSPA::add() : 악상기호 " << ms.dir.filename().string() << "에 대한 설정이 없어 기본(fixed)으로 진행합니다." << endl;
        add_fixed(ms);
    }
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
        MusicalSymbol   ms = ms_inputs[i];
        std::string     position = ms_positions[i];
        
        // 악상기호 합치기
        if (position == "fixed")
        {
            ms_result += ms;
        }
        else if (position == "in_top")
        {
            // 안쪽 배치 공간이 남은 경우
            if (edge[0] > in[0])
            {
                ms.y += ms.pad * ++in[0];
                
                if (pitch % 2 == 0)
                    ms.y += ms.pad / 2.0;
            }
            
            // 남지 않은 경우
            else 
            {
                ms.y += ms.pad * ++out[0];
                ms.y += pad[0];
                
                if (pitch % 2 == 0)
                    ms.y += ms.pad / 2.0;
                
                if (ms_result & ms)
                {
                    while (ms_result & ms){
                        ms.y += 1;
                        pad[0] += 1;
                    }
                    
                    ms.y += ms.pad / 4.5;
                    pad[0] += ms.pad / 4.5;
                }
            }
            
            ms_result += ms;
        }
        else if (position == "in_bottom")
        {
            // 안쪽 배치 공간이 남은 경우
            if (edge[1] > in[1])
            {
                ms.y -= ms.pad * ++in[1];
                if (pitch % 2 == 0)
                    ms.y -= ms.pad / 2.0;
            }
            
            // 남지 않은 경우
            else
            {
                ms.y -= ms.pad * ++out[1];
                ms.y -= pad[1];
                
                if (pitch % 2 == 0)
                    ms.y -= ms.pad / 2.0;
                
                if (ms_result & ms)
                {
                    while (ms_result & ms){
                        ms.y -= 1;
                        pad[1] += 1;
                    }
                    
                    ms.y -= ms.pad / 4.5;
                    pad[1] += ms.pad / 4.5;
                }
            }
            
            ms_result += ms;
        }
        else if (position == "in_left")
        {
            // 안쪽 배치 공간이 남은 경우
            if (edge[2] > in[2])
            {
                ms.x += ms.pad * ++(++in[2]);
            }
            
            // 남지 않은 경우
            else
            {
                ms.x += ms.pad * ++(++out[2]);
            }
            
            ms_result += ms;
        }
        else if (position == "in_right")
        {
            // 안쪽 배치 공간이 남은 경우
            if (edge[3] > in[3])
            {
                ms.x -= ms.pad * ++(++in[3]);
            }
            
            // 남지 않은 경우
            else
            {
                ms.x -= ms.pad * ++(++out[3]);
            }
            
            ms_result += ms;
        }
        else if (position == "out_top")
        {
            ms.y += ms.pad * ++out[0];
            ms.y += pad[0];
            
            if (pitch % 2 == 0)
                ms.y += ms.pad / 2.0;
            
            if (ms_result & ms)
            {
                while (ms_result & ms){
                    ms.y += 1;
                    pad[0] += 1;
                }
                
                ms.y += ms.pad / 4.5;
                pad[0] += ms.pad / 4.5;
            }
            
            ms_result += ms;
        }
        else if (position == "out_bottom")
        {
            ms.y -= ms.pad * ++out[1];
            ms.y -= pad[1];
            
            if (pitch % 2 == 0)
                ms.y -= ms.pad / 2.0;
            
            if (ms_result & ms)
            {
                while (ms_result & ms){
                    ms.y -= 1;
                    pad[1] += 1;
                }
                
                ms.y -= ms.pad / 4.5;
                pad[1] += ms.pad / 4.5;
            }
            
            ms_result += ms;
        }
        else if (position == "out_left")
        {
            ms.x += ms.pad * ++(++out[2]);
            
            ms_result += ms;
        }
        else if (position == "out_right")
        {
            ms.x -= ms.pad * ++(++out[3]);
            
            ms_result += ms;
        }
        else
            throw std::runtime_error("MSPA::get() : 일치하는 배치 정보가 없습니다.");
    }
    
    return this->ms_result;
}


// 초기화
void MSPA::reset()
{
    
}


}
