#pragma once
#ifndef msig_Matrix_hpp
#define msig_Matrix_hpp

// c++17
#include <opencv2/opencv.hpp>   // OpenCV
#include <vector>
namespace msig {


// 행렬 연산
cv::Mat mat_attach      (const cv::Mat& img, const cv::Mat& img_sub, int x, int y); // 합성
cv::Mat mat_rotate      (const cv::Mat& img, double degree, int x, int y);          // 회전
cv::Mat mat_scale       (const cv::Mat& img, double scale);                         // 확대
cv::Mat mat_symmetry    (const cv::Mat& img, std::string symmetry);                 // 대칭

// 영상 처리
cv::Mat remove_padding(const cv::Mat& img);											// 흑백 영상 패딩 제거
cv::Mat remove_padding(const cv::Mat& img, int& center_x, int& center_y);			// 흑백 영상 패딩 제거


}

#endif /* msig_Matrix_hpp */
