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

#include <msig_common.hpp>
#include <msig_Structure.hpp>
#include <msig_Processing.hpp>
#include <msig_Utility.hpp>


namespace MSIG {
namespace Algorithm {


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
    std::vector<std::thread> threads;
    std::mutex mutex_vF;
    std::mutex mutex_vvp;
public:
    DependentSelectionTree(const std::filesystem::path& defaultDatasetDirectory, double declineRate=1.0);
private:
    double  __generate_probability();
    void    __thread_function(std::deque<std::vector<std::filesystem::path>>& vvp, bool randomPick);
public:
    void    reconstruction();
    void    pick(std::deque<std::vector<std::filesystem::path>>& vvp, bool randomPick, int numThreads=-1);
    void    get_all_images_name(std::vector<std::string>& imagesNames);
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
    MusicalSymbol(std::filesystem::path imagePath, bool makingConfigData, int width=192, int height=512, int staffPadding=26);
public:
    explicit        operator bool() const;
    MusicalSymbol   operator& (const MusicalSymbol& other) const;
    MusicalSymbol   operator| (const MusicalSymbol& other) const;
    MusicalSymbol   operator+ (const MusicalSymbol& other) const;
private:
    cv::Mat         __rendering(int x, int y, double degree, double scale, bool extensionSize, bool auxiliaryStaff, bool auxiliaryCenter);
    void            __save_config();
    void            __as_default();
    int             __pitch_to_number(const std::string& pitch);
public:
    void            edit_config();
    void            show();
    cv::Mat         rendering(bool extensionSize, bool auxiliaryStaff, bool auxiliaryCenter);
public:
    MusicalSymbol   copy() const;
};


}
}

#endif /* msig_Algorithm_hpp */
