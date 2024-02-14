#include <iostream>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

namespace msig
{


void Note::load_imgs(){
    using namespace std;
    using namespace cv;
    using namespace std::__fs::filesystem;
    
    path d1(".");
    path d2(dataset_dir);
    
    for (int i=0; i<dirs.size(); i+=2){
        // 이미지 경로 생성
        path d3(dirs[i]), d4(dirs[i+1] + ".png");
        path img_dir = d1 / d2 / d3 / d4;
        
        // key 이름 생성
        string key_name = dirs[i] + "_" + dirs[i+1];
        
        // 이미지 불러오기
        Mat img = imread(img_dir, IMREAD_GRAYSCALE);
        if (img.data){
            imgs[key_name] = img.clone();
            img.release();
        }
    }
}

void Note::load_imgs_config(){
    using namespace std;
    using namespace cv;
    
    // 1. symbol_dataset_config.txt 열기
    // 2. imgs로 load된 이미지들의 config 불러오기. 만약 존재하지 않는다면 add_imgs_config(?) 함수를 호출하여 x_y_degree_scale 설정.
    // 3. ...?
}


}
