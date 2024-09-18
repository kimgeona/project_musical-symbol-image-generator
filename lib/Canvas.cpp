
#include <msig_Rendering.hpp>

namespace MSIG
{
namespace Rendering
{

Canvas::Canvas(std::filesystem::path defaultDataset, double trainRate, double validationRate, double testRate, bool brushing, int imageWidth, int imageHeight) :
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

void        Canvas::__making_csv(std::filesystem::path csvPath, const std::deque<std::vector<std::filesystem::path>>& selectionList)
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
void        Canvas::__making_image(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>>& selectionList)
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
std::string Canvas::__labeling(std::string name, const std::vector<std::filesystem::path>& vp)
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

void        Canvas::draw_thread(int numThreads)
{
    
}
void        Canvas::draw()
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

}
}
