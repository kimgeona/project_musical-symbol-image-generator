
#include <msig_Rendering.hpp>

namespace MSIG {
namespace Rendering {

Canvas::Canvas(std::filesystem::path defaultDataset,
               bool brushing,
               int imageWidth, int imageHeight) : dst(defaultDataset)
{
    namespace fs = std::filesystem;
    
    // 1. 기존 데이터셋 경로 저장
    this->path = defaultDataset;
    
    // 2. 생성할 데이터셋 경로 생성
    this->newDatasetPath = fs::path("MSIG_" + defaultDataset.filename().string());
    
    // 3. DST 생성
    
    
    // 3. 기본 데이터셋 폴더의 모든 이미지 이름 구하기
    Algorithm::DependentSelectionTree dst(defaultDataset);
    dst.get_all_images_name(imageNames);
    
    // 4. 나머지 변수 값 저장
    this->brushing = brushing;
    if (imageWidth>0 && imageHeight>0) {
        this->imageWidth = imageWidth;
        this->imageHeight = imageHeight;
    }
    else throw std::runtime_error("MSIG::Rendering::Canvas::Canvas() : 이미지 크기는 0보다 크게 지정해야 합니다.");
    
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
Canvas::__remove_dataset()
{
    namespace fs = std::filesystem;
    
    if (fs::exists(this->newDatasetPath)) {
        std::cout << "  - 이미 생성되어 있는 데이터셋을 지웁니다." << std::endl;
        fs::remove_all(this->newDatasetPath);
    }
}

void
Canvas::__making_dataset(std::string datasetName,
                         MSIG::Algorithm::DependentSelectionTree& dst,
                         size_t imageCount,
                         int numThreads)
{
    namespace fs = std::filesystem;
    
    // 데이터셋 경로
    fs::path datasetPath = newDatasetPath/fs::path(datasetName);
    
    // 상위 경로 생성
    fs::create_directories(datasetPath);
    
    // CSV 파일 생성
    std::cout << "  - " << datasetName << " 데이터셋의 CSV 파일을 생성합니다." << std::endl;
    std::fstream csv = std::fstream((datasetPath/fs::path("label.csv")).string(), std::ios::out);
    if (!csv) {
        throw std::runtime_error("MSIG::Rendering::Canvas::__making_csv() : CSV 파일을 쓸 수 없습니다.");
    }
    
    // CSV 헤더 생성
    csv << "name";
    for (auto& s : this->imageNames)
    {
        csv << "," << s + "-x-1";
        csv << "," << s + "-y-1";
        csv << "," << s + "-x-2";
        csv << "," << s + "-y-2";
        csv << "," << s + "-cx";
        csv << "," << s + "-cy";
        csv << "," << s + "-probability";
    }
    csv << "\n";
    
    // 현재 컴퓨터의 CPU 갯수 구하기
    unsigned int numberOfCPU = 0;
    if (numThreads < 0) numberOfCPU = std::thread::hardware_concurrency();
    else                numberOfCPU = (unsigned int)numThreads;
    if (numberOfCPU < 1) {
        numberOfCPU = 1;
    }
    
    // 데이터셋 생성
    std::cout << "  - " << datasetName << " 데이터셋을 생성합니다." << std::endl;
    
    std::deque<std::vector<std::filesystem::path>> selectionList;
    size_t count = 0;
    
    // 이미지 조합 뽑기
    dst.pick(selectionList, imageCount);
    
    // 악상기호 이미지 생성 스레딩 시작
    threads.clear();
    for (size_t i=0; i<numberOfCPU; i++) {
        threads.emplace_back(&Canvas::__thread_function, this,
                             std::ref(datasetPath),
                             std::ref(csv),
                             std::ref(count),
                             std::ref(selectionList));
    }
    
    // 악상기호 이미지 생성 스레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }
    
    // CSV 파일 닫기
    csv.close();
}

void
Canvas::__thread_function(std::filesystem::path& imagePath,
                          std::fstream& csv,
                          size_t& count,
                          std::deque<std::vector<std::filesystem::path>>& selectionList)
{
    namespace fs = std::filesystem;
    
    // FIXME: 나중에 dynamic programming을 적용시켜서 더 빠르게 처리해 보자.
    while (true)
    {
        // 조합 목록 추출
        std::vector<std::filesystem::path> vp;
        {
            std::lock_guard<std::mutex> lock(mutex_dvp);
            
            // 남은 작업량 확인
            if (selectionList.empty())
                break;
            
            // 추출
            vp = selectionList.front();
            
            // 제거
            selectionList.pop_front();
        }
        
        // 악상기호 조합
        MSIG::Algorithm::MusicalSymbolAssemble msa;
        for (auto& p : vp) {
            msa.push_back(musicalSymbols.at(p));
        }
        
        // 악상기호 브러싱
        cv::Mat resultImage;
        cv::Mat resultImageYOLO;
        if (brushing) {
            // TODO: MSIG::Processing::Brush 완성하기
            // TODO: ms.rendering(true, false, false)로 변경 후 Brush 클래스의 함수들 통과시키기
            // TODO: Brush 클래스의 함수들을 통과시킨 수 최종적으로 이미지 크기 자르기
            resultImage = msa.rendering(false, false, false);
            resultImageYOLO = msa.rendering(false, true, true);
        }
        else {
            resultImage = msa.rendering(false, false, false);
            resultImageYOLO = msa.rendering(false, true, true);
        }
        
        // 레이블 생성
        std::map<std::filesystem::path, std::array<int, 6>> labels = msa.labels();
        
        // 저장
        {
            std::lock_guard<std::mutex> lock(mutex_csv);
            
            // 악상기호 이미지 저장
            cv::imwrite((imagePath/fs::path(std::to_string(count)+".png")).string(), resultImage);
            cv::imwrite((imagePath/fs::path(std::to_string(count)+"_yolo.png")).string(), resultImageYOLO);
            
            // 악상기호 CSV 데이터 저장
            csv << __labeling(std::to_string(count)+".png", labels) << "\n";
            
            // 카운트 증가
            count++;
        }
    }
}

std::string
Canvas::__labeling(std::string name,
                   std::map<std::filesystem::path, std::array<int, 6>>& labels)
{
    // imageNames를 순회하며 labels에 존재하면 추가
    for (const auto& imageName : imageNames)
    {
        // labels에서 imageName을 가진 key값 찾기
        auto it = std::find_if(labels.begin(), labels.end(), [&imageName](const auto& pair){
            return pair.first.stem().string()==imageName;
        });
        
        // 못찾은 경우
        if (it==labels.end())
        {
            name += ",0,0,0,0,0,0,0";
        }
        
        // 찾은 경우
        else
        {
            // x1, y1, x2, y2, cx, cy, probability
            for (size_t i=0; i<6; i++)
                name += "," + std::to_string(it->second[i]);
            name += ",1";
        }
    }
    return name;
}

void
Canvas::calculate()
{
    // 악상기호 조합 생성
    std::cout << "  * 악상기호 조합을 계산합니다.";
    dst.reconstruction();
    std::cout << " [총 " << static_cast<size_t>(dst) << "개 조합]" << std::endl;
}

void
Canvas::draw(size_t trainImageCount,
             size_t validationImageCount,
             size_t testImageCount,
             int numThreads)
{
    namespace fs = std::filesystem;
    
    // 기존 데이터셋 지우기
    __remove_dataset();
    
    // 데이터셋 생성
    __making_dataset("train", dst, trainImageCount, numThreads);
    __making_dataset("validation", dst, validationImageCount, numThreads);
    __making_dataset("test", dst, testImageCount, numThreads);
}

}
}
