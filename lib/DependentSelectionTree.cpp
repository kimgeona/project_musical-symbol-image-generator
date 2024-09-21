
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Algorithm {

DependentSelectionTree::DependentSelectionTree(const std::filesystem::path& defaultDatasetDirectory, double declineRate) :
generator(std::random_device()()),
distribution(0.0, 1.0),
originaFolder(defaultDatasetDirectory, declineRate)
{
    // 1. 폴더 논리적 재구성 실행
    originaFolder.reconstruction(reconstructedFolders);
    
    // 2. 현재 스레드(메인) ID 저장
    mainThreadID = std::this_thread::get_id();
}

double
DependentSelectionTree::__generate_probability() {
    namespace fs = std::filesystem;
    
    // 1. 난수를 뽑아서 반환
    return distribution(generator);
}

void
DependentSelectionTree::pick_thread(std::deque<std::vector<std::filesystem::path>>& vvp, bool printStatus, bool randomPick, int numThreads) {
    /*
     NOTE: 현재 CPU 갯수만큼 쓰레딩 처리를 하였지만 조합 계산에 있어 유의미한 차이를 보이지 않는다.
           아마 조합 계산은 빨리 다 되었는데 이를 저장할 vvp 벡터가 하나밖에 없어서 그런게 아닌가 싶다.
           vvp 벡터를 여러개 만들어 실행해보려고 하였지만, 나중에 이들을 하나로 합쳐야 하는작업이 생기고
           해당 작업을 수행할 수 있는 메모리 공간의 여유가 없을 것 같아서 그냥 현재 단일 쓰레드로 처리해야겠다고 생각함.

     NOTE: 만약 멀티 쓰레드 처리를 하고자 한다면 thread_processing() 함수 안에 공유자원 잠금과
           관련한 주석들을 전부 해제하기 바람.
     */
    
    // 1. 현재 컴퓨터의 CPU 갯수 구하기
    unsigned int numberOfCPU = 0;
    if (numThreads < 0) numberOfCPU = std::thread::hardware_concurrency();
    else                numberOfCPU = (unsigned int)numThreads;
    if (numberOfCPU < 1) {
        numberOfCPU = 1;
    }
    
    // 2. 조합 상황 출력
    if (printStatus) {
        std::cout << std::endl << "  - 악상기호 조합을 생성합니다." << std::endl;
    }
    
    // 3. 쓰레딩 시작
    for (size_t i=0; i<numberOfCPU; i++) {
        threads.emplace_back(&DependentSelectionTree::pick, this, std::ref(vvp), printStatus, randomPick);
    }
    
    // 4. 쓰레딩 작업 기다리기
    for (size_t i=0; i<numberOfCPU; i++) {
        threads[i].join();
    }

   // 5. 조합 상황 출력
   if (printStatus) {
       std::cout << "  - 악상기호 조합 생성을 완료하였습니다. 총 " << vvp.size() << "개." << std::endl << std::endl;
   }
}

void
DependentSelectionTree::pick(std::deque<std::vector<std::filesystem::path>>& vvp, bool printStatus, bool randomPick) {
    // NOTE: 현재 그냥 단일 쓰레드로 처리할 예정이기에 공유자원 잠금 코드를 주석처리함.
    //       혹시 해당 작업을 멀티 쓰레딩으로 처리하고자 한다면, 공유자원 잠금 주석을 전부 지우길 바람.
    
    // *. 현재 스레드 ID 저장
    std::thread::id thisThreadID = std::this_thread::get_id();
    
    // 1. 조합 상황 출력
    if (printStatus && (this->mainThreadID == thisThreadID)) {
        std::cout << "  - 악상기호 조합을 생성합니다." << std::endl;
    }
    
    // 2. 스레딩 작업
    while(true)
    {
        // 공유자원 잠금
        if (mainThreadID != thisThreadID)
            mutex_vF.lock();
        
        // 남은 작업 확인
        if (reconstructedFolders.empty()) {
            if (mainThreadID != thisThreadID)
                mutex_vF.unlock();
            break;
        }
        
        // 남은 작업 불러오기
        Structure::Folder folder = reconstructedFolders.front();
        reconstructedFolders.pop_front();

        // 공유자원 잠금 해제
        if (mainThreadID != thisThreadID)
            mutex_vF.unlock();
        
        // 폴더 벡터 생성
        std::vector<Structure::Folder> folders = static_cast<std::vector<Structure::Folder>>(folder);
        
        // 가능한 이미지 조합 갯수 구하기
        size_t combinationCount = 1;
        for (auto& f : folders) {
            combinationCount = combinationCount * f.images.size();
        }
        
        // 생성한 이미지 갯수
        size_t createdCombinationCount = 0;
        
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
            if (mainThreadID != thisThreadID)
            {
                // 공유자원 잠금
                std::lock_guard<std::mutex> lock(mutex_vvp);
                
                // 생성된 이미지 조합 저장
                vvp.push_back(combination);
                
                // 생성한 이미지 갯수 증가
                createdCombinationCount++;
            }
            else
            {
                // 생성된 이미지 조합 저장
                vvp.push_back(combination);
                
                // 생성한 이미지 갯수 증가
                createdCombinationCount++;
            }
        }
        if (mainThreadID != thisThreadID)
        {
            // 공유자원 잠금
            std::lock_guard<std::mutex> lock(mutex_vF);
            
            // 조합 상황 출력
            if (printStatus) {
                std::cout << "  - 남은 경로 : " << reconstructedFolders.size() << ", 조합 생성 갯수 : (" << createdCombinationCount << "/" << combinationCount << ")" << (reconstructedFolders.size()==0 ? " - 스레드를 종료합니다." : "") << std::endl;
            }
        }
        else
        {
            // 조합 상황 출력
            if (printStatus) {
                std::cout << "  - 남은 경로 : " << reconstructedFolders.size() << ", 조합 생성 갯수 : (" << createdCombinationCount << "/" << combinationCount << ")" << std::endl;
            }
        }
    }
    
    // 3. 조합 상황 출력
    if (printStatus && (mainThreadID == thisThreadID)) {
        std::cout << "  - 악상기호 조합 생성을 완료하였습니다. 총 " << vvp.size() << "개." << std::endl << std::endl;
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
