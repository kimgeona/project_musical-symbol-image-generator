
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
    // TODO: 이미지 확장자를 .png 말고 다른 이미지 파일로도 받을지 고민해보기.
    if (!fs::exists(imagePath)                     ||
        !fs::is_regular_file(imagePath)            ||
        imagePath.extension().string()!=".png")
    {
        std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"은 존재하지 않는 이미지입니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 2. 이미지 주소 저장
    this->path = imagePath;
    
    try
    {
        // 3. 이미지 데이터 불러오기
        // TODO: 데이터들의 좌우 공백을 제거하여 값을 저장할 방법 생각하기.
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
        
        // 3-3. 제외 정보 저장
        for (auto& s1 : Utility::split(splitedData.at(0), ","))
        {
            fs::path stringToPath;
            for (auto& s2 : Utility::split(s1, "_")) {
                stringToPath = stringToPath / fs::path(s2);
            }
            stringToPath = this->path.parent_path() / stringToPath;
            if (fs::exists(stringToPath) && (fs::is_directory(stringToPath) || (fs::is_regular_file(stringToPath) && stringToPath.extension().string()==".png"))) {
                this->excludeFoldersAndImages.push_back(stringToPath);
            }
        }
        
        // 3-4. 다중 선택 정보 저장
        for (auto& s1 : Utility::split(splitedData.at(1), ","))
        {
            fs::path stringToPath = this->path.parent_path() / fs::path(s1);
            if (fs::exists(stringToPath) && fs::is_directory(stringToPath)) {
                this->multipleSelectionFolders.push_back(stringToPath);
            }
        }
    }
    catch (const std::out_of_range& e)
    {
        std::string errorMessage = "MSIG::Algorithm::Image.Image() : \"" + imagePath.string() + "\"에 관해 기재된 규칙 형식이 잘못되었습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 4. 불러온 이미지 데이터 검사
    // 4-1. 제외 정보 중복 제거
    // NOTE: __rule__.txt 작성된 데이터들이 정렬 되어있으면 좋겠다는 생각에 set<>을 이용하여 중복을 제거함. 만약 지금보다 빠른 성능이 요구된다면 unorderd_set<>을 이용할 것.
    std::set<std::filesystem::path> tmp_sp_1(this->excludeFoldersAndImages.begin(), this->excludeFoldersAndImages.end());
    this->excludeFoldersAndImages = std::vector<std::filesystem::path>(tmp_sp_1.begin(), tmp_sp_1.end());
    
    // 4-2. 제외 정보 (논리적인) 중복 제거
    std::vector<std::filesystem::path> tmp_vp = this->excludeFoldersAndImages;
    for (auto& p1 : tmp_vp) {
        if (fs::is_regular_file(p1)) continue;
        for (auto& p2 : tmp_vp)
            if (fs::is_regular_file(p2) && p1 == p2.parent_path())
                this->excludeFoldersAndImages.erase(std::find(this->excludeFoldersAndImages.begin(), this->excludeFoldersAndImages.end(), p2));
    }
    
    // FIXME: 만약 제외 정보에서 특정 폴더내 모든 이미지가 선택되었다면 제외 정보를 특정 폴더 이름으로 대체하는 코드를 작성해야함.
    
    // 4-3. 다중 선택 정보 중복 제거
    std::set<std::filesystem::path> tmp_sp_2(this->multipleSelectionFolders.begin(), this->multipleSelectionFolders.end());
    this->multipleSelectionFolders = std::vector<std::filesystem::path>(tmp_sp_2.begin(), tmp_sp_2.end());
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
