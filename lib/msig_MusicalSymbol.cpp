#include <msig_MusicalSymbol.hpp>

namespace msig
{


// msig_MusicalSymbol.cpp
MusicalSymbol::MusicalSymbol() {
    dir             = std::filesystem::path();
    dir_config      = std::filesystem::path();
    img             = cv::Mat();
    x               = 0;
    y               = 0;
    scale           = 0.0;
    rotate          = 0.0;
}
MusicalSymbol::MusicalSymbol(std::filesystem::path dir, std::filesystem::path dir_config) {
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 아래 과정에서 이상이 있을 경우
    if (init_dir(dir)   ||
        init_dir(dir)   ||
        init_img()      ||
        init_config())
    {
        cout << "MusicalSymbol:" << dir.filename().string() << " 을(를) 생성하는 과정에서 문제가 있습니다." << endl;
        MusicalSymbol();
    }
}
MusicalSymbol::MusicalSymbol(const MusicalSymbol& other) {
    dir             = other.dir;
    dir_config      = other.dir_config;
    img             = other.img.clone();
    x               = other.x;
    y               = other.y;
    scale           = other.scale;
    rotate          = other.rotate;
}


}
