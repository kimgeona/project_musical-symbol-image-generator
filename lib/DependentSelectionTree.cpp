
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Algorithm {

DependentSelectionTree::DependentSelectionTree(const std::filesystem::path& defaultDatasetDirectory)
{
    // original 폴더 생성
    originaFolder = Structure::Folder(defaultDatasetDirectory);
}

DependentSelectionTree::operator size_t() const
{
    // original 폴더 재구성 여부 확인
    if (reconstructedFolders.folders.empty()) {
        throw std::runtime_error("MSIG::Algorithm::DependentSelectionTree -> size_t : 먼저 reconstruction()을 진행해야합니다.");
    }
    
    // 생성가능한 모든 조합 갯수 계산해서 반환
    size_t imageCount = 0;
    for (auto& F : reconstructedFolders.folders)
        imageCount += static_cast<size_t>(F);
    return imageCount;
}

void
DependentSelectionTree::__thread_function(std::deque<std::vector<std::filesystem::path>>& vvp, size_t& numImages)
{
    while (true)
    {
        // 공유자원 잠금
        mutex_t.lock();
        
        // 남은 작업 확인
        if (numImages==0) {
            mutex_t.unlock();
            return;
        }
        
        // 생성해야되는 이미지 갯수 감소
        numImages--;
        
        // 공유자원 잠금 해제
        mutex_t.unlock();
        
        // 이미지 조합 뽑기
        std::vector<std::filesystem::path> vp;
        reconstructedFolders.pick(vp);
        
        // 공유자원 잠금
        mutex_vvp.lock();
        
        // 이미지 조합 저장
        vvp.push_back(vp);
        
        // 공유자원 잠금해제
        mutex_vvp.unlock();
    }
}

void
DependentSelectionTree::reconstruction()
{
    // 폴더 재구성 실행
    reconstructedFolders = originaFolder.reconstruction();
}

void
DependentSelectionTree::pick(std::deque<std::vector<std::filesystem::path>>& vvp, size_t numImages, int numThreads)
{
    namespace fs = std::filesystem;
    
    // 1. 현재 컴퓨터의 CPU 갯수 구하기
    unsigned int numberOfCPU = 0;
    if (numThreads < 0) numberOfCPU = std::thread::hardware_concurrency();
    else                numberOfCPU = (unsigned int)numThreads;
    if (numberOfCPU < 1) {
        numberOfCPU = 1;
    }
    
    // 2. 조합 벡터 초기화
    vvp = std::deque<std::vector<std::filesystem::path>>();
    
    // 3. 쓰레딩 시작
    threads.clear();
    for (size_t i=0; i<numberOfCPU; i++) {
        threads.emplace_back(&DependentSelectionTree::__thread_function, this, std::ref(vvp), std::ref(numImages));
    }
    
    // 4. 쓰레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }
}

void
DependentSelectionTree::get_all_images_name(std::vector<std::string>& imagesNames)
{
    // 모든 이미지 이름 구하기
    std::set<std::string> allImagesNames;
    originaFolder.get_all_images_name(allImagesNames);
    
    // 벡터로 변환
    imagesNames = std::vector<std::string>(allImagesNames.begin(), allImagesNames.end());
    
    // 벡터 정렬
    std::sort(imagesNames.begin(), imagesNames.end());
}

}
}
