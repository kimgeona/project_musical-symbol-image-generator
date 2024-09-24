/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_Rendering.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개별 헤더 파일들 중 하나로, 악상기호 이미지를 만들어내는 클래스의 정의가 기술되어 있습
 니다.
 -------------------------------------------------------------------------------
 포함된 클래스:
 - Canvas() : 악상기호 이미지를 생성하는 클래스.
 -------------------------------------------------------------------------------
 */

#ifndef msig_Rendering_hpp
#define msig_Rendering_hpp

#include <msig_common.hpp>
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Rendering {
   

/*
 -------------------------------------------------------------------------------
 이름: Canvas()
 -------------------------------------------------------------------------------
 설명:
 악상기호 이미지를 생성하는 클래스.
 
 Canvas는 MSIG.Algorithm을 이용하여 악상기호 이미지 데이터셋을 생성하는 클래스입니다.
 -------------------------------------------------------------------------------
 주요 기능:
 - 악상기호 이미지 데이터셋 생성
 - 머신러닝 학습을 위한 이미지 레이블(CSV 파일) 생성
 - 생성할 데이터셋에 대한 설정(이미지 증강 여부, 이미지 크기, train-valid-test 구성 여부) 기능
 -------------------------------------------------------------------------------
 사용 방법:
 사용하기 위한 전체적인 순서는 다음과 같습니다.
 
 우선 캔버스를 데이터셋의 주소를 주어 생성합니다.
 MSIG::Rendering:Canvas canvas("기본 데이터셋 주소");
 
 생성할 데이터셋에 대한 설정을 진행합니다.
 canvas.set_data_augmentation(true);
 canvas.set_image_size(width, height);
 canvas.set_train_valid_test(10, 1, 3);
 
 데이터셋을 생성합니다.
 canvas.create_dataset();
 -------------------------------------------------------------------------------
 주의 사항:
 - Canvas는 자동으로 시스템에서 제공하는 코어수에 맞춰 스레드를 생성하여 데이터셋을 병렬로 생성합니다.
 -------------------------------------------------------------------------------
 */
class Canvas
{
public:
    std::filesystem::path path;
    std::filesystem::path newDatasetPath;
public:
    bool brushing;
    int  imageWidth;
    int  imageHeight;
public:
    Algorithm::DependentSelectionTree dstTrain;
    Algorithm::DependentSelectionTree dstValidation;
    Algorithm::DependentSelectionTree dstTest;
public:
    std::deque<std::vector<std::filesystem::path>> selectionListTrain;
    std::deque<std::vector<std::filesystem::path>> selectionListValidation;
    std::deque<std::vector<std::filesystem::path>> selectionListTest;
    std::vector<std::string>                       imageNames;
public:
    std::map<std::filesystem::path, Algorithm::MusicalSymbol> musicalSymbols;
private:
    std::vector<std::thread> threads;
    std::mutex mutex_dvp;
public:
    Canvas(std::filesystem::path defaultDataset, double trainRate, double validationRate, double testRate, bool brushing, int imageWidth=192, int imageHeight=512);
private:
    void        __making_csv(std::filesystem::path csvPath, const std::deque<std::vector<std::filesystem::path>>& selectionList);
    void        __making_image(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>> &selectionList, int numThreads);
    void        __thread_function(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>>& selectionList);
    std::string __labeling(std::string name, const std::vector<std::filesystem::path>& vp);
public:
    void        draw(int numThreads=-1);
};


}
}

#endif /* msig_Rendering_hpp */
