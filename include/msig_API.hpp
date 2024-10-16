/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_API.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개별 헤더 파일들 중 하나로, 사용자가 직접 손쉽게 사용할 수 있는 함수들이 작성되어있습니
 다.
 -------------------------------------------------------------------------------
 포함된 기능:
 - make_multiple_notes() : 음표들의 머리 이미지를 이용해 겹음 이미지들을 만들어줌
 - ...
 -------------------------------------------------------------------------------
 */

#ifndef msig_API_hpp
#define msig_API_hpp

#include <msig_common.hpp>
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace API {


// compound interval 이미지들을 만들어주는 함수
void
make_multiple_notes(std::string note_left_path,     // 왼쪽 머리 이미지 경로
                    std::string note_right_path,    // 오른쪽 머리 이미지 경로
                    std::string dataset_name,       // 생성할 데이터셋 이름
                    std::string image_name)         // 생성할 이미지 이름
{
    // 악상기호 이미지 준비
    MSIG::Algorithm::MusicalSymbol note_left(note_left_path, true, 128, 400);
    MSIG::Algorithm::MusicalSymbol note_right(note_right_path, true, 128, 400);
    //MSIG::Algorithm::MusicalSymbol staff("staff-note-compound/staff-b4-0-0.png", true, 128, 400);
    
    // 폴더 삭제
    std::filesystem::remove_all(dataset_name + "-up");
    std::filesystem::remove_all(dataset_name + "-down");
    std::filesystem::create_directory(dataset_name + "-up");
    std::filesystem::create_directory(dataset_name + "-down");
    
    // 랜덤 시드 설정
    std::srand(static_cast<unsigned int>(std::time(0)));
    
    // 음표 위치 조정 정보
    std::array<int, 11> pos = {
        35, 28, 21, 14, 7, 0, -7, -14, -21, -28, -35
    };
    
    // 악상기호들
    std::array<MSIG::Algorithm::MusicalSymbol, 11> noteLeft = {
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy(),
        note_left.copy()
    };
    std::array<MSIG::Algorithm::MusicalSymbol, 11> noteRight = {
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy(),
        note_right.copy()
    };
    for (size_t i=0; i<11; i++)
    {
        noteLeft[i].y += pos[i];
        noteRight[i].y += pos[i];
    }
    
    // config 파일 열기 생성
    std::fstream config_up((std::filesystem::path(dataset_name+"-up") / std::filesystem::path("config.txt")).string(), std::ios::out);
    if (!config_up) {
        std::cout << "config_up 파일을 열 수 없습니다." << std::endl;
        return;
    }
    std::fstream config_down((std::filesystem::path(dataset_name+"-down") / std::filesystem::path("config.txt")).string(), std::ios::out);
    if (!config_down) {
        std::cout << "config_down 파일을 열 수 없습니다." << std::endl;
        return;
    }
    
    // 조합 생성
    for (size_t iter=0; iter<100; iter++)
    {
        // 생성할 이미지 뽑기
        size_t i = (std::rand() % 0b11111111111) + 1;
        
        // 악상기호 조합 클래스
        MSIG::Algorithm::MusicalSymbolAssemble mss_up;
        MSIG::Algorithm::MusicalSymbolAssemble mss_down;
        
        //mss_up.push_back(staff);
        //mss_down.push_back(staff);
        
        // 스위칭 확인
        std::array<bool, 11> switched_up = {false};
        std::array<bool, 11> switched_down = {false};
        
        // 음표 머리 추가 : note-up
        for (size_t j=0; j<11; j++)
        {
            // 맨 처음인 경우
            if (j==0)
            {
                // 현재 악상기호 삽입인지
                if (i & (0b10000000000 >> j))
                {
                    // 다음 악상기호 삽입인지
                    if (i & (0b10000000000 >> (j+1))) {
                        mss_up.push_back(noteRight[j]);
                        switched_up[j] = true;
                    }
                    else {
                        mss_up.push_back(noteLeft[j]);
                        switched_up[j] = false;
                    }
                }
            }
            // 중간, 맨 마지막인 경우
            else
            {
                // 현재 악상기호 삽입인지
                if (i & (0b10000000000 >> j))
                {
                    // 이전 악상기호 삽입인지 && 이전 악상기호 바뀌지 않은 상태
                    if ((i & (0b10000000000 >> (j-1))) && switched_up[j-1]==false) {
                        mss_up.push_back(noteRight[j]);
                        switched_up[j] = true;
                    }
                    else {
                        mss_up.push_back(noteLeft[j]);
                        switched_up[j] = false;
                    }
                }
            }
        }
        
        // 음표 머리 추가 : note-down
        for (size_t j=0; j<11; j++)
        {
            // 맨 처음인 경우
            if (j==0)
            {
                // 현재 악상기호 삽입인지
                if (i & (0b10000000000 >> j))
                {
                    mss_down.push_back(noteRight[j]);
                    switched_up[j] = false;
                }
            }
            // 중간, 맨 마지막인 경우
            else
            {
                // 현재 악상기호 삽입인지
                if (i & (0b10000000000 >> j))
                {
                    // 이전 악상기호 삽입인지 && 이전 악상기호 바뀌지 않은 상태
                    if ((i & (0b10000000000 >> (j-1))) && switched_down[j-1]==false) {
                        mss_down.push_back(noteLeft[j]);
                        switched_down[j] = true;
                    }
                    else {
                        mss_down.push_back(noteRight[j]);
                        switched_down[j] = false;
                    }
                }
            }
        }
        
        // 음표 머리 위치에 따라 기둥과 꼬리 좌표 계산
        size_t start = -1;
        size_t end = -1;
        for (size_t j=0; j<11; j++)
        {
            if (i & (0b10000000000 >> j))
            {
                start = (start == -1) ? j : start;
                end = j;
            }
        }
        
        // 음표 머리가 음표 기둥 양 옆에 있는지 확인
        bool is_head_right = false;
        for (size_t j=0; j<11; j++)
            if (switched_up[j]==true) {
                is_head_right=true;
                break;
            }
        bool is_head_left = false;
        for (size_t j=0; j<11; j++)
            if (switched_down[j]==true) {
                is_head_left=true;
                break;
            }
        
        // 건너뛸지 말지 확률 뽑기 : 머리 갯수
        size_t count = 0;
        for (size_t j=0; j<11; j++) {
            if (i & (0b10000000000 >> j))
                count ++;
        }
        if (count > 5) {
            iter--;
            continue;
        }
        
        // 건너뛸지 말지 확률 뽑기 : 머리 좌우로 표시됨
        if (is_head_right || is_head_left)
        if (1 < (std::rand() / std::rand())) {
            iter--;
            continue;
        }
        
        
        // 미리보기
        //mss_up.show();
        //mss_down.show();
        
        // 이미지 생성
        auto image_up = mss_up.rendering(false, false, false, false);
        auto image_down = mss_down.rendering(false, false, false, false);
        
        // 기둥 그리기(랜덤)
        size_t note_length = std::rand() % 40;
        cv::Point up_pt1(64, 200 - pos[start] * 2 - 40 - (int)note_length);
        cv::Point up_pt2(64, 200 - pos[end] * 2 + 5);
        cv::Point down_pt1(64, 200 - pos[start] * 2 - 5);
        cv::Point down_pt2(64, 200 - pos[end] * 2 + 40 + (int)note_length);
        cv::line(image_up, up_pt1, up_pt2, cv::Scalar(0, 0, 0), 3);
        cv::line(image_down, down_pt1, down_pt2, cv::Scalar(0, 0, 0), 3);
        
        // 꼬리 그리기(랜덤)
        int note_beat = (std::rand() % 4) + 1;
        int note_deg = (std::rand() % 41) - 20;
        for (size_t j=0; j<note_beat; j++)
        {
            // 꼬리 그리기
            if (j==0)
            {
                int note_tail = (std::rand() % 3) - 1; // -1: 왼쪽만, 0: 양쪽, 1: 오른쪽만
                if      (note_tail>0) {
                    // note-up
                    cv::rectangle(image_up, up_pt1 + cv::Point(2, 5), up_pt1 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    // note-down
                    if (is_head_left) {
                        cv::rectangle(image_down, down_pt2 + cv::Point(2, 5), down_pt2 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    else {
                        cv::rectangle(image_down, down_pt2 + cv::Point(2, 5), down_pt2 + cv::Point(-38/2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                }
                else if (note_tail<0) {
                    // note-up
                    if (is_head_right) {
                        cv::rectangle(image_up, up_pt1 + cv::Point(38, 5), up_pt1 + cv::Point(-2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    else {
                        cv::rectangle(image_up, up_pt1 + cv::Point(38/2, 5), up_pt1 + cv::Point(-2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    // note-down
                    cv::rectangle(image_down, down_pt2 + cv::Point(38, 5), down_pt2 + cv::Point(-2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                }
                else {
                    // note-up
                    if (is_head_right) {
                        cv::rectangle(image_up, up_pt1 + cv::Point(38, 5), up_pt1 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    else{
                        cv::rectangle(image_up, up_pt1 + cv::Point(38/2, 5), up_pt1 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    // note-down
                    if (is_head_left) {
                        cv::rectangle(image_down, down_pt2 + cv::Point(38, 5), down_pt2 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                    else{
                        cv::rectangle(image_down, down_pt2 + cv::Point(38, 5), down_pt2 + cv::Point(-38/2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                    }
                }
            }
            else
            {
                // note-up
                if (is_head_right) {
                    cv::rectangle(image_up, up_pt1 + cv::Point(38, 5), up_pt1 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                }
                else{
                    cv::rectangle(image_up, up_pt1 + cv::Point(38/2, 5), up_pt1 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                }
                // note-down
                if (is_head_left) {
                    cv::rectangle(image_down, down_pt2 + cv::Point(38, 5), down_pt2 + cv::Point(-38, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                }
                else{
                    cv::rectangle(image_down, down_pt2 + cv::Point(38, 5), down_pt2 + cv::Point(-38/2, -5), cv::Scalar(0, 0, 0), cv::FILLED);
                }
            }
            
            if (j==note_beat-1)
                break;
            
            // 꼬리 연결 선 그리기
            cv::line(image_up, up_pt1, up_pt1 + cv::Point(0, -10), cv::Scalar(0, 0, 0), 3);
            cv::line(image_down, down_pt2, down_pt2 + cv::Point(0, 10), cv::Scalar(0, 0, 0), 3);
            up_pt1 = up_pt1 + cv::Point(0, -15);
            down_pt2 = down_pt2 + cv::Point(0, 15);
        }
        
        // 이미지, config 저장
        switch (note_beat)
        {
            case 1:
                cv::imwrite(dataset_name + "-up/" + image_name  + "-up-beam-08-" + std::bitset<11>(i).to_string() + ".png", image_up);
                config_up << image_name + "-up-beam-08-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                cv::imwrite(dataset_name + "-down/" + image_name + "-down-beam-08-" + std::bitset<11>(i).to_string() + ".png", image_down);
                config_down << image_name + "-down-beam-08-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                break;
            case 2:
                cv::imwrite(dataset_name + "-up/" + image_name  + "-up-beam-16-" + std::bitset<11>(i).to_string() + ".png", image_up);
                config_up << image_name + "-up-beam-16-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                cv::imwrite(dataset_name + "-down/" + image_name + "-down-beam-16-" + std::bitset<11>(i).to_string() + ".png", image_down);
                config_down << image_name + "-down-beam-16-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                break;
            case 3:
                cv::imwrite(dataset_name + "-up/" + image_name  + "-up-beam-32-" + std::bitset<11>(i).to_string() + ".png", image_up);
                config_up << image_name + "-up-beam-32-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                cv::imwrite(dataset_name + "-down/" + image_name + "-down-beam-32-" + std::bitset<11>(i).to_string() + ".png", image_down);
                config_down << image_name + "-down-beam-32-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                break;
            case 4:
                cv::imwrite(dataset_name + "-up/" + image_name  + "-up-beam-64-" + std::bitset<11>(i).to_string() + ".png", image_up);
                config_up << image_name + "-up-beam-64-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                cv::imwrite(dataset_name + "-down/" + image_name + "-down-beam-64-" + std::bitset<11>(i).to_string() + ".png", image_down);
                config_down << image_name + "-down-beam-64-" + std::bitset<11>(i).to_string() + ".png" + "=64~128~0.0~1.0~fixed\n";
                break;
        }
    }
    
    // config 파일 닫기
    config_up.close();
    config_down.close();
}


}
}

#endif /* msig_API_hpp */
