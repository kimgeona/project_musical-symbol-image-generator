#include <msig_Canvas.hpp>

namespace msig
{


// 그리기
cv::Mat Canvas::draw(){
    using namespace std;
    using namespace cv;
    
    // 변수들
    int     img_w=256;  // 생성할 이미지 너비
    int     img_h=512;  // 생성할 이미지 높이
    
    // 흰 이미지 생성
    Mat img = Mat(img_h, img_w, CV_8UC1, Scalar(255));
    
    // 악상기호 배치 알고리즘(MSPA)
    MSPA mspa;
    
    // 그리기 목록(draw_list)에 있는 악상 기호들 배치
    for(auto& ms : draw_list)
    {
        // 악상기호 패딩 제거
        //remove_padding(ms.img, ms.x, ms.y);
        
        // 악상기호 종류에 따라 적절한 위치에 배치
        draw_adjustment(mspa, ms);
    }
    
    // 배치가 완료된 악상기호 이미지에 그리기
    img = draw_symbols(img, mspa.get());
    
    // 생성된 이미지 리턴
    return img;
}


// 그리기 위치 조정
void    Canvas::draw_locationing(MSPA& mspa, MusicalSymbol& ms){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 파일
    /*
     edge-@ measure-@       octave-@        : --
     octave-down-middle.png                 : bottom
     octave-up-middle.png                   : top
     
     line-@ note-down-@     octave-#        : --
     octave-down-end.png                    : bottom
     octave-down-middle.png                 : bottom
     octave-down-start-8.png                : bottom
     octave-down-start-15.png               : bottom
     octave-up-end.png                      : up
     octave-up-middle.png                   : up
     octave-up-start-8.png                  : up
     octave-up-start-15.png                 : up
     
     line-@ note-up-@       octave-#        : --
     octave-down-end.png                    : bottom
     octave-down-middle.png                 : bottom
     octave-down-start-8.png                : bottom
     octave-down-start-15.png               : bottom
     octave-up-end.png                      : up
     octave-up-middle.png                   : up
     octave-up-start-8.png                  : up
     octave-up-start-15.png                 : up
     */
    vector<string> f_fixed  = {};
    vector<string> f_top    = {
        path("octave-up-end.png"),
        path("octave-up-middle.png"),
        path("octave-up-start-8.png"),
        path("octave-up-start-15.png"),};
    vector<string> f_bottom = {
        path("octave-down-end.png"),
        path("octave-down-middle.png"),
        path("octave-down-start-8.png"),
        path("octave-down-start-15.png"),
    };
    vector<string> f_left   = {};
    vector<string> f_right  = {};
    
    // 폴더
    /*
     edge-@                                 : fixed
     edge-@ measure-@                       : fixed
     edge-@ measure-@       octave-@        : --
     edge-@ measure-@       repetition-@    : top
     line-@                                 : fixed
     line-@ clef-@                          : fixed
     line-@ key-@                           : fixed
     line-@ note-down-@                     : fixed
     line-@ note-down-@     accidental-#    : left
     line-@ note-down-@     articulation-#  : top
     line-@ note-down-@     dynamic-#       : bottom
     line-@ note-down-@     octave-#        : --
     line-@ note-down-@     ornament-#      : top
     line-@ note-down-@     repetition-#    : top
     line-@ note-up-@                       : fixed
     line-@ note-up-@       accidental-#    : left
     line-@ note-up-@       articulation-#  : bottom
     line-@ note-up-@       dynamic-#       : bottom
     line-@ note-up-@       octave-#        : --
     line-@ note-up-@       ornament-#      : top
     line-@ note-up-@       repetition-#    : top
     line-@ repetition-@                    : fixed
     line-@ rest-@                          : fixed
     line-@ time-@                          : fixed
     */
    vector<path> d_fixed    = {
        dir_ds/path("complete")/path("edge-@"),
        dir_ds/path("complete")/path("edge-@")/path("measure-@"),
        dir_ds/path("complete")/path("line-@"),
        dir_ds/path("complete")/path("line-@")/path("clef-@"),
        dir_ds/path("complete")/path("line-@")/path("key-@"),
        dir_ds/path("complete")/path("line-@")/path("note-down-@"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@"),
        dir_ds/path("complete")/path("line-@")/path("repetition-@"),
        dir_ds/path("complete")/path("line-@")/path("rest-@"),
        dir_ds/path("complete")/path("line-@")/path("time-@"),
    };
    vector<path> d_top      = {
        dir_ds/path("complete")/path("edge-@")/path("measure-@")/path("repetition-@"),
        dir_ds/path("complete")/path("line-@")/path("note-down-@")/path("articulation-#"),
        dir_ds/path("complete")/path("line-@")/path("note-down-@")/path("ornament-#"),
        dir_ds/path("complete")/path("line-@")/path("note-down-@")/path("repetition-#"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@")/path("ornament-#"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@")/path("repetition-#"),
    };
    vector<path> d_bottom   = {
        dir_ds/path("complete")/path("line-@")/path("note-down-@")/path("dynamic-#"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@")/path("articulation-#"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@")/path("dynamic-#"),
    };
    vector<path> d_left     = {
        dir_ds/path("complete")/path("line-@")/path("note-down-@")/path("accidental-#"),
        dir_ds/path("complete")/path("line-@")/path("note-up-@")/path("accidental-#"),
    };
    vector<path> d_right    = {};
    

    // 파일 이름 매칭
    if (find(f_fixed.begin(), f_fixed.end(), ms.dir.filename()) != f_fixed.end())
    {
        mspa.add(ms, "fixed", {});
        return;
    }
    if (find(f_top.begin(), f_top.end(), ms.dir.filename()) != f_top.end())
    {
        mspa.add(ms, "top", {});
        return;
    }
    if (find(f_bottom.begin(), f_bottom.end(), ms.dir.filename()) != f_bottom.end())
    {
        mspa.add(ms, "bottom", {});
        return;
    }
    if (find(f_left.begin(), f_left.end(), ms.dir.filename()) != f_left.end())
    {
        mspa.add(ms, "left", {"line-@", "edge-@"});
        return;
    }
    if (find(f_right.begin(), f_right.end(), ms.dir.filename()) != f_right.end())
    {
        mspa.add(ms, "right", {"line-@", "edge-@"});
        return;
    }
        
    
    // 폴더 이름 매칭
    if (find(d_fixed.begin(), d_fixed.end(), ms.dir.parent_path()) != d_fixed.end())
    {
        mspa.add(ms, "fixed", {});
        return;
    }
    if (find(d_top.begin(), d_top.end(), ms.dir.parent_path()) != d_top.end())
    {
        mspa.add(ms, "top", {});
        return;
    }
    if (find(d_bottom.begin(), d_bottom.end(), ms.dir.parent_path()) != d_bottom.end())
    {
        mspa.add(ms, "bottom", {});
        return;
    }
    if (find(d_left.begin(), d_left.end(), ms.dir.parent_path()) != d_left.end())
    {
        mspa.add(ms, "left", {"line-@", "edge-@"});
        return;
    }
    if (find(d_right.begin(), d_right.end(), ms.dir.parent_path()) != d_right.end())
    {
        mspa.add(ms, "right", {"line-@", "edge-@"});
        return;
    }
    
    // 매칭 되지 않은 악상기호들
    cout << "msig::Canvas::draw() : 악상기호 " << ms.dir.filename().string() << "에 대한 설정이 없어 기본(fixed)으로 진행합니다." << endl;
    mspa.add(ms, "fixed");
}


// 악상 기호 그리기
cv::Mat Canvas::draw_symbols(const cv::Mat& img, const msig::MusicalSymbol& ms, bool auxiliary_line){
    using namespace std;
    using namespace cv;
    
    // 편집할 이미지들
    Mat img1 = img.clone();         // 행렬 복사
    Mat img2 = ms.img.clone();      // 행렬 복사
    
    // 악상기호 이미지 중점
    int cx = ms.x;
    int cy = ms.y;
    
    // 이미지 편집(회전, 확대 및 축소, 대칭)
    //img2 = mat_rotate(img2, ms.rotate, cx, cy);
    img2 = mat_scale(img2, ms.scale, cx, cy);
    
    // 보조선 그리기 (오선지)
    if (auxiliary_line==true){
        for (auto h : vector<int>({-ms.pad*2, -ms.pad, 0, ms.pad, ms.pad*2})){
            int n = img1.rows/2.0;
            line(img1, Point(0,n+h), Point(img1.cols,n+h), Scalar(200), 2.2, LINE_AA);
        }
    }
    
    // 이미지 합성 좌표 계산
    int x = (img.cols)/2.0 - cx;   // 이미지 중심 - sub 이미지 중심 + config 값
    int y = (img.rows)/2.0 - cy;
    
    // 이미지 합성
    img1 = mat_attach(img1, img2, x, y);
    
    // 보조선 그리기 (십자선)
    if (auxiliary_line==true){
        Point cross(img1.cols/2.0, img1.rows/2.0);
        line(img1, cross+Point(-10,0), cross+Point(10,0), Scalar(100), 1, LINE_AA);
        line(img1, cross+Point(0,-10), cross+Point(0,10), Scalar(100), 1, LINE_AA);
    }
    
    return img1;
}


}
