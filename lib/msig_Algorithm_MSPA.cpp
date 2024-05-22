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
        std::vector<std::string> staff_data = my_split(ms.dir.filename(), "-");
        
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
     |- fermata-up.png                              : out_top
     |- marcato-up.png                              : out_top
     |- portato.png                                 : out_top
     |- staccatissimo.png                           : out_top
     |- staccato.png                                : in_top
     |- tenuto.png                                  : in_top
     
     line-@         note-up-@       articulation-#  : --
     |- accent.png                                  : out_bottom
     |- fermata-down.png                            : out_bottom
     |- marcato-down.png                            : out_bottom
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
        path("new-symbol-dataset")/path("complete")/path("edge-left-@")/path("measure-@")/path("octave-@")/path("octave-up-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("edge-right-@")/path("measure-@")/path("octave-@")/path("octave-up-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("accent.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("fermata-up.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("marcato-up.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("portato.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("staccatissimo.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-up-end.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-up-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-up-start-8.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-up-start-15.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-up-end.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-up-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-up-start-8.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-up-start-15.png"),
    };
    vector<path> file_out_bottom    = {
        path("new-symbol-dataset")/path("complete")/path("edge-left-@")/path("measure-@")/path("octave-@")/path("octave-down-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("edge-right-@")/path("measure-@")/path("octave-@")/path("octave-down-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("accent.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("fermata-down.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("marcato-down.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("portato.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("staccatissimo.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-down-end.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-down-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-down-start-8.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("octave-#")/path("octave-down-start-15.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-down-end.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-down-middle.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-down-start-8.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("octave-#")/path("octave-down-start-15.png"),
    };
    vector<path> file_in_top        = {
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("staccato.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#")/path("tenuto.png"),
    };
    vector<path> file_in_bottom     = {
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("staccato.png"),
        path("new-symbol-dataset")/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#")/path("tenuto.png"),
    };
    
    // 악상기호 상태 확인
    if (ms.status==-1) throw std::runtime_error("MSPA::add() : 해당 악상기호는 비활성 상태이므로 배치할 수 없습니다.");
    
    // 파일 목록 일치하는것 찾기
    else if (find(file_out_top.begin(),     file_out_top.end(),     ms.dir) != file_out_top.end())      add(ms, MSPA_TOP | MSPA_OUT);
    else if (find(file_out_bottom.begin(),  file_out_bottom.end(),  ms.dir) != file_out_bottom.end())   add(ms, MSPA_BOTTOM | MSPA_OUT);
    else if (find(file_in_top.begin(),      file_in_top.end(),      ms.dir) != file_in_top.end())       add(ms, MSPA_TOP | MSPA_IN);
    else if (find(file_in_bottom.begin(),   file_in_bottom.end(),   ms.dir) != file_in_bottom.end())    add(ms, MSPA_BOTTOM | MSPA_IN);
    
    // 폴더 목록 일치하는것 찾기
    else if (find(folder_staff.begin(),         folder_staff.end(),         ms.dir.parent_path()) != folder_staff.end())      add(ms, MSPA_STAFF);
    else if (find(folder_fixed.begin(),         folder_fixed.end(),         ms.dir.parent_path()) != folder_fixed.end())      add(ms, MSPA_FIXED);
    else if (find(folder_out_top.begin(),       folder_out_top.end(),       ms.dir.parent_path()) != folder_out_top.end())    add(ms, MSPA_TOP | MSPA_OUT);
    else if (find(folder_out_bottom.begin(),    folder_out_bottom.end(),    ms.dir.parent_path()) != folder_out_bottom.end()) add(ms, MSPA_BOTTOM | MSPA_OUT);
    else if (find(folder_in_left.begin(),       folder_in_left.end(),       ms.dir.parent_path()) != folder_in_left.end())    add(ms, MSPA_LEFT | MSPA_IN);
    
    // 매칭 되지 않은 악상기호들
    else
    {
        cout << "MSPA::add() : 악상기호 " << ms.dir.string() << "에 대한 설정이 없어 기본(fixed)으로 진행합니다." << endl;
        add(ms, MSPA_FIXED);
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
                ms.y += ms.pad * out[0]++;
                ms.y += pad[0];
                
                if (pitch % 2 == 0) ms.y += ms.pad / 2.0;
                else                ms.y += ms.pad;
                
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
                ms.y -= ms.pad * out[1]++;
                ms.y -= pad[1];
                
                if (pitch % 2 == 0) ms.y -= ms.pad / 2.0;
                else                ms.y -= ms.pad;
                
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
