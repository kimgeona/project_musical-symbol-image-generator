
#include <msig_Rendering.hpp>

namespace MSIG {
namespace Rendering {

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
    
    // 5. 현재 스레드(메인) ID 저장
    mainThreadID = std::this_thread::get_id();
}

void
Canvas::__making_csv(std::filesystem::path csvPath, const std::deque<std::vector<std::filesystem::path>>& selectionList) {
    namespace fs = std::filesystem;
    
    // 파일 열기
    if (!fs::exists(csvPath.parent_path())) {
        fs::create_directories(csvPath.parent_path());
    }
    std::fstream csv(csvPath.string(), std::ios::out);
    if (!csv)
    {
        throw std::runtime_error("MSIG::Rendering::Canvas::__making_csv() : CSV 파일을 쓸 수 없습니다.");
    }
    
    // csv 헤더 생성
    csv << "name";
    for (auto& s : this->imageNames)
        csv << "," << s;
    csv << std::endl;
    
    // csv 데이터 쓰기
    for (size_t i=0; i<selectionList.size(); i++)
    {
        csv << __labeling(std::to_string(i)+".png", selectionList[i]) << std::endl;
    }
    csv.close();
}

void
Canvas::__making_image_thread(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>> &selectionList, int numThreads) {
    namespace fs = std::filesystem;
    
    // 1. 현재 컴퓨터의 CPU 갯수 구하기
    unsigned int numberOfCPU = 0;
    if (numThreads < 0) numberOfCPU = std::thread::hardware_concurrency();
    else                numberOfCPU = (unsigned int)numThreads;
    if (numberOfCPU < 1) {
        numberOfCPU = 1;
    }
    
    // 2. 쓰레딩 시작
    for (size_t i=0; i<numberOfCPU; i++) {
        threads.emplace_back(&Canvas::__making_image, this, imagePath, std::ref(selectionList));
    }
    
    // 3. 쓰레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }
}

void
Canvas::__making_image(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>>& selectionList) {
    namespace fs = std::filesystem;
    
    // FIXME: 나중에 dynamic programming을 적용시켜서 더 빠르게 처리해 보자.
    
    // *. 현재까지 생성한 이미지 갯수
    static size_t count = 0;
    
    // *. 현재 스레드 ID 저장
    std::thread::id thisThreadID = std::this_thread::get_id();
    
    // 이미지 생성 시작
    while (true)
    {
        // 공유자원 잠금
        if (mainThreadID != thisThreadID)
            mutex_dvp.lock();
        
        // 남은 작업량 확인
        if (selectionList.empty()) {
            mutex_dvp.unlock();
            break;
        }
        
        // 조합 목록 추출
        std::vector<std::filesystem::path> vp = selectionList.front();
        selectionList.pop_front();
        
        // 카운트 증가 및 저장
        size_t thisCount = count++;
        
        // 공유자원 잠금 해제
        if (mainThreadID != thisThreadID)
            mutex_dvp.unlock();
        
        // 악상기호 조합
        MSIG::Algorithm::MusicalSymbol ms(vp[0], false);
        for (size_t i=1; i<vp.size(); i++) {
            ms = ms + MSIG::Algorithm::MusicalSymbol(vp[i], false, this->imageWidth, this->imageHeight);
        }
        
        // 악상기호 이미지 구하기
        cv::Mat resultImage;
        
        // 악상기호 브러싱
        if (brushing) {
            // TODO: MSIG::Processing::Brush 작성이 끝나면 ms.rendering(true, false, false) 로 바꾸기
            resultImage = ms.rendering(false, false, false);
            // TODO: MSIG::Processing::Brush 작성이 끝나면 작성한 함수들에 resultImage를 한번씩 통과시키기. 그리고 마지막에 this->imageWidht, this->imageHeight 크기만큼 자르기.
        }
        else {
            resultImage = ms.rendering(false, false, false);
        }
        
        // 완성된 악상기호 저장
        cv::imwrite(imagePath.string()+std::to_string(thisCount)+".png", resultImage);
    }
}

std::string
Canvas::__labeling(std::string name, const std::vector<std::filesystem::path>& vp) {
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

void
Canvas::pick_thread(int numThreads) {
    namespace fs = std::filesystem;
    
    // 1. train 데이터셋
    dstTrain.pick_thread(selectionListTrain, false, true, numThreads);
    
    // 2. validation 데이터셋
    dstValidation.pick_thread(selectionListValidation, false, true, numThreads);
    
    // 3. test 데이터셋
    dstTest.pick_thread(selectionListTest, false, true, numThreads);
}

void
Canvas::pick() {
    namespace fs = std::filesystem;
    
    // 1. train 데이터셋
    dstTrain.pick(selectionListTrain, false, true);
    
    // 2. validation 데이터셋
    dstValidation.pick(selectionListValidation, false, true);
    
    // 3. test 데이터셋
    dstTest.pick(selectionListTest, false, true);
}

void
Canvas::draw_thread(int numThreads) {
    namespace fs = std::filesystem;
    
    // *. 기존 데이터셋 지우기
    if (fs::exists(this->newDatasetPath))
    {
        std::cout << "  - 존재하는 데이터셋 폴더 \"" << this->newDatasetPath.string() << "\"를 지웁니다." << std::endl;
        fs::remove_all(this->newDatasetPath);
    }
    
    // 1. train 데이터셋
    __making_csv(newDatasetPath/fs::path("train")/fs::path("label.csv"), selectionListTrain);   // 악상기호 데이터셋 레이블 생성
    __making_image_thread(newDatasetPath/fs::path("train"), selectionListTrain);                // 악상기호 이미지 생성 시작
    
    // 2. validation 데이터셋
    __making_csv(newDatasetPath/fs::path("validation")/fs::path("label.csv"), selectionListValidation);
    __making_image_thread(newDatasetPath/fs::path("validation"), selectionListValidation);
    
    // 3. test 데이터셋
    __making_csv(newDatasetPath/fs::path("test")/fs::path("label.csv"), selectionListTest);
    __making_image_thread(newDatasetPath/fs::path("test"), selectionListTest);
}

void
Canvas::draw() {
    namespace fs = std::filesystem;
    
    // *. 기존 데이터셋 지우기
    if (fs::exists(this->newDatasetPath))
    {
        std::cout << "  - 존재하는 데이터셋 폴더 \"" << this->newDatasetPath.string() << "\"를 지웁니다." << std::endl;
        fs::remove_all(this->newDatasetPath);
    }
    
    // 1. train 데이터셋
    __making_csv(newDatasetPath/fs::path("train")/fs::path("label.csv"), selectionListTrain);   // 악상기호 데이터셋 레이블 생성
    __making_image(newDatasetPath/fs::path("train"), selectionListTrain);                       // 악상기호 이미지 생성 시작
    
    // 2. validation 데이터셋
    __making_csv(newDatasetPath/fs::path("validation")/fs::path("label.csv"), selectionListValidation);
    __making_image(newDatasetPath/fs::path("validation"), selectionListValidation);
    
    // 3. test 데이터셋
    __making_csv(newDatasetPath/fs::path("test")/fs::path("label.csv"), selectionListTest);
    __making_image(newDatasetPath/fs::path("test"), selectionListTest);
}

}
}
