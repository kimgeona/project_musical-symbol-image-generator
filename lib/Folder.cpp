
#include <msig_Structure.hpp>

namespace MSIG {
namespace Structure {

Folder::Folder(const std::filesystem::path& folderPath, double declineRate)
{
    namespace fs = std::filesystem;
    
    // *. 기본 값 설정
    this->splited = false;
    this->stretched = false;
    
    // 1. folderPath가 올바른 경로인지 확인
    if (!fs::exists(folderPath) ||
        !fs::is_directory(folderPath))
    {
        std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + folderPath.string() + "\"는 올바르지 않은 폴더 경로 입니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 2. 폴더 주소 저장
    this->path = folderPath;
    
    // 3. __rule__.txt 파일 존재 확인
    if (!fs::exists(folderPath / fs::path("__rule__.txt")) ||
        !fs::is_regular_file(folderPath / fs::path("__rule__.txt")))
    {
        std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + folderPath.string() + "\"에 __rule__.txt 파일이 존재하지 않습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 4. 존재하는 이미지 조사
    for (auto& it : fs::directory_iterator(folderPath))
    {
        // FIXME: 이미지 확장자를 .png 말고 다른 이미지 파일로도 받을지 고민해보기.
        if (fs::exists(it.path())           &&
            fs::is_regular_file(it.path())  &&
            it.path().filename().extension().string()==".png")
            images.emplace_back(it.path());
    }
    
    // 5. 존재하는 폴더 조사
    for (auto& it : fs::directory_iterator(folderPath))
    {
        if (fs::exists(it.path()) &&
            fs::is_directory(it.path()))
            folders.emplace_back(it.path());
    }
    
    // 6. 빈 폴더인지 확인
    if (images.empty()) {
        std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + folderPath.string() + "\"는 비어있는 폴더이므로 데이터셋으로 구성할 수 없습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 7. 존재하는 이미지의 실행 확률 계산
    std::map<std::string, double> groupCount;
    // 7-1. 그룹이 지정된 이미지들만 갯수 카운트
    for (auto& image : images) {
        if (!image.groupName.empty())
            // FIXME: 부동 소수점 끼리의 연산은 정확도를 어떻게 할지 고민해보기. (ex: 1.0 + 1.0 -> 2.000000000000001)
            groupCount[image.groupName] += 1.0;
    }
    // 7-2. 그룹이 지정된 이미지들만 뽑기 확률 계산
    for (auto& [groupName, count] : groupCount) {
        count = 1.0 / count * declineRate;
    }
    // 7-3. 그룹이 지정된 이미지들만 뽑기 확률 새로 지정
    for (auto& image : images) {
        if (!image.groupName.empty())
            image.selectionProbability = groupCount[image.groupName];
    }
    
    // 8. __rule__.txt 검사
    // 8-1. 그룹 정보가 다른 이미지의 이름으로 지정되어 있다면 에러 발생
    for (auto& image : images)
    for (auto& otherImage : images)
    {
        if (image.path == otherImage.path) continue;
        if (image.groupName == otherImage.path.stem()){
            std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + image.path.string() + "\"의 그룹 이름을 \"" + otherImage.path.stem().string() + "\"로 지정할 수 없습니다.";
            throw std::runtime_error(errorMessage);
        }
    }
    
    // 8-2. 제외 정보(폴더)와 다중 선택 정보(폴더)의 교집합이 있으면 안됨. (단, 제외 정보(이미지)와 다중 선택 정보(폴더)는 가능)
    // NOTE: 어짜피 다중 선택 정보로 중복해서 뽑혔다고 하여도 제외 정보로 인해 없어지게됨. 그래서 코드 작성 건너뜀.
    
    // 9. 폴더 정렬
    std::sort(folders.begin(), folders.end());
    std::sort(images.begin(), images.end());
    
    // 10. __rule__.txt 새로 저장
    __save_rule(false);
}

Folder::Folder(const Folder& original, bool copyFolders, bool copyImages)
{
    namespace fs = std::filesystem;
    
    // *. original Folder 데이터 복사
    this->splited = original.splited;
    this->stretched = original.stretched;
    this->path = original.path;
    
    // 1. 폴더들 복사
    if (copyFolders)
        this->folders = original.folders;
    
    // 2. 이미지들 복사
    if (copyImages)
        this->images = original.images;
}

Folder::operator std::vector<Folder>() const
{
    if (folders.empty()) {
        return std::vector<Folder>({Folder(*this, false, true)});
    }
    if (folders.size() > 1) {
        std::string errorMessage = "MSIG::Algorithm::Folder.std::vector<Folder>() : 일렬로 연결된 폴더만 vector<Folder> 타입 변환이 가능합니다.";
        throw std::runtime_error(errorMessage);
    }
    else {
        std::vector<Folder> vF = static_cast<std::vector<Folder>>(folders[0]);
        vF.insert(vF.begin(), Folder(*this, false, true));
        return vF;
    }
}

Folder::operator size_t() const
{
    // 현재 폴더의 이미지 갯수 조사
    size_t imageCount = this->images.size();
    
    // 하위 폴더가 없다면 현재 폴더 이미지 갯수만 반환
    if (folders.size()==0)
        return imageCount;
    
    // 하위 폴더의 이미지 갯수 조사
    size_t subImageCount = 0;
    for (auto& folder : folders)
    {
        subImageCount += static_cast<size_t>(folder);
    }
    
    return imageCount * subImageCount;
}

bool
Folder::operator<(const Folder& other) const {
    return this->path < other.path;
}

int
Folder::operator()(const Folder& other) {
    // *. 분할된 폴더만 해당 연산자를 사용할 수 있음
    if (!this->splited) {
        std::string errorMessage = "MSIG::Algorithm::Folder.operator() : 분할되지 않은 폴더는 연결 연산자를 사용할 수 없습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    if (folders.empty()) {
        // 검사할 폴더
        Folder unverifiedFolder = other;
        
        // 제외 대상 폴더가 삽입된 경우, -1 리턴
        if (std::find(images[0].excludeFoldersAndImages.begin(), images[0].excludeFoldersAndImages.end(), unverifiedFolder.path)!=images[0].excludeFoldersAndImages.end()) {
            return -1;
        }
        
        // 제외 대상 이미지가 삽입된 경우, 전부 제거
        std::vector<Image> newImages;
        for (auto& image : unverifiedFolder.images) {
            if (std::find(images[0].excludeFoldersAndImages.begin(), images[0].excludeFoldersAndImages.end(), image.path)==images[0].excludeFoldersAndImages.end()) {
                newImages.push_back(image);
            }
        }
        unverifiedFolder.images = newImages;
        
        // other에 이미지가 비어있으면, -1 리턴
        if (unverifiedFolder.images.empty()) {
            return -1;
        }
        
        // this의 제외 조건 other에게 상속 시키기
        for (auto& otherImage : unverifiedFolder.images) {
            // 제외 조건 집합
            std::set<std::filesystem::path> cascadedExcludeList;
            // 제외 조건 합치기
            for (auto& p : otherImage.excludeFoldersAndImages) {
                cascadedExcludeList.insert(p);
            }
            for (auto& p : this->images[0].excludeFoldersAndImages) {
                cascadedExcludeList.insert(p);
            }
            // 합쳐진 제외 조건 저장
            otherImage.excludeFoldersAndImages = std::vector<std::filesystem::path>(cascadedExcludeList.begin(), cascadedExcludeList.end());
        }
        
        // 폴더 연결
        folders.push_back(unverifiedFolder);
        return 0;
    }
    else if (folders.size() > 1) {
        std::string errorMessage = "MSIG::Algorithm::Folder.operator() : 하위 폴더가 하나인 폴더에만 새로운 폴더를 연결할 수 있습니다.";
        throw std::runtime_error(errorMessage);
    }
    else {
        return folders[0](other);
    }
}

std::vector<Folder>
Folder::__split() {
    namespace fs = std::filesystem;
    
    // *.
    this->splited = true;
    
    // 1. 하위 폴더 먼저 __split()
    std::vector<Folder> childFolders;
    for (auto& folder : folders) {
        std::vector<Folder> splitedFolder = folder.__split();
        childFolders.insert(childFolders.end(), splitedFolder.begin(), splitedFolder.end());
    }
    
    // 2. 폴더 분할
    // 2-1. 분할 그룹 조사
    std::set<std::vector<std::filesystem::path>> setForExclude;             // 제외 정보
    std::set<std::vector<std::filesystem::path>> setForMultipleSelection;   // 다중 선택 정보
    for (auto& image : images) {
        setForExclude.insert(image.excludeFoldersAndImages);
        setForMultipleSelection.insert(image.multipleSelectionFolders);
    }
    // 2-2. 분할 그룹에 따라 분할
    std::vector<Folder> splitedFolders;
    std::vector<std::vector<std::filesystem::path>> vectorForExclude(setForExclude.begin(), setForExclude.end());
    std::vector<std::vector<std::filesystem::path>> vectorForMultipleSelection(setForMultipleSelection.begin(), setForMultipleSelection.end());
    for (auto& v1 : vectorForExclude)
    for (auto& v2 : vectorForMultipleSelection)
    {
        // 현재 폴더 복사
        Folder newFolder(*this, false, false);
        
        // 새로 만든 폴더에 하위 폴더들 추가
        newFolder.folders = childFolders;
        
        // 새로 만들 폴더에 이미지들 추가
        for (auto& image : images)
        if (image.excludeFoldersAndImages==v1 && image.multipleSelectionFolders==v2)
        {
            newFolder.images.push_back(image);
        }
        
        // 생성된 폴더에 이미지가 존재하면 저장
        if (!newFolder.images.empty())
            splitedFolders.push_back(newFolder);
    }
    
    // 3. 분할된 폴더 구조 검사
    // 3-1. 다중 선택 폴더에 저장 되어있는 폴더들을 "단일 선택 폴더"와, "다중 선택 폴더"로 나누어 분리 저장
    // test* ─┬─ a* ─┬─ e
    //        │      ├─ f
    //        │      └─ g
    //        └─ b*
    // - - - - - - - - - - -
    // test* ─┬─ a ─┬─ e
    //        │     ├─ f
    //        │     └─ g
    //        ├─ a* ─┬─ e
    //        │      ├─ f
    //        │      └─ g
    //        ├─ b
    //        └─ b*
    std::vector<Folder> splitedFolders2;
    for (auto& newFolder : splitedFolders)
    {
        // 3-1-1. newFolder.images의 다중 선택 항목들 조사
        std::set<std::filesystem::path> multipleSelectionList;
        for (auto& image : newFolder.images)
        for (auto& p : image.multipleSelectionFolders) {
            multipleSelectionList.insert(p);
        }
        
        // 3-1-2. newFolder.folders의 폴더들을 "단일 선택 폴더"와 "다중 선택 폴더"로 분리
        Folder singleSelectionFolder(newFolder, false, true);
        Folder multipleSelectionFolder(newFolder, false, true);
        for (auto& folder : newFolder.folders)
        {
            if (multipleSelectionList.find(folder.path)!=multipleSelectionList.end())
                multipleSelectionFolder.folders.push_back(folder);
            else
                singleSelectionFolder.folders.push_back(folder);
        }
        
        // 3-1-3. "단일 선택 폴더"에 저장된 이미지들의 다중 선택 데이터 초기화
        for (auto& image : singleSelectionFolder.images) {
            image.multipleSelectionFolders.clear();
        }
        
        // 3-1-4. newFolder 에서 분리된 "단일 선택 폴더"들과 "다중 선택 폴더"들 저장
        if (singleSelectionFolder.folders.empty() ||
            multipleSelectionFolder.folders.empty())
        {
            // newFolder는 분리되지 않았으므로 그냥 저장
            splitedFolders2.push_back(newFolder);
        }
        else
        {
            // 분리된 singleSelectionFolder와 multipleSelectionFolder 저장
            splitedFolders2.push_back(singleSelectionFolder);
            splitedFolders2.push_back(multipleSelectionFolder);
        }
    }
    // 3-2. "다중 선택 폴더" 내부에 "다중 선택으로 인해 분할된 폴더=(동일한 path를 가지는 폴더)"가 존재하면 따로 분리 보관
    // test* ─┬─ a ─┬─ e
    //        │     ├─ f
    //        │     └─ g
    //        ├─ a* ─┬─ e
    //        │      ├─ f
    //        │      └─ g
    //        ├─ b
    //        └─ b*
    // - - - - - - - - - - -
    // test* ─┬─ a ─┬─ e
    //        │     ├─ f
    //        │     └─ g
    //        └─ b
    // test* ─┬─ a ─┬─ e
    //        │     ├─ f
    //        │     └─ g
    //        └─ b*
    // test* ─┬─ a* ─┬─ e
    //        │      ├─ f
    //        │      └─ g
    //        └─ b
    // test* ─┬─ a* ─┬─ e
    //        │      ├─ f
    //        │      └─ g
    //        └─ b*
    std::vector<Folder> splitedFolders3;
    for (auto& newFolder : splitedFolders2)
    {
        // 3-2-1. newFolder가 "다중 선택 폴더"가 아니면 건너뜀.
        if (newFolder.images[0].multipleSelectionFolders.empty()) {
            splitedFolders3.push_back(newFolder);
            continue;
        }
        
        // 3-2-2. newFolder.folders를 path를 기준으로 분리
        // {fa, fa, fa*, fb, fb, fb*} -> a={fa, fa, fa*}, b={fb, fb, fb*}
        std::set<std::filesystem::path> paths;
        for (auto& folder : newFolder.folders) {
            paths.insert(folder.path);
        }
        std::map<std::filesystem::path, std::vector<Folder>> pairs;
        for (auto& p : paths)
        for (auto& folder : newFolder.folders) {
            if (folder.path == p)
                pairs[p].push_back(folder);
        }
        
        // 3-2-3. 나올 수 있는 폴더 조합 수 모두 계산
        // a={fa, fa, fa*}, b={fb, fb, fb*} -> 3 * 3 -> 9
        size_t combinationCount = 1;
        for (auto& [p, vF] : pairs)
            combinationCount = combinationCount * vF.size();
        
        // 3-2-4. 폴더들을 조합하여 newFolder들 생성
        for (size_t i=0; i<combinationCount; i++) {
            Folder folder(newFolder, false, true);
            for (auto& [p, vF] : pairs) {
                folder.folders.push_back(vF[i % vF.size()]);
            }
            splitedFolders3.push_back(folder);
        }
    }
    // 3-3. 제외 대상 제거
    for (auto& newFolder : splitedFolders3)
    {
        newFolder.__pruning(false, true);
    }
    
    // 4. 완성된 분할된 폴더 반환.
    return splitedFolders3;
}

void
Folder::__stretch() {
    namespace fs = std::filesystem;
    
    // *. __split() 작업이 이루어지지 않은 폴더는 __stretch() 작업이 이루어지면 안됨.
    if (this->splited == false)
    {
        std::string errorMessage = "MSIG::Algorithm::Folder.__stretch() : 먼저 __split()가 먼저 선행되어야 합니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // *.
    this->stretched = true;
    
    // 1. 하위 폴더가 존재하지 않는 경우
    if (this->folders.empty()) {
        return;
    }
    
    // 2. 하위 폴더들 먼저 stretch
    for (auto& folder : this->folders)
        folder.__stretch();
    
    // 3. 중복 선택 폴더인 경우 stretch 수행
    if (!(this->images[0].multipleSelectionFolders.empty()))
    {
        // 3-1. 가능한 모든 폴더 조합 갯수 구하기
        // (a,b,c) <- (a), (b), (c),
        //            (a,b), (b,c), (a,c), (b, a), (c, a), (c, b)
        //            (a,b,c), (a,c,b), (b,a,c), (b,c,a), (c,a,b), (c,b,a)
        std::vector<std::vector<unsigned char>> combinations;
        this->__calculate_all_combination(combinations, (unsigned char)this->folders.size());
        
        // 3-2. 뽑힌 조합대로 폴더들 새로 구성
        std::vector<Folder> stretchedFolders;
        for (auto& combination : combinations) {
            // 첫 번째 폴더 생성
            Folder pickedFolder(this->folders[combination[0]], true, true);
            // 폴더들 연결
            int wrong = 0;
            for (size_t i=1; i<combination.size(); i++) {
                wrong = pickedFolder(this->folders[combination[i]]);
                // 잘못 연결 확인
                if (wrong) {
                    break;
                }
            }
            // 잘못 연결(제외조건 위반 연결)되면 이번 연결은 건너뛰기
            if (wrong) {
                continue;
            }
            // 생성된 목록에 추가
            stretchedFolders.push_back(pickedFolder);
        }
        
        // 3-3. 제외 대상 제거
        for (auto& folder : stretchedFolders) {
            folder.__pruning(true, true);
        }
        
        // 3-4. stretched 된 폴더들 생성 확률 낮추기
        for (auto& folder : stretchedFolders) {
            for (auto& image : folder.images) {
                image.selectionProbability *= 1.0 / stretchedFolders.size();// * this->folders.size();    // (1 / streched 된 폴더들 갯수) * streched 되기 전 원래 폴더 갯수
            }
        }
        
        // 3-5. 기존 folders를 stretchedFolders로 교체
        this->folders = stretchedFolders;
    }
}

void
Folder::__pruning(bool recursive, bool keepImageData) {
    namespace fs = std::filesystem;
    
    // 현재 폴더에 있는 모든 이미지들의 제외 항목들 제거하기
    
    // 1. 현재 이미지들의 제외 항목 조사
    std::set<std::filesystem::path> excludeList;
    for (auto& image : images)
    for (auto& p : image.excludeFoldersAndImages) {
        excludeList.insert(p);
    }
    
    // 2. 제외 항목에 해당하는 folders들 전부 제거
    folders.erase(std::remove_if(folders.begin(), folders.end(), [&excludeList](const Folder& folder){return excludeList.find(folder.path)!=excludeList.end();}), folders.end());
    
    // 3. 제외 항목에 해당하는 folders.images들 전부 제거
    for (auto& folder : folders) {
        folder.images.erase(std::remove_if(folder.images.begin(), folder.images.end(), [&excludeList](const Image& image){return excludeList.find(image.path)!=excludeList.end();}), folder.images.end());
    }
    
    // 4. 제거 후 이미지가 존재하지 않는 folders들 전부 제거
    folders.erase(std::remove_if(folders.begin(), folders.end(), [](const Folder& folder){return folder.images.empty();}), folders.end());
    
    // 5. images의 제외 항목 데이터 초기화
    if (!keepImageData) {
        for (auto& image : images) {
            image.excludeFoldersAndImages.clear();
        }
    }
    
    // 6. 하위 폴더의 있는 모든 이미지들의 제외 항목들 제거하기
    if (recursive) {
        for (auto& folder : folders) {
            folder.__pruning(recursive, keepImageData);
        }
    }
}

void
Folder::__deleting_rules(bool recursive, bool excludeList, bool multipleSelectionList) {
    namespace fs = std::filesystem;
    
    // 하위 폴더의 있는 모든 이미지들의 제외, 다중 선택 정보 지우기
    if (recursive) {
        for (auto& folder : folders) {
            folder.__deleting_rules(recursive, excludeList, multipleSelectionList);
        }
    }
    
    // 현재 폴더에 있는 모든 이미지들의 제외, 다중 선택 정보 지우기
    for (auto& image : images) {
        if (excludeList) {
            image.excludeFoldersAndImages.clear();
        }
        if (multipleSelectionList) {
            image.multipleSelectionFolders.clear();
        }
    }
}

void
Folder::__calculate_all_combination(std::vector<std::vector<unsigned char>>& combinations, unsigned char numberOfThings) {
    // 뽑혀야 하는 대상들의 인덱스 번호
    std::vector<unsigned char> vec;
    for (unsigned char i=0; i<numberOfThings; i++)
        vec.push_back(i);
    
    // 조합 계산
    for (size_t r = 1; r <= vec.size(); r++)
    {
        // bit vector 생성
        std::vector<bool> bitmask(r, true);
        bitmask.resize(vec.size(), false); // {true * r, false * vec.size()}
        do {
            // bitmask를 통해 조합 추출
            std::vector<unsigned char> combination;
            for (size_t i=0; i < vec.size(); i++) {
                if (bitmask[i])
                    combination.push_back(vec[i]);
            }
            // 조합에 대한 순열 계산
            do {
                // 현재 조합의 순열 저장
                combinations.push_back(combination);
            }
            while (std::next_permutation(combination.begin(), combination.end())); // 순열을 계산함
        }
        while (std::prev_permutation(bitmask.begin(), bitmask.end())); // 다음 조합을 구함
    }
}

void
Folder::__save_rule(bool recursive) {
    namespace fs = std::filesystem;
    
    // *. 추상화된 폴더는 규칙 저장 하면 안됨.
    if (this->splited == true | this->stretched == true)
    {
        std::string errorMessage = "MSIG::Algorithm::Folder.__save_rule() : 논리적 재구성이 이루어진 폴더는 규칙을 저장할 수 없습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 1. 현재 폴더의 __rule__.txt 삭제.
    std::fstream((this->path/fs::path("__rule__.txt")).string(), std::ios::out).close();
    
    // 2. 현재 폴더의 __rule__.txt 내용 생성.
    std::vector<std::string>    rule;
    std::set<std::string>       ruleGroup;
    for (auto& image : images)
    {
        std::string data, tmp;
        
        // 2-1. 그룹 또는 이미지 정보 생성
        if (image.groupName.empty()) data += image.path.filename().string() + "=";
        else                         data += image.groupName + "=";
        for (auto& p : image.excludeFoldersAndImages)
        {
            if (tmp.empty()) tmp += (fs::is_directory(p) ? p.filename().string() : p.parent_path().filename().string() + "_" + p.filename().string());
            else             tmp += "," + (fs::is_directory(p) ? p.filename().string() : p.parent_path().filename().string() + "_" + p.filename().string());
        }
        data += tmp + "~";
        tmp = "";
        for (auto& p : image.multipleSelectionFolders)
        {
            if (tmp.empty()) tmp += p.filename().string();
            else             tmp += "," + p.filename().string();
        }
        data += tmp + "\n";
        
        // 2-2. 정보 보관
        if (image.groupName.empty()) {
            // 이미지 정보
            rule.push_back(data);
        }
        else {
            // 그룹과 이미지 정보
            ruleGroup.insert(data);
            rule.push_back(image.path.filename().string() + "=" + image.groupName + "\n");
        }
    }
    
    // 3. 생성된 __rule__.txt 내용 합치기
    std::string ruleTXT;
    ruleTXT += "# group\n";
    for (auto& s : ruleGroup) {
        ruleTXT += s;
    }
    ruleTXT += "\n";
    ruleTXT += "# rule\n";
    for (auto& s : rule) {
        ruleTXT += s;
    }
    
    // 4. 파일로 저장
    Utility::attach(this->path/fs::path("__rule__.txt"), ruleTXT);
    
    // *. 하위 폴더의 __rule__.txt 새로 쓰기.
    if (recursive)
    {
        for (auto& folder : folders) {
            folder.__save_rule(true);
        }
    }
}

void
Folder::reconstruction(std::deque<Folder>& reconstructionFolders) {
    namespace fs = std::filesystem;
    
    // 1. 논리적 분할 -> 논리적 펼치기 -> 반환
    for (auto& splitedFolder : this->__split()) {
        splitedFolder.__stretch();
        splitedFolder.__deleting_rules(true, true, true);
        reconstructionFolders.push_back(splitedFolder);
    }
}

Folder
Folder::peek() {
    namespace fs = std::filesystem;
    
    if (!this->folders.empty()) {
        Folder thisFolder(*this, false, true);
        thisFolder.folders.push_back(this->folders[0].peek());
        return thisFolder;
    }
    else {
        return Folder(*this, false, true);
    }
}

bool
Folder::pop() {
    namespace fs = std::filesystem;
    
    // 현재 폴더가 마지막 폴더가 아닌 경우
    if (!this->folders.empty()) {
        // 첫번째 하위 폴더 pop() 결과 저장
        bool left = this->folders[0].pop();
        if (left)
        {
            return true;
        }
        // 첫번째 하위 폴더가 마지막인경우
        else
        {
            // 첫번째 하위 폴더 지우기
            this->folders.erase(this->folders.begin());
            // 현재 폴더가 마지막인 경우
            if (this->folders.size()==0)
            {
                return false;
            }
            // 현재 폴더가 마지막이 아닌 경우
            else
            {
                return true;
            }
        }
    }
    // 현재 폴더가 마지막인 경우
    else {
        return false;
    }
}

void
Folder::tree(const std::string& prefix, size_t order) {
    namespace fs = std::filesystem;
    
    /*
     디렉토리 구조를 출력할때 폴더 뒤에 붙는 '^', '*' 문자에 대한 설명.
     
     '^' : 현재 폴더의 이미지들중 제외 정보를 가지고 있는 이미지가 존재함을 의미
     '*' : 현재 폴더의 이미지들중 다중 선택 정보를 가지고 있는 이미지가 존재함을 의미
     */
    
    // 1-1. 연결문 생성
    std::string currentPrefix = "";
    std::string nextPrefix = "";
    switch (order) {
        /*
         0x10000 : ─── : 수평선
         0x01000 :  │  : 수직선
         0x00100 : ─┬─ : 시작
         0x00010 :  ├─ : 가운데
         0x00001 :  └─ : 마지막
         0x00000 :     : 공백
         */
        case 0x10000: currentPrefix += " ───"; nextPrefix += "    "; break;
        case 0x01000: currentPrefix += "  │ "; nextPrefix += "  │ "; break;
        case 0x00100: currentPrefix += " ─┬─"; nextPrefix += "  │ "; break;
        case 0x00010: currentPrefix += "  ├─"; nextPrefix += "  │ "; break;
        case 0x00001: currentPrefix += "  └─"; nextPrefix += "    "; break;
        case 0x00000: currentPrefix += "";     nextPrefix += "";     break;
    }
    
    // 1-2. 폴더 이름 생성
    std::string folderName = " " + this->path.filename().string();
    for (auto& image : images) {
        if (!image.excludeFoldersAndImages.empty()) {
            folderName += "^";
            break;
        }
    }
    for (auto& image : images) {
        if (!image.multipleSelectionFolders.empty()) {
            folderName += "*";
            break;
        }
    }
    currentPrefix += folderName;
    for (size_t i=0; i<folderName.size(); i++)
        nextPrefix += " ";
    
    // 1-2. currentPrefix 출력
    std::cout << currentPrefix;
    
    // 2. 재귀적으로 하위 폴더 출력문 출력
    if (folders.size() < 1)
    {
        // 최하위 폴더 도달시 줄바꿈
        std::cout << "\n";
        return;
    }
    else if (folders.size() == 1)
    {
        //
        folders[0].tree(prefix + nextPrefix, 0x10000);
        return;
    }
    else
    {
        //
        for (size_t i=0; i<this->folders.size(); i++)
        {
            if (i==0) {
                folders[i].tree(prefix + nextPrefix, 0x00100);
                std::cout << prefix + nextPrefix;
            }
            else if (i==this->folders.size()-1) {
                folders[i].tree(prefix + nextPrefix, 0x00001);
            }
            else {
                folders[i].tree(prefix + nextPrefix, 0x00010);
                std::cout << prefix + nextPrefix;
            }
        }
    }
}

void
Folder::get_all_images_name(std::set<std::string>& allImagesNames) {
    namespace fs = std::filesystem;
    
    // 현재 폴더의 모든 이미지 이름 삽입
    for (auto& image : images) {
        allImagesNames.insert(image.path.filename().stem().string());
    }
    
    // 하위 폴더의 모든 이미지 이름 조사
    for (auto& folder : folders) {
        folder.get_all_images_name(allImagesNames);
    }
}

}
}
