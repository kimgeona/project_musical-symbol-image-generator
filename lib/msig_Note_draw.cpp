#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 1. 1채널 이미지 생성
    Mat img(height, width, CV_8UC1, Scalar(255));
    
    // 2. draw_list 좌표 수정
    //     2-1. draw_list에 있는 내용들 상호관계 파악하여 "x_y_각도_확대축소_대칭_전체길이_현재" 수정
    //         2-1-1. 같이 표기할 수 없는 내용들 조사.
    //         2-1-2. 위치 우선순위 조사. (+ 슬러 나중에)
    //         2-1-3. x_y 값 위치 조정.
    
    // 3. Mat이미지에 그리기
    
    // draw_list에 있는 악상 기호들 img에 그리기
    for(auto key : draw_list){
        // 악상 기호 img
        Mat img_symbol = imgs[key.first].clone();
        // 악상 기호 config
        string img_config = key.second;
        
        // config 값 뽑기
        vector<string> split;
        for (int p=0, n=(int)img_config.find("_", p);   // 초기식 : 단어 시작점과 끝점 찾기
             p<img_config.length();                     // 조건식 : 시작점이 문장의 끝이 아닌경우
             p=n+1, n=(int)img_config.find("_", p))     // 변화식 : 그다음 단어의 시작점과 끝점 찾기
        {
            if (n<0) n = (int)img_config.length();
            split.push_back(img_config.substr(p, n-p)); // 시작점과 끝점에 해당하는 단어 추가, 저장
        }
        int     config_x = stoi(split[0]);
        int     config_y = stoi(split[1]);
        double  config_degree = stod(split[2]);
        double  config_scale = stod(split[3]);
        string  config_mirror = split[4];
        string  config_length = split[5];
        string  config_pre = split[6];
        
        // 이미지 중심점 계산
        int cx = img_symbol.cols/2.0;
        int cy = img_symbol.rows/2.0;
        
        // 이미지 편집
        img_symbol = rotate_mat(img_symbol, config_degree, cx, cy); // 이미지 회전
        img_symbol = scale_mat(img_symbol, config_scale);           // 이미지 확대 및 축소
        
        // 이미지 편집에 의한 중심점 업데이트
        cx = img_symbol.cols/2.0;
        cy = img_symbol.rows/2.0;
        
        // 이미지 합치기
        img = combine_mat(img, (width/2)-cx+config_x, (height/2)-cy+config_y, img_symbol); // 이미지 합치기
    }
    // 3. 생성된 이미지 리턴
    return img;
}


}
