#include <msig.hpp>

namespace msig
{


cv::Mat Note::draw(){
    using namespace std;
    using namespace cv;
    
    // 흰 이미지 생성
    Mat img(img_h, img_w, CV_8UC1, Scalar(255));
    
    // draw_list에 있는 악상 기호들 img에 그리기
    for(auto key : draw_list){
        // 이미지와 config 불러오기
        Mat img_symbol = img_symbols[key.first].clone();    // 악상 기호 이미지
        string img_config = key.second;                     // 악상 기호 config
        
        // 악상 기호 그리기
        img = draw_symbols(img, img_symbol, img_config);
    }
    // 생성된 이미지 리턴
    return img;
}


// 악상 기호 그리기
cv::Mat Note::draw_symbols(const cv::Mat& img, const cv::Mat& img_symbol, std::string img_config, bool auxiliary_line){
    using namespace std;
    using namespace cv;
    
    // 편집할 이미지들
    Mat img1 = img.clone();         // 행렬 복사
    Mat img2 = img_symbol.clone();  // 행렬 복사
    
    // 편집 정보 추출
    vector<string> configs = split(img_config, "_"); // "x_y_각도_확대축소_대칭"
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    img2 = rotate_mat(img2, stod(configs[2]), img2.cols/2.0, img2.rows/2.0);
    img2 = scale_mat(img2, stod(configs[3]));
    img2 = symmetry_mat(img2, configs[4]);
    
    // 이미지 합성 좌표 계산
    int x = (img_w)/2.0 - (img2.cols/2.0) + (stoi(configs[0]));   // 이미지 중심 - sub 이미지 중심 + config 값
    int y = (img_h)/2.0 - (img2.rows/2.0) + (stoi(configs[1]));
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true){
        for (auto h : vector<int>({-pad*2, -pad, 0, pad, pad*2})){
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
