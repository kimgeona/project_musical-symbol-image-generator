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

namespace MSIG
{
namespace Rendering
{
   

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
    Canvas(std::filesystem::path defaultDataset, double trainRate, double validationRate, double testRate, bool brushing, int imageWidth=192, int imageHeight=512) :
    dstTrain(defaultDataset, trainRate),
    dstValidation(defaultDataset, validationRate),
    dstTest(defaultDataset, testRate)
    {
        namespace fs = std::filesystem;
        
        // 1. 기존 데이터셋 경로 저장
        this->path = defaultDataset;
        
        // 2. 생성할 데이터셋 경로 생성
        this->newDatasetPath = fs::path("MSIG_" + MSIG_VERSION + "_" + defaultDataset.filename().string());
        
        // 3. 기본 데이터셋 폴더의 모든 이미지 이름 구하기
        dstTrain.get_all_images_name(imageNames);
        
        // 4. 나머지 변수 값 저장
        this->brushing = brushing;
        if (imageWidth>0 && imageHeight>0) {
            this->imageWidth = imageWidth;
            this->imageHeight = imageHeight;
        }
        else {
            
        }
    }
private:
    void        __making_csv(std::filesystem::path csvPath, const std::deque<std::vector<std::filesystem::path>>& selectionList)
    {
        namespace fs = std::filesystem;
        
        // 파일 열기
        std::fstream csv(csvPath.string(), std::ios::out);
        if (!csv)
        {
            throw std::runtime_error("MSIG::Rendering::Canvas::__making_csv() : CSV 파일을 쓸 수 없습니다.");
        }
        
        // csv 데이터 쓰기
        for (size_t i=0; i<selectionList.size(); i++)
        {
            csv << __labeling(std::to_string(i)+".png", selectionList[i]) << std::endl;
        }
        csv.close();
    }
    void        __making_image(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>>& selectionList)
    {
        namespace fs = std::filesystem;
        
        // TODO: 스레딩 기법 적용시키기
        // FIXME: 나중에 dynamic programming을 살려보자!
        
        // 현재 생성한 이미지 갯수
        size_t count = 0;
        
        // 이미지 생성 시작
        while (selectionList.size()>0)
        {
            // 조합 목록 추출
            std::vector<std::filesystem::path> vp = selectionList.front();
            selectionList.pop_front();
            
            // 악상기호 조합
            MSIG::Algorithm::MusicalSymbol ms(vp[0]);
            for (size_t i=1; i<vp.size(); i++) {
                ms = ms + MSIG::Algorithm::MusicalSymbol(vp[i], false, this->imageWidth, this->imageHeight);
            }
            
            // 악상기호 이미지 구하기
            cv::Mat resultImage;
            
            // 악상기호 브러싱
            if (brushing) {
                resultImage = ms.rendering(true, false, false);
                // TODO: 모델 과적합 방지를 위해 resultImage를 MSIG::Processing::Brush 클래스에서 제공하는 함수들에 한번씩 통과시키기. 그리고 마지막에 this->imageWidht, this->imageHeight 크기만큼 자르기.
            }
            else {
                resultImage = ms.rendering(false, false, false);
            }
            
            // 완성된 악상기호 저장
            cv::imwrite(imagePath.string()+std::to_string(count++)+".png", resultImage);
        }
    }
    std::string __labeling(std::string name, const std::vector<std::filesystem::path>& vp)
    {
        // 이미지 이름들 복사본
        std::vector<std::string> imageNamesCopy = this->imageNames;
        
        // 레이블 목록에서 일치하는 것 찾기
        for (const auto& p : vp)
        {
            // 레이블 목록(vs)에 조합(p)이 존재하는지 찾기
            auto iter = find(imageNamesCopy.begin(), imageNamesCopy.end(), p.filename().stem().string());
            
            // 찾았다면
            if (iter != imageNamesCopy.end()) *iter = "";
        }
        
        // 찾은 것들은 1로, 못찾은 것들은 0로 csv 데이터 생성
        for (auto& s : imageNamesCopy)
        {
            if (s == "")    name += ",1";  // 찾음
            else            name += ",0";  // 못찾음
        }
        
        // 생성된 csv 데이터 반환
        return name;
    }
public:
    void draw_thread(int numThreads=-1)
    {
        
    }
    void draw()
    {
        namespace fs = std::filesystem;
        
        // *. 기존 데이터셋 지우기
        if (!fs::exists(this->newDatasetPath))
        {
            std::cout << "  - 존재하는 데이터셋 폴더 \"" << this->newDatasetPath << "\"를 지웁니다." << std::endl;
            fs::remove(this->newDatasetPath);
        }
        
        // 1. train 데이터셋
        dstTrain.pick(selectionListTrain);                                                          // 악상기호 조합 생성
        __making_csv(newDatasetPath/fs::path("train")/fs::path("label.csv"), selectionListTrain);   // 악상기호 데이터셋 레이블 생성
        __making_image(newDatasetPath/fs::path("train"), selectionListTrain);                       // 악상기호 이미지 생성 시작
        
        // 2. validation 데이터셋
        dstValidation.pick(selectionListValidation);
        __making_csv(newDatasetPath/fs::path("validation")/fs::path("label.csv"), selectionListValidation);
        __making_image(newDatasetPath/fs::path("validation"), selectionListValidation);
        
        // 3. test 데이터셋
        dstValidation.pick(selectionListTest);
        __making_csv(newDatasetPath/fs::path("test")/fs::path("label.csv"), selectionListTest);
        __making_image(newDatasetPath/fs::path("test"), selectionListTest);
    }
};


}
}

#endif /* msig_Rendering_hpp */
