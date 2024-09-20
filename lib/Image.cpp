
#include <msig_Structure.hpp>

namespace MSIG {
namespace Structure {

Image::Image(const std::filesystem::path& imagePath)
{
    namespace fs = std::filesystem;
    
    // *. 기본 값 설정
    this->selectionProbability = 1.0;
    this->groupName = "";
    
    // 1. imagePath가 올바른 이미지 경로인지 확인
    // FIXME: 이미지 확장자를 .png 말고 다른 이미지 파일로도 받을지 고민해보기.
    if (!fs::exists(imagePath)                  ||
        !fs::is_regular_file(imagePath)         ||
        imagePath.extension().string()!=".png")
    {
        std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"은 존재하지 않는 이미지입니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 2. 이미지 주소 저장
    this->path = imagePath;
    
    // 3. 이미지 데이터 불러오기
    try {
        // FIXME: 데이터들의 좌우 공백을 제거하여 값을 저장할 방법 생각하기.
        std::string imageData = Utility::grep(imagePath.parent_path()/fs::path("__rule__.txt"), imagePath.filename().string());
        std::vector<std::string> splitedData;
        
        while (true)
        {
            // 3-1. 데이터가 비어있으면 새로 생성
            if (imageData.empty())
                imageData = imagePath.filename().string() + "=~";
            
            // 3-2. 데이터 종류 나누기
            splitedData = Utility::split(Utility::split(imageData, "=").at(1), "~");
            if (splitedData.size()>2)
            {
                std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"에 관해 기재된 규칙 형식이 잘못되었습니다.";
                throw std::runtime_error(errorMessage);
            }
            if (splitedData.size()==2)
            {
                break;
            }
            if (splitedData.size()==1 && !splitedData.at(0).empty())
            {
                this->groupName = splitedData.at(0); // 그룹 이름 지정
                imageData = Utility::grep(imagePath.parent_path()/fs::path("__rule__.txt"), this->groupName);
            }
            else
            {
                imageData = imagePath.filename().string() + "=~~";
            }
        }
        
        // 3-3. 다중 선택 정보 저장
        for (auto& s1 : Utility::split(splitedData.at(1), ","))
        {
            // 문자열에서 다중 선택 경로 추출
            fs::path currentLevelPath = this->path.parent_path() / fs::path(s1);
            
            // currentLevel에서 폴더 존재 확인
            if (fs::exists(currentLevelPath) &&
                fs::is_directory(currentLevelPath))
            {
                // 다중 선택 정보에 경로 추가
                this->multipleSelectionFolders.push_back(currentLevelPath);
            }
        }
        
        // 3-3. 제외 정보 저장
        for (auto& s1 : Utility::split(splitedData.at(0), ","))
        {
            // 문자열에서 제외 경로 추출
            fs::path stringToPath;
            for (auto& s2 : Utility::split(s1, "_")) {
                stringToPath = stringToPath / fs::path(s2);
            }
            fs::path currentLevelPath = this->path.parent_path() / stringToPath;
            fs::path parentLevelPath = this->path.parent_path().parent_path() / stringToPath;
            
            // currentLevel에서 폴더 또는 이미지 존재 확인
            if (fs::exists(currentLevelPath)                    &&
                (fs::is_directory(currentLevelPath)             ||
                 (fs::is_regular_file(currentLevelPath)         &&
                  currentLevelPath.extension().string()==".png" &&
                  currentLevelPath != this->path)))
            {
                // 제외 정보에 경로 추가
                this->excludeFoldersAndImages.push_back(currentLevelPath);
            }
            
            // parentLevel에서 폴더 또는 이미지 존재 확인
            else if (fs::exists(parentLevelPath)                    &&
                     (fs::is_directory(parentLevelPath)             ||
                      (fs::is_regular_file(parentLevelPath)         &&
                       parentLevelPath.extension().string()==".png" &&
                       parentLevelPath != this->path)))
             {
                 // 제외 정보에 경로 추가
                 this->excludeFoldersAndImages.push_back(parentLevelPath);
             }
        }
    }
    catch (const std::out_of_range& e)
    {
        std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"에 관해 기재된 규칙 형식이 잘못되었습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 4. 불러온 이미지 데이터 검사
    // 4-1. 중복 제거
    std::set<std::filesystem::path> tmp_sp_1(this->excludeFoldersAndImages.begin(), this->excludeFoldersAndImages.end());
    std::set<std::filesystem::path> tmp_sp_2(this->multipleSelectionFolders.begin(), this->multipleSelectionFolders.end());
    this->excludeFoldersAndImages = std::vector<std::filesystem::path>(tmp_sp_1.begin(), tmp_sp_1.end());
    this->multipleSelectionFolders = std::vector<std::filesystem::path>(tmp_sp_2.begin(), tmp_sp_2.end());
    
    // FIXME: 특정 폴더 내에 있는 이미지들이 전부 제외 정보로 등록되어있는지 확인해봐야 될까? 아니면 안해도 될까?
    
    // 4-2. 제외 정보와 다중 선택 정보 교집합 검사. 교집합이 있으면 안됨.
    // NOTE: 어짜피 다중 선택 정보로 중복해서 뽑혔다고 하여도 제외 정보로 인해 없어지게됨. 그래서 코드 작성 건너뜀.
}

Image::Image(const Image& original, bool copyExclude, bool copyMultipleSelection)
{
    namespace fs = std::filesystem;
    
    // *. original Image 데이터 복사
    this->selectionProbability = original.selectionProbability;
    this->path = original.path;
    this->groupName = original.groupName;
    
    // 1. 폴더들 복사
    if (copyExclude)
        this->excludeFoldersAndImages = original.excludeFoldersAndImages;
    
    // 2. 이미지들 복사
    if (copyMultipleSelection)
        this->multipleSelectionFolders = original.multipleSelectionFolders;
}

bool
Image::operator<(const Image& other) const {
    return this->path < other.path;
}

void
Image::print_information() {
    using namespace std;
    namespace fs = std::filesystem;
    
    cout << "Name : " << path.filename().string() << endl;
    cout << "Path : " << path.string() << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "selection Probabuility : " << selectionProbability << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "groupName : " << groupName << endl;
    cout << "exclue folder and images : " << endl;
    for (const auto& p : excludeFoldersAndImages)
    {
        if (fs::is_directory(p)) cout << "|  " << p.filename().string() << endl;
        else                     cout << "|  " << (p.parent_path().filename()/p.filename()).string() << endl;
    }
    cout << "multiple selection folders : " << endl;
    for (const auto& p : multipleSelectionFolders)
    {
        cout << "|  " << p.filename().string() << endl;
    }
}

}
}
