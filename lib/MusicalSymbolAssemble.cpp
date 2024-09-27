
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Algorithm {

MusicalSymbolAssemble::MusicalSymbolAssemble()
{

}

MusicalSymbolAssemble::MusicalSymbolAssemble(const std::initializer_list<MusicalSymbol>& MusicalSymbols) :
mss(MusicalSymbols)
{
    // MS_STAFF인 이미지가 2개 이상 존재하는지 확인
    size_t count = 0;
    for (auto& ms : mss) {
        if (ms.position & MS_STAFF) {
            count++;
        }
    }
    if (count > 1) {
        std::runtime_error("MSIG::Algorithm::MusicalSymbolAssemble.MusicalSymbolAssemble() : staff 이미지는 최대 1개까지 삽입 가능합니다.");
    }
}

int
MusicalSymbolAssemble::__pitch_to_number(const std::string& pitch)
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

bool
MusicalSymbolAssemble::__set_staff(const std::string& staffInfoString,
                                   const int& staffPadding,
                                   int& pitch,
                                   std::array<int, 2>& space,
                                   std::array<int, 2>& edge,
                                   std::array<int, 4>& padding)
{
    // 데이터 추출
    std::vector<std::string> staffData = Utility::split(staffInfoString, "-");
    
    // 숫자 값으로 추출
    pitch = 0;
    int ledgerTop = 0;
    int ledgerBottom = 0;
    try
    {
        pitch        = __pitch_to_number(staffData.at(1));                         // 음정 위치
        ledgerTop    = __pitch_to_number("f5") + 2 * std::stoi(staffData.at(2));   // ledger top 위치
        ledgerBottom = __pitch_to_number("e4") - 2 * std::stoi(staffData.at(3));   // ledger bottom 위치
    }
    catch (const std::out_of_range& e)
    {
        // staff 이름 형식이 잘못되면 에러
        std::string errorMessage = "MSIG::Algorithm::MusicalSymbolAssemble.__set_staff() : \"" + staffInfoString + "\"의 staff 표현 형식이 잘못되었습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 현재 오선지 이미지에서 벗어난 곳에 음정이 설정되어 있는지 확인
    if (pitch < ledgerBottom-1 ||
        pitch > ledgerTop+1) {
        throw std::runtime_error("MSIG::Algorithm::MusicalSymbolAssemble.__set_staff() : \"" + staffInfoString + "\"에서 pitch와 ledger 정보가 잘못되었습니다.");
    }
    
    // 현재 음표의 위치(pitch)를 기준으로 상,하에 악상기호들이 몇 개가 들어갈 수 있는지 계산.
    edge[0] = (ledgerTop - pitch) / 2;
    edge[1] = (pitch - ledgerBottom) / 2;
    
    // 범위 클리핑
    if (edge[0] < 0)
        edge[0] = 0;
    if (edge[1] < 0)
        edge[1] = 0;
    
    // 실선에 걸치는 음표인 경우엔 들어갈 수 있는 공간 1개 감소
    if (edge[0] > 0 && pitch % 2 == 0)
        edge[0]--;
    if (edge[1] > 0 && pitch % 2 == 0)
        edge[1]--;
    
    // 명시적 배치 공간 초기화
    space[0] = 0;
    space[1] = 0;
    
    // 유동적 배치 도움 패딩 초기화
    padding[0] = (ledgerTop - pitch) * staffPadding * 0.5;      // 음정이 오선지 위에 걸터있는경우엔 음수값이 됨. (ex: g5, d4)
    padding[1] = (pitch - ledgerBottom) * staffPadding * 0.5;   // (이하 동일)
    padding[2] = 0;
    padding[3] = 0;
}

void
MusicalSymbolAssemble::__coordinate_adjustment(const int& deltaX, const int& deltaY)
{
    // 바운딩 박스 좌표 및 악상기호 중심 좌표 전부 deltaX, deltaY만큼 증가
    for (auto& [name, coordinate] : coordinates) {
        coordinate[0] += deltaX;
        coordinate[1] += deltaY;
        coordinate[2] += deltaX;
        coordinate[3] += deltaY;
        coordinate[4] += deltaX;
        coordinate[5] += deltaY;
    }
}

bool
MusicalSymbolAssemble::__is_staff_here()
{
    // mss 에서 MS_STAFF인 요소 찾기
    auto it = std::find_if(mss.begin(), mss.end(), [](const MusicalSymbol& ms){
        return ms.position & MS_STAFF;
    });
    
    // 찾으면 true, 못찾으면 false
    if (it==mss.end())
        return false;
    else
        return true;
}

void
MusicalSymbolAssemble::push_back(const MusicalSymbol& ms)
{
    // MS_STAFF 중복 확인
    if ((ms.position & MS_STAFF) && __is_staff_here())
    {
        std::runtime_error("MSIG::Algorithm::MusicalSymbolAssemble.push_back() : staff 이미지는 최대 1개까지 삽입 가능합니다.");
    }
    
    // 맨 뒤에 삽입
    mss.push_back(ms);
}

void
MusicalSymbolAssemble::pop_back()
{
    // 맨 뒤에 제거
    mss.pop_back();
}

MusicalSymbol
MusicalSymbolAssemble::assemble()
{
    // 조합할 악상기호가 있는지 확인
    if (mss.empty())
        throw std::runtime_error("MSIG::Algorithm::MusicalSymbolAssemble.assemble() : 조합할 MusicalSymbol들이 없습니다.");
    
    // 바운딩 박스 좌표 및 악상기호 중심 좌표 초기화
    coordinates.clear();
    
    // 오선지와 함께 배치
    if (__is_staff_here())
    {
        // 배치 변수들
        int pitch;                  // 현재 음정
        std::array<int, 2> space;   // 명시적 배치
        std::array<int, 2> edge;    // 명시적 배치 경계
        std::array<int, 4> padding; // 유동적 배치 도움 패딩
        
        // 오선지 이미지 추출
        MusicalSymbol staffImage = std::find_if(mss.begin(), mss.end(), [](const MusicalSymbol& ms){
            return ms.position & MS_STAFF;
        })->copy();
        staffImage.as_default();
        
        // 배치 변수들 설정
        __set_staff(staffImage.path.stem().string(), staffImage.staffPadding, pitch, space, edge, padding);
        
        // 배치 수행
        for (MusicalSymbol& ms : mss)
        {
            // subImage 생성
            MusicalSymbol subImage = ms.copy();
            
            // subImage의 degree, scalse값을 기본값으로 설정
            subImage.as_default();
            
            // subImage의 여백을 제거한 크기의 검은 이미지 생성
            MusicalSymbol tmpImage = ms.copy();
            tmpImage.image = Processing::Matrix::remove_padding(tmpImage.image, tmpImage.x, tmpImage.y);
            tmpImage.image = cv::Mat(tmpImage.image.rows, tmpImage.image.cols, CV_8UC1, cv::Scalar(0));
            
            // 기존 subImage 중심 좌표 저장
            int originX = tmpImage.x;
            int originY = tmpImage.y;
            
            // staffImage와 합성할 subImage 좌표 수정
            if      (subImage.position == MS_FIXED)
            {
                // 패딩 업데이트
                padding[0] = (tmpImage.y) > (padding[0]) ? (tmpImage.y) : (padding[0]);
                padding[1] = (tmpImage.image.rows - tmpImage.y) > (padding[1]) ? (tmpImage.image.rows - tmpImage.y) : (padding[1]);
                padding[2] = (tmpImage.x) > (padding[2]) ? (tmpImage.x) : (padding[2]);
                padding[3] = (tmpImage.image.cols - tmpImage.x) > (padding[3]) ? (tmpImage.image.cols - tmpImage.x) : (padding[3]);
            }
            else if (subImage.position & MS_TOP)
            {
                // 안쪽 배치
                if ((subImage.position & MS_IN) &&
                    (space[0] < edge[0]))
                {
                    // 남은 공간에 배치
                    subImage.y += staffImage.staffPadding * space[0]++;
                    tmpImage.y += staffImage.staffPadding * space[0]++;
                    
                    // 음표가 실선에 겹쳐져 있는 경우 추가적인 패딩 추가
                    if (pitch % 2 == 0) {
                        subImage.y += staffImage.staffPadding + staffImage.staffPadding / 2.0;
                        tmpImage.y += staffImage.staffPadding + staffImage.staffPadding / 2.0;
                    }
                    else {
                        subImage.y += staffImage.staffPadding;
                        tmpImage.y += staffImage.staffPadding;
                    }
                    
                    // 패딩 업데이트
                    if (padding[0] < tmpImage.y)
                        padding[0] = tmpImage.y;
                }
                // 바깥쪽 배치
                else
                {
                    // 이전과 이어서 배치
                    tmpImage.y += padding[0];
                    
                    // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                    while (static_cast<bool>(staffImage | tmpImage)){
                        tmpImage.y += 1;
                        padding[0] += 1;
                    }
                    
                    // staffImage와 tmpImage 사이 간격 주기
                    tmpImage.y -= staffImage.staffPadding * 0.7;
                    padding[0] += staffImage.staffPadding * 0.7;
                    
                    // subImage 좌표 수정
                    subImage.y += padding[0];
                }
            }
            else if (subImage.position & MS_BOTTOM)
            {
                // 안쪽 배치
                if ((subImage.position & MS_IN) &&
                    (space[1] < edge[1]))
                {
                    // 남은 공간에 배치
                    subImage.y -= staffImage.staffPadding * space[1]++;
                    tmpImage.y -= staffImage.staffPadding * space[1]++;
                    
                    // 음표가 실선에 겹쳐져 있는 경우 추가적인 패딩 추가
                    if (pitch % 2 == 0) {
                        subImage.y -= staffImage.staffPadding + staffImage.staffPadding / 2.0;
                        tmpImage.y -= staffImage.staffPadding + staffImage.staffPadding / 2.0;
                    }
                    else {
                        subImage.y -= staffImage.staffPadding;
                        tmpImage.y -= staffImage.staffPadding;
                    }
                    
                    // 패딩 업데이트
                    if (padding[1] < abs(tmpImage.y) + tmpImage.image.rows)
                        padding[1] = abs(tmpImage.y) + tmpImage.image.rows;
                }
                // 바깥쪽 배치
                else
                {
                    // 이전과 이어서 배치
                    tmpImage.y -= padding[1];
                    
                    // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                    while (static_cast<bool>(staffImage | tmpImage)){
                        tmpImage.y -= 1;
                        padding[1] += 1;
                    }
                    
                    // staffImage와 tmpImage 사이 간격 주기
                    tmpImage.y -= staffImage.staffPadding * 0.7;
                    padding[1] += staffImage.staffPadding * 0.7;
                    
                    // subImage 좌표 수정
                    subImage.y -= padding[1];
                }
            }
            else if (subImage.position & MS_LEFT)
            {
                // sumImage.x += 누적 패딩 값 + 오선지 패딩 * 0.7 + 이미지 중심 좌표에서 오른쪽 크기
                subImage.x += padding[2] + staffImage.staffPadding * 0.7 + (tmpImage.image.cols - tmpImage.x);
                
                // 누적 패딩 += subImage 가로 크기 + 오선지 패딩
                padding[2] += tmpImage.image.cols + staffImage.staffPadding * 0.7 ;
            }
            else if (subImage.position & MS_RIGHT)
            {
                // sumImage.x += 누적 패딩 값 + 오선지 패딩 * 0.7 + 이미지 중심 좌표에서 왼쪽 크기
                subImage.x -= padding[3] + staffImage.staffPadding * 0.7 + (tmpImage.x);
                
                // 누적 패딩 += subImage 가로 크기 + 오선지 패딩
                padding[3] -= tmpImage.image.cols + staffImage.staffPadding;
            }
            
            // 바운딩 박스 좌표, 악상기호 중심 좌표 저장
            coordinates[subImage.path] = std::array<int, 6>({
                staffImage.x - (tmpImage.x - originX) - tmpImage.x,                         // x1
                staffImage.y - (tmpImage.y - originY) - tmpImage.y,                         // y1
                staffImage.x - (tmpImage.x - originX) - tmpImage.x + tmpImage.image.cols,   // x2
                staffImage.y - (tmpImage.y - originY) - tmpImage.y + tmpImage.image.rows,   // y2
                staffImage.x - (tmpImage.x - originX),                                      // cx
                staffImage.y - (tmpImage.y - originY)                                       // cy
            });
            
            // 오선지 이미지는 건너뛰기
            if (subImage.position & MS_STAFF)
                continue;
            
            // staffImage와 subImage 합성
            staffImage = staffImage & subImage;
        }
        
        // 합성 완료된 이미지 주소 없애기
        staffImage.path = std::filesystem::path();
        
        // 합성 완료된 이미지 반환
        return staffImage;
    }
    
    // 오선지를 제외한 배치
    else
    {
        // 배치 변수들
        std::array<int, 4> padding = {0}; // 유동적 배치 도움 패딩
        
        // 완성된 이미지
        MusicalSymbol resultImage = mss[0].copy();
        resultImage.as_default();
        
        // 배치 수행
        for (size_t i=1; i<mss.size(); i++)
        {
            // subImage 생성
            MusicalSymbol subImage = mss[i].copy();
            
            // subImage의 degree, scalse값을 기본값으로 설정
            subImage.as_default();
            
            // subImage의 여백을 제거한 크기의 검은 이미지 생성
            MusicalSymbol tmpImage = mss[i].copy();
            tmpImage.image = Processing::Matrix::remove_padding(tmpImage.image, tmpImage.x, tmpImage.y);
            tmpImage.image = cv::Mat(tmpImage.image.rows, tmpImage.image.cols, CV_8UC1, cv::Scalar(0));
            
            // 기존 subImage 중심 좌표 저장
            int originX = tmpImage.x;
            int originY = tmpImage.y;
            
            // staffImage와 합성할 subImage 좌표 수정
            if      (subImage.position == MS_FIXED)
            {
                // 패딩 업데이트
                padding[0] = (tmpImage.y) > (padding[0]) ? (tmpImage.y) : (padding[0]);
                padding[1] = (tmpImage.image.rows - tmpImage.y) > (padding[1]) ? (tmpImage.image.rows - tmpImage.y) : (padding[1]);
                padding[2] = (tmpImage.x) > (padding[2]) ? (tmpImage.x) : (padding[2]);
                padding[3] = (tmpImage.image.cols - tmpImage.x) > (padding[3]) ? (tmpImage.image.cols - tmpImage.x) : (padding[3]);
            }
            else if (subImage.position & MS_TOP)
            {
                // 이전과 이어서 배치
                tmpImage.y += padding[0];
                
                // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                while (static_cast<bool>(resultImage | tmpImage)){
                    tmpImage.y += 1;
                    padding[0] += 1;
                }
                
                // staffImage와 tmpImage 사이 간격 주기
                tmpImage.y -= resultImage.staffPadding * 0.7;
                padding[0] += resultImage.staffPadding * 0.7;
                
                // subImage 좌표 수정
                subImage.y += padding[0];
            }
            else if (subImage.position & MS_BOTTOM)
            {
                // 이전과 이어서 배치
                tmpImage.y -= padding[1];
                
                // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                while (static_cast<bool>(resultImage | tmpImage)){
                    tmpImage.y -= 1;
                    padding[1] += 1;
                }
                
                // staffImage와 tmpImage 사이 간격 주기
                tmpImage.y -= resultImage.staffPadding * 0.7;
                padding[1] += resultImage.staffPadding * 0.7;
                
                // subImage 좌표 수정
                subImage.y -= padding[1];
            }
            else if (subImage.position & MS_LEFT)
            {
                // 이전과 이어서 배치
                tmpImage.x += padding[2];
                
                // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                while (static_cast<bool>(resultImage | tmpImage)){
                    tmpImage.x += 1;
                    padding[2] += 1;
                }
                
                // staffImage와 tmpImage 사이 간격 주기
                tmpImage.x -= resultImage.staffPadding * 0.7;
                padding[2] += resultImage.staffPadding * 0.7;
                
                // subImage 좌표 수정
                subImage.x += padding[2];
            }
            else if (subImage.position & MS_RIGHT)
            {
                // 이전과 이어서 배치
                tmpImage.x -= padding[3];
                
                // staffImage와 tmpImage가 서로 겹치지 않을때 까지 tmpImage 올리기
                while (static_cast<bool>(resultImage | tmpImage)){
                    tmpImage.x -= 1;
                    padding[3] += 1;
                }
                
                // staffImage와 tmpImage 사이 간격 주기
                tmpImage.x -= resultImage.staffPadding * 0.7;
                padding[3] += resultImage.staffPadding * 0.7;
                
                // subImage 좌표 수정
                subImage.x -= padding[3];
            }
            
            // 바운딩 박스 좌표, 악상기호 중심 좌표 저장
            coordinates[subImage.path] = std::array<int, 6>({
                resultImage.x - (tmpImage.x - originX) - tmpImage.x,                         // x1
                resultImage.y - (tmpImage.y - originY) - tmpImage.y,                         // y1
                resultImage.x - (tmpImage.x - originX) - tmpImage.x + tmpImage.image.cols,   // x2
                resultImage.y - (tmpImage.y - originY) - tmpImage.y + tmpImage.image.rows,   // y2
                resultImage.x - (tmpImage.x - originX),                                      // cx
                resultImage.y - (tmpImage.y - originY)                                       // cy
            });
            
            // 오선지 이미지는 건너뛰기
            if (subImage.position & MS_STAFF)
                continue;
            
            // staffImage와 subImage 합성
            resultImage = resultImage & subImage;
        }
        
        // 합성 완료된 이미지 주소 없애기
        resultImage.path = std::filesystem::path();
        
        // 합성 완료된 이미지 반환
        return resultImage;
        
        return mss[0];
    }
}

std::map<std::filesystem::path, std::array<int, 6>>
MusicalSymbolAssemble::assemble_labels()
{
    return coordinates;
}

}
}
