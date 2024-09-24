
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
        throw std::runtime_error("MSIG::Rendering::Canvas::Canvas() : 이미지 크기는 0보다 크게 지정해야 합니다.");
    }
    
    // 5. MuscialSymbols 전부 로드하기
    for (auto& p : fs::recursive_directory_iterator(defaultDataset)) {
        if (fs::exists(p.path())                  &&
            fs::is_regular_file(p.path())         &&
            p.path().extension().string()==".png")
        {
            musicalSymbols.emplace(p.path(), Algorithm::MusicalSymbol(p.path(), true, this->imageWidth, this->imageHeight));
        }
    }
}

void
Canvas::__making_csv(std::filesystem::path csvPath, const std::deque<std::vector<std::filesystem::path>>& selectionList) {
    namespace fs = std::filesystem;
    
    // static 변수
    static size_t i = 0;
    static std::string csvPathString = "";
    
    //
    if (i==0)
    {
        csvPathString = csvPath.string();
    }
    else if (csvPathString != csvPath.string())
    {
        i = 0;
        csvPathString = csvPath.string();
    }
    
    // 파일 열기
    std::fstream csv;
    if (i==0)
    {
        // 폴더 생성
        if (!fs::exists(csvPath.parent_path())) {
            fs::create_directories(csvPath.parent_path());
        }
        // 파일 생성하고 쓰기
        csv = std::fstream(csvPath.string(), std::ios::out);
    }
    else
    {
        // 파일 맨 뒤에 쓰기
        csv = std::fstream(csvPath.string(), std::ios::out|std::ios::app);
    }
    if (!csv)
    {
        throw std::runtime_error("MSIG::Rendering::Canvas::__making_csv() : CSV 파일을 쓸 수 없습니다.");
    }
    
    
    // csv 헤더 생성
    if (i==0)
    {
        csv << "name";
        // TODO: YOLO 형식으로 csv 헤더 다시 작성해야함
        for (auto& s : this->imageNames)
            csv << "," << s;
        csv << std::endl;
    }
    
    // csv 데이터 쓰기
    for (size_t pre=i+selectionList.size(); i<pre; i++)
    {
        csv << __labeling(std::to_string(i)+".png", selectionList[i]) << std::endl;
    }
    csv.close();
}

void
Canvas::__making_image(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>> &selectionList, int numThreads) {
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
        threads.emplace_back(&Canvas::__thread_function, this, imagePath, std::ref(selectionList));
    }
    
    // 3. 쓰레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }
}

void
Canvas::__thread_function(std::filesystem::path imagePath, std::deque<std::vector<std::filesystem::path>>& selectionList) {
    namespace fs = std::filesystem;
    
    // FIXME: 나중에 dynamic programming을 적용시켜서 더 빠르게 처리해 보자.
    
    // *. 현재까지 생성한 이미지 갯수
    static size_t count = 0;
    
    // 이미지 생성 시작
    while (true)
    {
        // 공유자원 잠금
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
        mutex_dvp.unlock();
        
        // 악상기호 조합
        MSIG::Algorithm::MusicalSymbol ms = musicalSymbols.at(vp[0]);
        for (size_t i=1; i<vp.size(); i++) {
            ms = ms + musicalSymbols.at(vp[i]);
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
    // TODO: YOLO 형식으로 코드 다시 작성해야함
    
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
Canvas::draw(int numThreads) {
    namespace fs = std::filesystem;
    
    // *. 기존 데이터셋 지우기
    if (fs::exists(this->newDatasetPath))
    {
        std::cout << "  - 존재하는 데이터셋 폴더 \"" << this->newDatasetPath.string() << "\"를 지웁니다." << std::endl;
        fs::remove_all(this->newDatasetPath);
    }
    
    // Train
    std::cout << "  - Train 데이터셋을 생성합니다." << std::endl;
    dstTrain.reconstruction();
    while (true) {
        // 악상기호 조합 뽑기
        dstTrain.pick(selectionListTrain, true);
        if (selectionListTrain.empty()) {
            break;
        }
        // 악상기호 데이터셋 생성
        __making_csv(newDatasetPath/fs::path("train")/fs::path("label.csv"), selectionListTrain);   // 악상기호 데이터셋 레이블 생성
        __making_image(newDatasetPath/fs::path("train"), selectionListTrain, numThreads);           // 악상기호 이미지 생성 시작
    }
    
    // Validation
    std::cout << "  - Validation 데이터셋을 생성합니다." << std::endl;
    dstValidation.reconstruction();
    while (true) {
        // 악상기호 조합 뽑기
        dstValidation.pick(selectionListValidation, true);
        if (selectionListValidation.empty()) {
            break;
        }
        // 악상기호 데이터셋 생성
        __making_csv(newDatasetPath/fs::path("validation")/fs::path("label.csv"), selectionListValidation);
        __making_image(newDatasetPath/fs::path("validation"), selectionListValidation, numThreads);
    }
    
    // Test
    std::cout << "  - Test 데이터셋을 생성합니다." << std::endl;
    dstTest.reconstruction();
    while (true) {
        // 악상기호 조합 뽑기
        dstTest.pick(selectionListTest, true);
        if (selectionListTest.empty()) {
            break;
        }
        // 악상기호 데이터셋 생성
        __making_csv(newDatasetPath/fs::path("test")/fs::path("label.csv"), selectionListTest);
        __making_image(newDatasetPath/fs::path("test"), selectionListTest, numThreads);
    }
}

}
}
