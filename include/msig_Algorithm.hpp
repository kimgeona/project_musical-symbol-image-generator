/*
 -------------------------------------------------------------------------------
 프로젝트: MSIG(Musical-Symbol-Image-Generator)
 파일이름: msig_Algorithm.hpp
 -------------------------------------------------------------------------------
 설명:
 MSIG 라이브러리의 개별 헤더 파일들 중 하나로, MSIG 라이브러리에서 사용되는 핵심적인 자료구조와 알고리즘
 들에 대한 클래스 정의가 기술되어 있습니다.
 -------------------------------------------------------------------------------
 클래스:
 - Image()                  : 악상기호 이미지의 정보를 저장하는 클래스.
 - Folder()                 : 악상기호 이미지들을 저장하는 폴더 클래스.
 - DependentSelectionTree() : 악상기호 조합을 생성해주는 클래스.
 - MusicalSymbol()          : 악상기호를 보관하는 클래스.
 -------------------------------------------------------------------------------
 */

#ifndef msig_Algorithm_hpp
#define msig_Algorithm_hpp

// c++17
#include <iostream>
#include <filesystem>
#include <deque>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <mutex>
//
#include <msig_Structure.hpp>


namespace MSIG
{
namespace Algorithm
{


/*
 -------------------------------------------------------------------------------
 이름: DependentSelectionTree()
 -------------------------------------------------------------------------------
 설명:
 악상기호 조합을 생성해주는 클래스.
 
 DependentSelectionTree는 기본 데이터셋(사용자 정의 데이터셋)의 "악상기호 분류체계"를 분석하여 해당
 데이터셋에서 가능한 모든 악상기호의 조합을 생성하여 제공하는 역할을 합니다.
 -------------------------------------------------------------------------------
 주요 기능:
 - 기본 데이터셋 무결성(__rule__.txt) 검사
 - 기본 데이터셋 악상기호 이미지 좌표 정보(__config__.txt) 생성 및 수정
 - 기본 데이터셋을 트리 구조의 논리적인 형태로 변환하여 저장
 - 모든 악상기호 조합 계산
 -------------------------------------------------------------------------------
 사용 방법:
 사용하기 위한 전체적인 순서는 다음과 같습니다.
 
 우선 기본 데이터셋을 클래스 생성자를 통해 불러와주고,
 MSIG::Algorithm::DependentSelectionTree dst("기본 데이터셋 주소");
 
 악상기호 조합을 생성하기전 해당 데이터셋이 "악상기호 분류체계"에 맞게 구조화 되어있는지를 확인합니다.
 dst.check_rule();
 
 그리고 각 악상기호 이미지들의 좌표 정보를 생성하거나 수정한 후
 dst.make_config();
 dst.modify_config();
 
 마지막으로 가능한 모든 악상기호의 조합을 계산합니다.
 audo qvp = dst.list();
 -------------------------------------------------------------------------------
 주의 사항:
 - 클래스로 생성된 객체가 기본 데이터셋과 연결되어 있을때는 해당 데이터셋 파일이 다른 프로그램이나 사용자에
 의해 수정되면 안된다.
 -------------------------------------------------------------------------------
 */

class DependentSelectionTree
{
private:
    Structure::Folder              originaFolder;
    std::deque<Structure::Folder>  reconstructedFolders;
private:
    std::mt19937                           generator;
    std::uniform_real_distribution<double> distribution;
private:
    std::thread::id mainThreadID;
    std::vector<std::thread> threads;
    std::mutex mutex_vF;
    std::mutex mutex_vvp;
public:
    DependentSelectionTree(const std::filesystem::path& defaultDatasetDirectory, double selectionProbabilityControl=1.0) :
    generator(std::random_device()()),
    distribution(0.0, 1.0),
    originaFolder(defaultDatasetDirectory, selectionProbabilityControl)
    {
        // 1. 폴더 논리적 재구성 실행
        originaFolder.reconstruction(reconstructedFolders);
        
        // 2. 현재 스레드(메인) ID 저장
        mainThreadID = std::this_thread::get_id();
    }
private:
    double __generate_probability()
    {
        namespace fs = std::filesystem;
        
        // 1. 난수를 뽑아서 반환
        return distribution(generator);
    }
public:
    void pick_thread(std::deque<std::vector<std::filesystem::path>>& vvp, bool printStatus=false, bool randomPick=true, int numThreads=-1)
    {
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
            std::cout << "  - 악상기호 조합을 생성합니다." << std::endl;
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
    void pick(std::deque<std::vector<std::filesystem::path>>& vvp, bool printStatus=false, bool randomPick=true)
    {
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
    void get_all_images_name(std::vector<std::string>& imagesNames)
    {
        // 모든 이미지 이름 구하기
        std::set<std::string> allImagesNames;
        originaFolder.get_all_images_name(allImagesNames);
        
        // 벡터로 변환 및 정렬
        imagesNames = std::vector<std::string>(allImagesNames.begin(), allImagesNames.end());
        std::sort(imagesNames.begin(), imagesNames.end());
    }
};


/*
 -------------------------------------------------------------------------------
 이름: MusicalSymbol()
 -------------------------------------------------------------------------------
 설명:
 악상기호를 보관하는 클래스.
 
 MusicalSymbol는 악상기호 이미지(1개)를 보관하고 악상기호 이미지들끼리의 조합(이미지 합성)을 위한 연산
 을 제공하는 역할을 합니다.
 -------------------------------------------------------------------------------
 주요 기능:
 - 악상기호 이미지 저장.
 - 악상기호 이미지 데이터(중심 좌표, 확대 및 축소, 회전, 배치 정보 등등) 저장 및 수정.
 - 악상기호 이미지들끼리의 조합을 위한 연산자 함수 제공
 -------------------------------------------------------------------------------
 사용 방법:
 악상기호 이미지는 다음과 같이 불러옵니다.
 MSIG::Algorithm::MusicalSymbol ms("악상기호 이미지 주소");
 
 이미지 데이터를 새로 만들거나 수정하고 싶다면 다음과 같이 사용합니다.
 ms.make_config()
 ms.edit_config()
 
 악상기호 이미지들끼리의 합성은 다음과 같이 진행합니다.
 (bool)ms  // 비어있는 이미지인지 확인하기
 ms1 + ms2 // 배치 정보를 참고해서 이미지 합성하기
 ms1 & ms2 // 중심 좌표를 참고해서 and 연산
 ms1 | ms2 // 중심 좌표를 참고해서 or 연산
 -------------------------------------------------------------------------------
 주의 사항:
 - 생성된 객체와 연결된 이미지와 이미지 데이터 파일은 다른곳에서 수정이 일어난다 해도 현재 객체에 저장되어
 있는 데이터를 기반으로 연산이 처리됨.
 -------------------------------------------------------------------------------
 */

#define MS_POSITION_TYPE uint8_t
#define MS_FIXED  0b00000000
#define MS_TOP    0b00000001
#define MS_BOTTOM 0b00000010
#define MS_LEFT   0b00000100
#define MS_RIGHT  0b00001000
#define MS_STAFF  0b10000000
#define MS_IN     0b00010000
#define MS_OUT    0b00100000

class MusicalSymbol
{
public:
    // 원본 이미지 여부
    bool original;
public:
    // 이미지 경로
    std::filesystem::path path;
public:
    // 이미지 데이터
    cv::Mat          image;
    int              x;
    int              y;
    double           degree;
    double           scale;
    MS_POSITION_TYPE position;
public:
    // 생성할 이미지의 정보
    int width;
    int height;
    int diagonal;
    int staffPadding;
public:
    // 배치 관련 변수
    int  pitch;                                     // 현재 음정
    int  edge[4];                                   // 오선지 경계
    int  in  [4];                                   // 오선지 안
    int  out [4];                                   // 오선지 밖
    int  pad [4];                                   // 부가적인 패딩값
public:
    MusicalSymbol(std::filesystem::path imagePath, bool makingConfigData=false, int width=192, int height=512, int staffPadding=26)
    {
        namespace fs = std::filesystem;
        
        // *. 기본 값 설정
        this->original = true;
        this->width = width;
        this->height = height;
        this->diagonal = (int)sqrt((double)(width * width)+(double)(height * height));
        this->staffPadding = staffPadding;
        
        // 1. 존재하는 이미지 파일인지 확인
        // TODO: .png 파일만 이미지 파일로 할건지 생각해보기
        if (fs::exists(imagePath)                   &&
            fs::is_regular_file(imagePath)          &&
            imagePath.extension().string()==".png")
        {
            this->path = imagePath;
        }
        else
        {
            std::string errorMessage = "MSIG::Algorithm::MusicalSymbol.MusicalSymbol() : \"" + imagePath.string() + "\"은 존재하지 않는 이미지입니다.";
            throw std::runtime_error(errorMessage);
        }
        
        // 2. __config__.txt 파일이 없으면 생성
        if (!fs::exists(path.parent_path()/fs::path("__config__.txt")) ||
            !fs::is_regular_file(path.parent_path()/fs::path("__config__.txt")))
        {
            std::fstream((path.parent_path()/fs::path("__config__.txt")).string(), std::ios::out).close();
        }
        
        // 3. 이미지 불러오기
        this->image = cv::imread(this->path.string(), cv::IMREAD_GRAYSCALE);
        if (!this->image.data)
        {
            std::string errorMessage = "MSIG::Algorithm::MusicalSymbol.MusicalSymbol() : \"" + imagePath.string() + "\"이미지를 불러오는데 실패하였습니다.";
            throw std::runtime_error(errorMessage);
        }
        
        // 4. 이미지 데이터 불러오기
        // TODO: 데이터들의 좌우 공백을 제거하여 값을 저장할 방법 생각하기.
        std::string imageData = Utility::grep(imagePath.parent_path()/fs::path("__config__.txt"), imagePath.filename().string());
        if (imageData.empty())
        {
            // 4-1. 이미지 데이터가 없으면 새로 생성
            this->x = 0;
            this->y = 0;
            this->degree = 0.0;
            this->scale = 1.0;
            this->position = MS_FIXED;
            
            // 4-2. 생성 여부 확인
            if (makingConfigData) {
                edit_config();
            }
        }
        else
        {
            // 3-2. 이미지 데이터가 있으면 저장
            try
            {
                // 데이터 분리
                std::vector<std::string> splitedData = Utility::split(Utility::split(imageData, "=").at(1), "~");
                
                // 정규표현식 검사
                std::regex regexForInt("^([-+]?[0-9]+)$");
                std::regex regexForDouble("^([-+]?([0-9]+.[0-9]*|[0-9]*.[0-9]+))$");
                
                // 에러 메시지 생성
                std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"에 관해 기재된 규칙 값이 잘못되었습니다.";
                
                // 정규표현식 매칭 확인 후 저장
                if (std::regex_match(splitedData.at(0), regexForInt)    &&
                    std::regex_match(splitedData.at(1), regexForInt)    &&
                    std::regex_match(splitedData.at(2), regexForDouble) &&
                    std::regex_match(splitedData.at(3), regexForDouble))
                {
                    this->x      = std::stoi(splitedData.at(0));
                    this->y      = std::stoi(splitedData.at(1));
                    this->degree = std::stod(splitedData.at(2));
                    this->scale  = std::stod(splitedData.at(3));
                }
                else throw std::runtime_error(errorMessage);
                
                // 악상기호 배치 정보 추출
                std::vector<std::string> positions = Utility::split(splitedData.at(4), ",");
                if      (positions.at(0)=="fixed")  this->position = MS_FIXED;
                else if (positions.at(0)=="staff")  this->position = MS_STAFF;
                else if (positions.at(0)=="top")    this->position = MS_TOP;
                else if (positions.at(0)=="bottom") this->position = MS_BOTTOM;
                else if (positions.at(0)=="left")   this->position = MS_LEFT;
                else if (positions.at(0)=="right")  this->position = MS_RIGHT;
                else throw std::runtime_error(errorMessage);
                if      (positions.size()>1 && positions.at(1)=="in")  this->position |= MS_IN;
                else if (positions.size()>1 && positions.at(1)=="out") this->position |= MS_OUT;
                else if (positions.size()>1) throw std::runtime_error(errorMessage);
                
                // 오선지 이미지 인경우 특수 처리
                if (this->position & MS_STAFF)
                {
                    // 오선지 이미지인 경우 다음의 추가적인 값들을 계산함.
                    // pitch    : 현재 기본이 되는 음정
                    // edge[4]  : 악상기호를 오선지 안에 각각 몇개를 배치할 수 있는지의 갯수.
                    // in[4]    : 현재 오선지 안에 배치되어 있는 악상기호 갯수.
                    // out[4]   : 현재 오선지 안에 최대로 배치할 수 있는 악상기호 갯수.
                    
                    // pitch, ledger 정보 추출
                    std::vector<std::string> staffData = Utility::split(this->path.filename().stem().string(), "-");
                    
                    // 숫자 값으로 변환
                    int p=0;
                    int t=0;
                    int b=0;
                    try {
                        p = __pitch_to_number(staffData.at(1));                         // 음정 위치
                        t = __pitch_to_number("f5") + 2 * std::stoi(staffData.at(2));   // ledger top 위치
                        b = __pitch_to_number("e4") - 2 * std::stoi(staffData.at(3));   // ledger bottom 위치
                        
                        this->pitch = p;
                    }
                    catch (const std::out_of_range& e)
                    {
                        // staff 이름 형식이 잘못되면 에러
                        std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"staff 이미지 이름 형식이 잘못되었습니다.";
                        throw std::runtime_error(errorMessage);
                    }
                    
                    // 현재 오선지 이미지에서 벗어난 곳에 음정이 설정되어 있는지 확인
                    if (p < b-1 || p > t+1) {
                        throw std::runtime_error("MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\" staff 이미지 이름의 pitch와 ledger 정보가 잘못되었습니다.");
                    }
                    
                    // 현재 pitch 를 기준으로 상,하,좌,우에 악상기호들이 몇 개가 들어갈 수 있는지 계산.
                    this->edge[0] = (t - p) / 2;    // 현재 pitch를 기준으로 위에는 악상기호가 몇 개 들어갈수 있는지 계산.
                    this->edge[1] = (p - b) / 2;    // 현재 pitch를 기준으로 아래에는 악상기호가 몇 개 들어갈수 있는지 계산.
                    this->edge[2] = 100;            // 좌,우 는 기본 100개의 악상기호가 들어갈 수 있음.
                    this->edge[3] = 100;
                    
                    // 현재 pitch가 실선에 겹쳐있는지 겹쳐있지 않은지에 따라 배치할 수 있는 공간 1칸을 건너뛸지 말지 확인.
                    if (this->edge[0] > 0 && p % 2 == 0) {
                        this->in[0] = 1;
                        this->in[1] = 1;
                    }
                    
                    // 오선지 내부 배치를 벗어나는 지점 계산.
                    this->out[0] = edge[0];
                    this->out[1] = edge[1];
                    this->out[2] = edge[2];
                    this->out[3] = edge[3];
                    
                    // 배치 정보 추가
                    this->position |= MS_FIXED;
                }
            }
            catch (const std::out_of_range& e)
            {
                // 불러온 데이터 갯수가 이상하면 에러
                std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"에 관해 기재된 규칙 형식이 잘못되었습니다.";
                throw std::runtime_error(errorMessage);
            }
        }
        
        // 5. config 정보 저장
        __save_config();
    }
public:
    explicit operator bool() const
    {
        // 1. 현재 이미지에 검은색 픽셀이 존재하는지 확인. (임계값=128)
        // 현재 이미지 복사
        cv::Mat copiedImage = this->image.clone();
        
        // 임계값 처리(0 또는 1로)
        // FIXME: 경계 처리 값이 128을 상수로 취급하는게 좋을지 생각해보기.
        cv::threshold(copiedImage, copiedImage, 128, 255, cv::THRESH_BINARY);
        
        // 검은색(0) 값이 존재하는지 확인
        double minVal, maxVal;
        cv::minMaxLoc(copiedImage, &minVal, &maxVal);
        
        // 2. 결과 반환
        if (minVal > 0) return false;
        else            return true;
    }
    MusicalSymbol operator& (const MusicalSymbol& other) const
    {
        // 합성하려는 두 이미지의 같은 픽셀 위치에서 가장 작은(어두운) 값이 선택됨.
        // 256 & 256 -> 256
        // 256 & 194 -> 194
        // 194 & 0   -> 0
        // 0   & 0   -> 0
        
        // 1. 두 이미지 준비
        MusicalSymbol thisImage = this->copy();
        MusicalSymbol otherImage = other.copy();
        thisImage.__as_default();
        otherImage.__as_default();
        
        // 2. 두 이미지를 포함하는 영역 계산
        // ti=thisImage, oi=otherImage, ni=newImage
        // t=top, b=bottom, l=left, r=right
        int ti_t = (thisImage.y);
        int ti_b = (thisImage.image.rows) - (thisImage.y);
        int ti_l = (thisImage.x);
        int ti_r = (thisImage.image.cols) - (thisImage.x);
        int oi_t = (otherImage.y);
        int oi_b = (otherImage.image.rows) - (otherImage.y);
        int oi_l = (otherImage.x);
        int oi_r = (otherImage.image.cols) - (otherImage.x);
        int ni_t = (ti_t > oi_t) ? (ti_t) : (oi_t);
        int ni_b = (ti_b > oi_b) ? (ti_b) : (oi_b);
        int ni_l = (ti_l > oi_l) ? (ti_l) : (oi_l);
        int ni_r = (ti_r > oi_r) ? (ti_r) : (oi_r);
        cv::Size ni_sz(ni_l+ni_r, ni_t+ni_b);
        
        // 3. 두개의 흰 배경 생성
        cv::Mat img1(ni_sz, CV_8UC1, cv::Scalar(255));
        cv::Mat img2(ni_sz, CV_8UC1, cv::Scalar(255));
        
        // 4. 좌표 계산
        int ti_x, ti_y, oi_x, oi_y;
        if (ti_l > oi_l) {
            ti_x = 0;
            oi_x = ti_l - oi_l;
        }
        else {
            ti_x = oi_l - ti_l;
            oi_x = 0;
        }
        if (ti_t > oi_t) {
            ti_y = 0;
            oi_y = ti_t - oi_t;
        }
        else {
            ti_y = oi_t - ti_t;
            oi_y = 0;
        }
        
        // 5. 기존 이미지를 흰 배경에 붙여넣기
        thisImage.image.copyTo(img1(cv::Rect(ti_x, ti_y, thisImage.image.cols, thisImage.image.rows)));    // thisImage.image를 img1 안에 복사
        otherImage.image.copyTo(img2(cv::Rect(oi_x, oi_y, otherImage.image.cols, otherImage.image.rows))); // otherImage.image를 img2 안에 복사
        
        // 6. 두 흰 배경 합성
        cv::Mat new_img;
        cv::bitwise_and(img1, img2, new_img);
        
        // 7. 반환할 MusicalImage 정보 업데이트
        thisImage.image = new_img;
        if (ti_l < oi_l) thisImage.x = oi_l;
        if (ti_t < oi_t) thisImage.y = oi_t;
        
        // 8. 반환
        return thisImage;
    }
    MusicalSymbol operator| (const MusicalSymbol& other) const
    {
        // 합성하려는 두 이미지의 같은 픽셀 위치에서 가장 큰(밝은) 값이 선택됨.
        // 256 | 256 -> 256
        // 256 | 194 -> 256
        // 194 | 0   -> 194
        // 0   | 0   -> 0
        
        // 1. 두 이미지 준비
        MusicalSymbol thisImage = this->copy();
        MusicalSymbol otherImage = other.copy();
        thisImage.__as_default();
        otherImage.__as_default();
        
        // 2. 두 이미지를 포함하는 영역 계산
        // ti=thisImage, oi=otherImage, ni=newImage
        // t=top, b=bottom, l=left, r=right
        int ti_t = (thisImage.y);
        int ti_b = (thisImage.image.rows) - (thisImage.y);
        int ti_l = (thisImage.x);
        int ti_r = (thisImage.image.cols) - (thisImage.x);
        int oi_t = (otherImage.y);
        int oi_b = (otherImage.image.rows) - (otherImage.y);
        int oi_l = (otherImage.x);
        int oi_r = (otherImage.image.cols) - (otherImage.x);
        int ni_t = (ti_t > oi_t) ? (ti_t) : (oi_t);
        int ni_b = (ti_b > oi_b) ? (ti_b) : (oi_b);
        int ni_l = (ti_l > oi_l) ? (ti_l) : (oi_l);
        int ni_r = (ti_r > oi_r) ? (ti_r) : (oi_r);
        cv::Size ni_sz(ni_l+ni_r, ni_t+ni_b);
        
        // 3. 두개의 흰 배경 생성
        cv::Mat img1(ni_sz, CV_8UC1, cv::Scalar(255));
        cv::Mat img2(ni_sz, CV_8UC1, cv::Scalar(255));
        
        // 4. 좌표 계산
        int ti_x, ti_y, oi_x, oi_y;
        if (ti_l > oi_l) {
            ti_x = 0;
            oi_x = ti_l - oi_l;
        }
        else {
            ti_x = oi_l - ti_l;
            oi_x = 0;
        }
        if (ti_t > oi_t) {
            ti_y = 0;
            oi_y = ti_t - oi_t;
        }
        else {
            ti_y = oi_t - ti_t;
            oi_y = 0;
        }
        
        // 5. 기존 이미지를 흰 배경에 붙여넣기
        thisImage.image.copyTo(img1(cv::Rect(ti_x, ti_y, thisImage.image.cols, thisImage.image.rows)));    // thisImage.image를 img1 안에 복사
        otherImage.image.copyTo(img2(cv::Rect(oi_x, oi_y, otherImage.image.cols, otherImage.image.rows))); // otherImage.image를 img2 안에 복사
        
        // 6. 두 흰 배경 합성
        cv::Mat new_img;
        cv::bitwise_or(img1, img2, new_img);
        
        // 7. 반환할 MusicalImage 정보 업데이트
        thisImage.image = new_img;
        if (ti_l < oi_l) thisImage.x = oi_l;
        if (ti_t < oi_t) thisImage.y = oi_t;
        
        // 8. 반환
        return thisImage;
    }
    MusicalSymbol operator+ (const MusicalSymbol& other) const
    {
        // 현재 이미지에 다른 이미지를 적절한 위치에 배치 붙여넣기
        
        // 1. 두 이미지 준비
        MusicalSymbol thisImage = this->copy();
        MusicalSymbol otherImage = other.copy();
        thisImage.__as_default();
        otherImage.__as_default();
        MusicalSymbol& mainImage = thisImage;
        MusicalSymbol& subImage = otherImage;
        
        
        // 2. 오선지가 포함되어있는 MusicalSymbol이 메인으로 사용됨
        if      ((thisImage.position & MS_STAFF) && !(otherImage.position & MS_STAFF))
        {
            mainImage = thisImage;
            subImage = otherImage;
        }
        else if (!(thisImage.position & MS_STAFF) && (otherImage.position & MS_STAFF))
        {
            mainImage = otherImage;
            subImage = thisImage;
        }
        else
        {
            std::string errorMessage = "MSIG::Algorithm::MusicalSymbol.operator+() : + 연산을 진행하려는 MusicalSymbol 중에 하나는 staff 이미지를 포함해야 하고, 다른 하나는 staff 이미지를 포함하지 않아야 합니다.";
            throw std::runtime_error(errorMessage);
        }
        
        // 3. 악상기호 배치
        // mainImage에 붙일 sumImage의 x, y 좌표를 수정함.
        
        if      (subImage.position & MS_TOP)
        {
            // 안쪽 배치
            if ((subImage.position & MS_IN) && (mainImage.edge[0] > mainImage.in[0]))
            {
                subImage.y += mainImage.staffPadding * mainImage.in[0]++;
                
                if (pitch % 2 == 0) otherImage.y += mainImage.staffPadding / 2.0;
                else                otherImage.y += mainImage.staffPadding;
            }
            // 바깥쪽 배치
            else
            {
                // subImage의 여백을 제거한 크기의 검은 이미지 생성
                MusicalSymbol tmpImage = subImage.copy();
                tmpImage.image = Processing::Matrix::remove_padding(tmpImage.image, tmpImage.x, tmpImage.y);
                tmpImage.image = cv::Mat(tmpImage.image.rows, tmpImage.image.cols, CV_8UC1, cv::Scalar(0));
                
                // 배치를 위해 오선지 밖 좌표로 이동
                tmpImage.y += mainImage.staffPadding * mainImage.out[0];
                tmpImage.y += mainImage.pad[0];
                
                if (pitch % 2 == 0) tmpImage.y += mainImage.staffPadding / 2.0;
                else                tmpImage.y += mainImage.staffPadding;
                
                if (static_cast<bool>(mainImage | tmpImage))
                {
                    while (mainImage | tmpImage){
                        tmpImage.y += 1;
                        mainImage.pad[0] += 1;
                    }
                    
                    tmpImage.y += mainImage.staffPadding / 4.5;
                    mainImage.pad[0] += mainImage.staffPadding / 4.5;
                }
                
                // 악상기호 좌표 이동
                subImage.y += mainImage.staffPadding * mainImage.out[0]++;
                subImage.y += mainImage.pad[0];
                
                if (pitch % 2 == 0) subImage.y += mainImage.staffPadding / 2.0;
                else                subImage.y += mainImage.staffPadding;
            }
        }
        else if (subImage.position & MS_BOTTOM)
        {
            // 안쪽 배치
            if ((subImage.position & MS_IN) && (mainImage.edge[1] > mainImage.in[1]))
            {
                subImage.y -= mainImage.staffPadding * mainImage.in[1]++;
                
                if (pitch % 2 == 0) otherImage.y -= mainImage.staffPadding / 2.0;
                else                otherImage.y -= mainImage.staffPadding;
            }
            // 바깥쪽 배치
            else
            {
                // subImage의 여백을 제거한 크기의 검은 이미지 생성
                MusicalSymbol tmpImage = subImage.copy();
                tmpImage.image = Processing::Matrix::remove_padding(tmpImage.image, tmpImage.x, tmpImage.y);
                tmpImage.image = cv::Mat(tmpImage.image.rows, tmpImage.image.cols, CV_8UC1, cv::Scalar(0));
                
                // 배치를 위해 오선지 밖 좌표로 이동
                tmpImage.y -= mainImage.staffPadding * mainImage.out[1];
                tmpImage.y -= mainImage.pad[1];
                
                if (pitch % 2 == 0) tmpImage.y -= mainImage.staffPadding / 2.0;
                else                tmpImage.y -= mainImage.staffPadding;
                
                if (static_cast<bool>(mainImage | tmpImage))
                {
                    while (static_cast<bool>(mainImage | tmpImage)){
                        tmpImage.y -= 1;
                        mainImage.pad[1] += 1;
                    }
                    
                    tmpImage.y -= mainImage.staffPadding / 4.5;
                    mainImage.pad[1] += mainImage.staffPadding / 4.5;
                }
                
                // 악상기호 좌표 이동
                subImage.y -= mainImage.staffPadding * mainImage.out[1]++;
                subImage.y -= mainImage.pad[1];
                
                if (pitch % 2 == 0) subImage.y -= mainImage.staffPadding / 2.0;
                else                subImage.y -= mainImage.staffPadding;
            }
        }
        else if (subImage.position & MS_LEFT)
        {
            // TODO: 글리산도 악상기호도 배치가 가능하도록 수정해야함. 오선지를 제거 해야될 것 같은 느낌
            // 안쪽 배치
            if ((subImage.position & MS_IN) && (mainImage.edge[2] > mainImage.in[2]))
            {
                subImage.x += mainImage.staffPadding * ++(mainImage.in[2]);
            }
            // 바깥쪽 배치
            else
            {
                // FIXME: 현재로써는 오선지 좌우 바깥쪽으로 악상기호가 배치될 일이 없어서 이렇게 해두었음.
                subImage.x += mainImage.staffPadding * ++(mainImage.out[2]);
            }
        }
        else if (subImage.position & MS_RIGHT)
        {
            // TODO: 글리산도 악상기호도 배치가 가능하도록 수정해야함. 오선지를 제거 해야될 것 같은 느낌
            // 안쪽 배치
            if ((subImage.position & MS_IN) && (mainImage.edge[3] > mainImage.in[3]))
            {
                subImage.x -= mainImage.staffPadding * ++(++mainImage.in[3]);
            }
            // 바깥쪽 배치
            else
            {
                // FIXME: 현재로써는 오선지 좌우 바깥쪽으로 악상기호가 배치될 일이 없어서 이렇게 해두었음.
                subImage.x -= mainImage.staffPadding * ++(++mainImage.out[3]);
            }
        }
        
        // 4. 두 악상기호를 합치고 반환.
        return mainImage & subImage;
    }
private:
    cv::Mat __rendering(int x, int y, double degree, double scale, bool extensionSize, bool auxiliaryStaff, bool auxiliaryCenter)
    {
        namespace fs = std::filesystem;
        
        //
        cv::Mat newImage((extensionSize ? diagonal : width), (extensionSize ? diagonal : height), CV_8UC1, cv::Scalar(255));
        cv::Mat musicalSymbolImage = this->image.clone();
        
        // 이미지 편집
        musicalSymbolImage = Processing::Matrix::mat_rotate(musicalSymbolImage, degree, x, y);  // 이미지 회저
        musicalSymbolImage = Processing::Matrix::mat_scale(musicalSymbolImage, scale, x, y);    // 이미지 확대 축소
        
        // 보조선 그리기 : 배경 이미지에 오선지 그리기
        if (auxiliaryStaff)
        for (auto h : std::vector<int>({-staffPadding*2, -staffPadding, 0, staffPadding, staffPadding*2}))
        {
            int n = newImage.rows/2.0;
            cv::line(newImage, cv::Point(0,n+h), cv::Point(newImage.cols,n+h), cv::Scalar(200), 2.2, cv::LINE_AA);
        }
        
        // 배경 이미지와 악상기호 이미지 합성 좌표 계산
        int combineX = (newImage.cols/2.0) - x;
        int combineY = (newImage.rows/2.0) - y;
        
        // 배경 이미지와 악상기호 이미지 합성
        newImage = Processing::Matrix::mat_attach(newImage, musicalSymbolImage, combineX, combineY);
        
        // 보조선 그리기 : 완성된 이미지 위에 십자선
        if (auxiliaryCenter)
        {
            cv::Point crossPoint(newImage.cols/2.0, newImage.rows/2.0);
            cv::line(newImage, crossPoint+cv::Point(-10,0), crossPoint+cv::Point(10,0), cv::Scalar(100), 1, cv::LINE_AA);
            cv::line(newImage, crossPoint+cv::Point(0,-10), crossPoint+cv::Point(0,10), cv::Scalar(100), 1, cv::LINE_AA);
        }
        
        // 완성된 이미지 반환
        return newImage;
    }
    void    __save_config()
    {
        namespace fs = std::filesystem;
        
        // *. 만약 MusicalSymbol가 원본이면 저장 불가능
        if (original==false) {
            std::string errorMessage = "MSIG::Algorithm::MusicalSymbol.__save_config() : 해당 악상 기호는 복사본이기 때문에 config 데이터를 저장할 수 없습니다.";
            throw std::runtime_error(errorMessage);
        }
        
        // *. 변수들
        std::map<std::string, std::string> imageData;
        std::string line;
        fs::path configPath = path.parent_path() / fs::path("__config__.txt");
        
        // 1. __config__.txt 내용들 imageData에 저장 및 검사
        //
        std::ifstream originalFile(configPath.string());
        if (originalFile.is_open())
        {
            while (std::getline(originalFile, line)) {
                // 읽은 데이터 분할
                std::vector<std::string> splitedData = Utility::split(line, "=");
                if (splitedData.size()!=2)
                    continue;
                // 존재하는 파일의 데이터인지 확인
                if (fs::exists(path.parent_path()/fs::path(splitedData.at(0))) && fs::is_regular_file(path.parent_path()/fs::path(splitedData.at(0)))) {
                    imageData[splitedData.at(0)] = splitedData.at(1);
                }
            }
            originalFile.close();
        }
        
        // 2. __config__.txt 에 이미지 데이터 저장
        std::ofstream newFile(configPath.string());
        newFile << std::endl << "# config";
        bool found = false;
        for (auto& [key, value] : imageData)
        {
            if (!found && (key == path.filename().string())) {
                // 일치하는 기존 데이터를 찾았다면,
                found = true;
                newFile << std::endl << key + "=" + std::to_string(x) + "~" + std::to_string(y) + "~" + std::to_string(degree) + "~" + std::to_string(scale);
                //
                if      (position == MS_FIXED)  newFile << "~fixed";
                else if (position & MS_STAFF)   newFile << "~staff";
                else if (position & MS_TOP)     newFile << "~top";
                else if (position & MS_BOTTOM)  newFile << "~bottom";
                else if (position & MS_LEFT)    newFile << "~left";
                else if (position & MS_RIGHT)   newFile << "~right";
                //
                if      (position & MS_IN)  newFile << ",in";
                else if (position & MS_OUT) newFile << ",out";
                
            }
            else if (found && (key == path.filename().string())) {
                // 일치하지만 또 찾았을 경우, 건너뜀
                continue;
            }
            else {
                // 다른 데이터들은 그냥 저장
                newFile << std::endl << key + "=" + value;
            }
        }
        if (!found) {
            newFile << std::endl << path.filename().string() + "=" + std::to_string(x) + "~" + std::to_string(y) + "~" + std::to_string(degree) + "~" + std::to_string(scale);
        }
        newFile.close();
    }
    void    __as_default()
    {
        namespace fs = std::filesystem;
        
        //
        original = false;
        
        // 이미지 수정
        image = image.clone();
        image = Processing::Matrix::mat_rotate(image, degree, x, y);
        image = Processing::Matrix::mat_scale(image, scale, x, y);
    }
    int     __pitch_to_number(const std::string& pitch)
    {
        // 음정(p)과 옥타브(o)값 추출
        char p = pitch[0];
        int  o = pitch[1] - 48;
        
        // 옥타브 계산
        int number = o * 7;
        
        // 음정 계산
        switch (p)
        {
            case 'c': number += 0; break;
            case 'd': number += 1; break;
            case 'e': number += 2; break;
            case 'f': number += 3; break;
            case 'g': number += 4; break;
            case 'a': number += 5; break;
            case 'b': number += 6; break;
        }
        
        return number;
    }
public:
    void    edit_config()
    {
        namespace fs = std::filesystem;
        
        // 1. 기존 값 복사
        int tmpX = this->x;
        int tmpY = this->y;
        double tmpDegree = this->degree;
        double tmpScale = this->scale;
        
        // 2. 창 이름 생성
        std::string imageName = this->path.filename().string();
        
        // 3. 창 생성
        cv::namedWindow(imageName, cv::WINDOW_AUTOSIZE);
        
        // 4. 편집 화면 보여주기(30Hz)
        while (true)
        {
            // 현재 이미지 그리기
            cv::imshow(imageName, __rendering(x, y, degree, scale, true, true, true));
            
            // 키보드 입력
            int key = cv::waitKey(1000/30);
            
            // 완료에 의한 종료
            if (key=='\n'|| key=='\r') {
                break;
            }
            // 중단에 의한 종료
            if (key==27) {
                // 기존 값 복원
                this->x = tmpX;
                this->y = tmpY;
                this->degree = tmpDegree;
                this->scale = tmpScale;
                break;
            }
            
            // 키보드 이벤트
            switch (key) {
                // 기본 속도
                case 'a': this->x += 1; break;  // 좌로 이동
                case 'd': this->x -= 1; break;  // 우로 이동
                case 'w': this->y += 1; break;  // 상으로 이동
                case 's': this->y -= 1; break;  // 하로 이동
                case 'q': this->degree+=1.0;  break;  // 반시계 회전
                case 'e': this->degree-=1.0;  break;  // 시계 회전
                case 'z': this->scale-=0.1;   break;  // 축소
                case 'c': this->scale+=0.1;   break;  // 확대
                // 빠르게
                case 'A': this->x+=5;         break;  // 좌로 이동
                case 'D': this->x-=5;         break;  // 우로 이동
                case 'W': this->y+=5;         break;  // 상으로 이동
                case 'S': this->y-=5;         break;  // 하로 이동
                case 'Q': this->degree+=3.0;  break;  // 반시계 회전
                case 'E': this->degree-=3.0;  break;  // 시계 회전
                case 'Z': this->scale-=0.2;   break;  // 축소
                case 'C': this->scale+=0.2;   break;  // 확대
                // config 초기화
                case 'x':                       // 초기화
                case 'X':                       // 초기화
                    this->x=0;
                    this->y=0;
                    this->degree=0.0;
                    this->scale=1.0;
                    break;
            }
            
            // 범위 값 처리
            if (this->degree<0.0)   this->degree += 360.0;
            if (this->degree>360.0) this->degree -= 360.0;
            if (this->scale<0.1)    this->scale = 0.1;
        }
        
        // 5. 윈도우 닫기
        cv::destroyWindow(imageName);
        
        // 6. config 정보 저장
        __save_config();
    }
    void    show()
    {
        //
        cv::Mat previewImage = __rendering(x, y, degree, scale, true, false, false);
        
        //
        cv::imshow("MusicalSymbol preview", previewImage);
        cv::waitKey();
        cv::destroyWindow("MusicalSymbol preview");
    }
    cv::Mat rendering(bool extensionSize, bool auxiliaryStaff, bool auxiliaryCenter)
    {
        return __rendering(x, y, degree, scale, extensionSize, auxiliaryStaff, auxiliaryCenter);
    }
public:
    MusicalSymbol copy() const
    {
        MusicalSymbol ms = *this;
        ms.image = ms.image.clone();
        ms.original = false;
        return ms;
    }
};


}
}

#endif /* msig_Algorithm_hpp */
