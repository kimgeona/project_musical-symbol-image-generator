
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Algorithm {

DependentSelectionTree::DependentSelectionTree(const std::filesystem::path& defaultDatasetDirectory, double declineRate) :
generator(std::random_device()()),
distribution(0.0, 1.0),
originaFolder(defaultDatasetDirectory, declineRate)
{
    // 수행할 작업 없음.
}

double
DependentSelectionTree::__generate_probability() {
    namespace fs = std::filesystem;
    
    // 1. 난수를 뽑아서 반환
    return distribution(generator);
}

void
DependentSelectionTree::__thread_function(std::deque<std::vector<std::filesystem::path>>& vvp, bool randomPick) {
    while (true)
    {
        // 공유자원 잠금
        mutex_vF.lock();
        
        // 남은 작업 확인
        if (reconstructedFolders.empty()) {
            mutex_vF.unlock();
            return;
        }
        
        // 남은 작업 불러오기
        Structure::Folder folder = reconstructedFolders.front().peek();
        
        // 더이상 추출할 폴더가 존재하지 않으면
        if (!reconstructedFolders.front().pop())
            reconstructedFolders.pop_front();
        
        // 공유자원 잠금 해제
        mutex_vF.unlock();
        
        // 폴더 벡터 생성
        std::vector<Structure::Folder> folders = static_cast<std::vector<Structure::Folder>>(folder);
        
        // 가능한 이미지 조합 갯수 구하기
        size_t combinationCount = 1;
        for (auto& f : folders) {
            combinationCount = combinationCount * f.images.size();
        }
        
        // 이미지 조합 생성
        for (size_t i=0; i<combinationCount; i++)
        {
            std::vector<std::filesystem::path> combination; // 이미지 조합 벡터
            size_t discount = 1;                            // 폴더 선택 제어 변수
            bool skip = false;                              // 건너뛰기 확률
            for (size_t j=0; j<folders.size(); j++)
            {
                //
                if (j>0) {
                    discount = discount * folders[j-1].images.size();
                }
                // 이미지 생성 확률 체크
                if (randomPick && (folders[j].images[(i/discount) % folders[j].images.size()].selectionProbability < __generate_probability())) {
                    skip = true;
                    break;
                }
                // 이미지 선택
                combination.push_back(folders[j].images[(i/discount) % folders[j].images.size()].path);
            }
            // 이미지 생성 확률에 의한 건너뛰기
            if (skip && randomPick) {
                continue;
            }
            
            // 공유자원 잠금
            mutex_vF.lock();
            
            // 생성된 이미지 조합 저장
            vvp.push_back(combination);
            
            // 공유자원 잠금 해제
            mutex_vF.unlock();
        }
        
        // 공유자원 잠금
        mutex_vF.lock();
        
        // vvp가 randomPick에 의해 비어있으면 남은 작업 다시 불러와서 뽑기 진행.
        if (!vvp.empty()) {
            mutex_vF.unlock();
            break;
        }
        
        // 공유자원 잠금 해제
        mutex_vF.unlock();
    }
}

void
DependentSelectionTree::reconstruction() {
    namespace fs = std::filesystem;
    
    // 1. 폴더 논리적 재구성 실행
    originaFolder.reconstruction(reconstructedFolders);
}

void
DependentSelectionTree::pick(std::deque<std::vector<std::filesystem::path>>& vvp, bool randomPick, int numThreads) {
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
    for (size_t i=0; i<numberOfCPU; i++) {
        threads.emplace_back(&DependentSelectionTree::__thread_function, this, std::ref(vvp), randomPick);
    }
    
    // 4. 쓰레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }
}

void
DependentSelectionTree::get_all_images_name(std::vector<std::string>& imagesNames) {
    // 모든 이미지 이름 구하기
    std::set<std::string> allImagesNames;
    originaFolder.get_all_images_name(allImagesNames);
    
    // 벡터로 변환 및 정렬
    imagesNames = std::vector<std::string>(allImagesNames.begin(), allImagesNames.end());
    std::sort(imagesNames.begin(), imagesNames.end());
}

}
}
