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
    
    // 1. symbol_dataset_config.txt 열기(없으면 생성)
    // 2. line마다 작성되어있는 "이름=x_y_각도_배율" 불러오기
    // 3. Note::imgs_config에 key=string1, value=string2 형식으로 저장
    // 4. Note::imgs.key 값을 확인하여 없는 내용들에 대해선 "string1=string2" 생성 및 추가
    //     4-1. Note::imgs.key 값 불러오기
    //     4-2. string2 생성을 위한 Note::make_config()함수 실행
    //     4-3. 해당 함수 반환값으로 이름=x_y_각도_배율 생성하기
    //     4-1. 생성한 내용 symbol_dataset_config.txt 뒤에 추가, Note::imgs_config에 추가
    
    
}

std::string Note::make_config(string key){
    using namespace std;
    using namespace cv;
    
    // 1. 하얀 배경에 가운데 + 가 박혀져 있는 이미지 생성
    // 2. 해당 이미지 창에 띄움. 창 이름은 key로 설정
    // 3. config 정보를 생성하고자 하는 악상기호 이미지 띄우기.
    //     3-1. 방향키로 이미지 위치 조정
    //     3-2. 마우스 스크롤로 이미지 확대 및 축소
    //     3-3. 엔터시 저장 및 나가기, esc 입력시 나가기
}


}
