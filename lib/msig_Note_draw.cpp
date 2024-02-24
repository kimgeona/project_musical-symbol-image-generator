#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 1. 1채널 이미지 생성
    Mat img(img_h, img_w, CV_8UC1, Scalar(255));
    
    // 2. draw_list 좌표 수정
    //     2-1. draw_list에 있는 내용들 상호관계 파악하여 "x_y_각도_확대축소_대칭_전체길이_현재위치" 수정
    //         2-1-1. 같이 표기할 수 없는 내용들 조사.
    //         2-1-2. 위치 우선순위 조사. (+ 슬러 나중에)
    //         2-1-3. x_y 값 위치 조정.
    
    // 3. draw_list에 있는 악상 기호들 img에 그리기
    for(auto key : draw_list){
        // 악상 기호 img, 악상 기호 config 구하기
        Mat img_symbol = img_symbols[key.first].clone();
        string img_config = key.second;
        
        // 악상 기호 그리기
        img = draw_symbols(img, img_symbol, img_config);
    }
    // 3. 생성된 이미지 리턴
    return img;
}


// 악상 기호 그리기
cv::Mat Note::draw_symbols(const cv::Mat& img, const cv::Mat& img_symbol, std::string img_config, bool auxiliary_line){
    using namespace std;
    using namespace cv;
    
    // 편집할 이미지들
    Mat img1 = img.clone();         // 행렬 복사
    Mat img2 = img_symbol.clone();  // 행렬 복사
    
    // 편집 정보 추출 : "x_y_각도_확대축소_대칭_전체길이_현재위치"
    vector<string> configs = split(img_config, "_");
    
    // 편집 진행(4개, 7개)
    if (configs.size() > 4){
        // ...나중에 작성
    }
    else{
        // 이미지 편집(회전, 확대 및 축소)
        img2 = rotate_mat(img2, stod(configs[2]), img2.cols/2.0, img2.rows/2.0); // 중심을 기준으로 degree 만큼 회전
        img2 = scale_mat(img2, stod(configs[3]));
    }
    
    // 이미지 합성 좌표 계산
    int x = (img_w)/2.0 - (img2.cols/2.0) + (stoi(configs[0]));   // 이미지 중심 - sub 이미지 중심 + config 값
    int y = (img_h)/2.0 - (img2.rows/2.0) + (stoi(configs[1]));
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true){
        for (auto h : vector<int>({-50, -25, 0, 25, 50})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
    }
    
    // 이미지 합성
    img1 = attach_mat(img1, img2, x, y);
    
    // 보조선 그리기 (십자선)
    if (auxiliary_line==true){
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
    }
    
    return img1;
}

}
