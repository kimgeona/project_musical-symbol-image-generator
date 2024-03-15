#include <msig_MusicalSymbol.hpp>

namespace msig
{

// msig_MusicalSymbol_Preprocessing.cpp
void                        MusicalSymbol::restore_img(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 존재 여부 확인
    if (!exists(path(dir))) return;
    
    // 이미지 읽기
    Mat img = imread(dir, IMREAD_GRAYSCALE);
    CV_Assert(img.data);
    
    // 이미지 저장
    imwrite(dir, img);
}
void                        MusicalSymbol::remove_padding(std::filesystem::path dir){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 이미지 존재 여부 확인
    if (!exists(dir)) return;
    
    // 이미지 읽기
    Mat img = imread(dir.string(), IMREAD_GRAYSCALE);
    CV_Assert(img.data);

    // 흰색 픽셀을 0으로, 다른 색상의 픽셀을 1로 설정
    Mat non_white_img = img < 255;

    // 행과 열의 범위를 찾기 위한 변수
    int row_start = -1, row_end = -1, col_start = -1, col_end = -1;

    // 행의 범위를 찾기
    for (int i = 0; i < non_white_img.rows; i++) {
        if (row_start < 0 && countNonZero(non_white_img.row(i)) > 0) {
            row_start = i;
        }
        if (row_start >= 0 && countNonZero(non_white_img.row(i)) > 0) {
            row_end = i;
        }
    }

    // 열의 범위를 찾기
    for (int i = 0; i < non_white_img.cols; i++) {
        if (col_start < 0 && countNonZero(non_white_img.col(i)) > 0) {
            col_start = i;
        }
        if (col_start >= 0 && countNonZero(non_white_img.col(i)) > 0) {
            col_end = i;
        }
    }


    // 범위를 제한하여 이미지를 잘라냄
    Rect roi(col_start, row_start, col_end - col_start, row_end - row_start);
    Mat new_img = img(roi).clone();

    // 이미지 저장
    imwrite(dir.string(), new_img);
}


}
