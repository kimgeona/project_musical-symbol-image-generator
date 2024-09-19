
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
    if (!fs::exists(folderPath / fs::path("__rule__.txt")) || !fs::is_regular_file(folderPath / fs::path("__rule__.txt"))) {
        std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + folderPath.string() + "\"에 __rule__.txt 파일이 존재하지 않습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 4. 존재하는 이미지 조사
    for (auto& it : fs::directory_iterator(folderPath)) {
        // TODO: 이미지 확장자를 .png 말고 다른 이미지 파일로도 받을지 고민해보기.
        if (fs::is_regular_file(it.path()) && it.path().filename().extension().string()==".png")
            images.emplace_back(it.path());
    }
    
    // 5. 존재하는 폴더 조사
    for (auto& it : fs::directory_iterator(folderPath)) {
        if (fs::is_directory(it.path()))
            folders.emplace_back(it.path());
    }
    
    // 6. 빈 폴더인지 확인
    if (images.empty()) {
        std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + folderPath.string() + "\"는 비어있는 폴더이므로 데이터셋으로 구성할 수 없습니다.";
        throw std::runtime_error(errorMessage);
    }
    
    // 7. 존재하는 이미지의 실행 확률 계산
    std::map<std::string, double> groupCount;
    for (auto& image : images) {
        // 그룹이 지정된 이미지들만 갯수 카운트
        if (!image.groupName.empty())
            // FIXME: 부동 소수점 끼리의 연산은 정확하지 않을 수 있음. (ex: 1.0 + 1.0 -> 2.000000000000001)
            groupCount[image.groupName] += 1.0;
    }
    for (auto& [groupName, count] : groupCount) {
        count = 1.0 / count * declineRate;
    }
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
    for (auto& image : images)
    {
        // 두 벡터의 교집합 검사를 수행
        std::set<std::filesystem::path> excludeList;
        std::set<std::filesystem::path> multipleSelectionList;
        
        for (auto& p : image.excludeFoldersAndImages) {
            if (fs::is_directory(p))
                excludeList.insert(p);
        }
        for (auto& p : image.multipleSelectionFolders) {
            multipleSelectionList.insert(p);
        }
        
        std::set<std::filesystem::path> intersection;
        std::set_intersection(excludeList.begin(), excludeList.end(), multipleSelectionList.begin(), multipleSelectionList.end(), std::inserter(intersection, intersection.begin()));
        
        if (!intersection.empty())
        {
            std::string errorMessage = "MSIG::Algorithm::Folder.Folder() : \"" + image.path.string() + "\"의 \"제외 항목(폴더)\"과 \"다중 선택 항목(폴더)\"은 서로 겹치지 않아야 합니다.";
            throw std::runtime_error(errorMessage);
        }
    }
    
    // 9. 폴더 정렬
    std::sort(folders.begin(), folders.end());
    
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

bool
Folder::operator<(const Folder& other) const {
    return this->path < other.path;
}

void
Folder::operator()(const Folder& other) {
    if (folders.empty()) {
        folders.push_back(other);
    }
    else if (folders.size() > 1) {
        std::string errorMessage = "MSIG::Algorithm::Folder.operator() : 일렬로 연결된 폴더에만 새로운 폴더를 추가할 수 있습니다.";
        throw std::runtime_error(errorMessage);
    }
    else {
        folders[0](other);
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
    
    // 3. 분할된 폴더 구조 체크
    // 3-1. 제외 대상 제거, 제외 정보 제거
    for (auto& newFolder : splitedFolders)
    {
        // 3-1-1. newFolder.images의 제외 항목들 조사
        std::set<std::filesystem::path> excludeList;
        for (auto& image : newFolder.images)
        for (auto& p : image.excludeFoldersAndImages) {
            excludeList.insert(p);
        }
        
        // 3-1-2. 제외 항목에 해당하는 newFolder의 폴더들 전부 제거
        newFolder.folders.erase(std::remove_if(newFolder.folders.begin(), newFolder.folders.end(), [&excludeList](const Folder& folder){return excludeList.find(folder.path)!=excludeList.end();}), newFolder.folders.end());
        
        // 3-1-3. 제외 항목에 해당하는 newFolder.folders들의 이미지들 전부 제거
        for (auto& folder : newFolder.folders) {
            folder.images.erase(std::remove_if(folder.images.begin(), folder.images.end(), [&excludeList](const Image& image){return excludeList.find(image.path)!=excludeList.end();}), folder.images.end());
        }
        
        // 3-1-4. 이미지가 존재하지 않는 newFolder의 폴더들 전부 제거
        newFolder.folders.erase(std::remove_if(newFolder.folders.begin(), newFolder.folders.end(), [](const Folder& folder){return folder.images.empty();}), newFolder.folders.end());
        
        // 3-1-5. newFolder.images의 제외 항목 데이터 초기화
        for (auto& image : newFolder.images) {
            image.excludeFoldersAndImages.clear();
        }
    }
    
    // 3-2. 다중 선택 폴더에 저장 되어있는 폴더들을 "단일 선택 폴더"와, "다중 선택 폴더"로 나누어 분리 저장
    std::vector<Folder> splitedFolders2;
    for (auto& newFolder : splitedFolders)
    {
        // 3-2-1. newFolder.images의 다중 선택 항목들 조사
        std::set<std::filesystem::path> multipleSelectionList;
        for (auto& image : newFolder.images)
        for (auto& p : image.multipleSelectionFolders) {
            multipleSelectionList.insert(p);
        }
        
        // 3-2-2. newFolder.folders의 폴더들을 "단일 선택 폴더"와 "다중 선택 폴더"로 분리
        Folder singleSelectionFolder(newFolder, false, true);
        Folder multipleSelectionFolder(newFolder, false, true);
        for (auto& folder : newFolder.folders)
        {
            if (multipleSelectionList.find(folder.path)!=multipleSelectionList.end())
                multipleSelectionFolder.folders.push_back(folder);
            else
                singleSelectionFolder.folders.push_back(folder);
        }
        
        // 3-2-3. "단일 선택 폴더"에 저장된 이미지들의 다중 선택 데이터 초기화
        for (auto& image : singleSelectionFolder.images) {
            image.multipleSelectionFolders.clear();
        }
        
        // 3-2-4. newFolder 에서 분리된 "단일 선택 폴더"들과 "다중 선택 폴더"들 저장
        if (singleSelectionFolder.folders.empty() || multipleSelectionFolder.folders.empty()) {
            splitedFolders2.push_back(newFolder);
        }
        else {
            splitedFolders2.push_back(singleSelectionFolder);
            splitedFolders2.push_back(multipleSelectionFolder);
        }
    }
    
    // 3-3. "다중 선택 폴더" 내부에 "다중 선택으로 인해 분할된 폴더=(동일한 path를 가지는 폴더)"가 존재하면 따로 분리 보관
    std::vector<Folder> splitedFolders3;
    for (auto& newFolder : splitedFolders2)
    {
        // 3-1. newFolder가 "다중 선택 폴더"가 아니면 건너뜀.
        if (newFolder.images[0].multipleSelectionFolders.empty()) {
            splitedFolders3.push_back(newFolder);
            continue;
        }
        
        // 3-2. newFolder.folders를 path를 기준으로 분리
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
        
        // 3-3. 나올 수 있는 폴더 조합 수 모두 계산
        // a={fa, fa, fa*}, b={fb, fb, fb*} -> 3 * 3 -> 9
        size_t combinationCount = 1;
        for (auto& [p, vF] : pairs)
            combinationCount = combinationCount * vF.size();
        
        // 3-4. 폴더들을 조합하여 newFolder들 생성
        for (size_t i=0; i<combinationCount; i++) {
            Folder folder(newFolder, false, true);
            for (auto& [p, vF] : pairs) {
                folder.folders.push_back(vF[i % vF.size()]);
            }
            splitedFolders3.push_back(folder);
        }
    }
    
    // 4. 완성된 분할된 폴더 반환.
    return splitedFolders3;
}

std::vector<Folder>
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
        // 현재 폴더만 반환
        return std::vector<Folder>({Folder(*this, false, true)});
    }
    
    // 2. 단일 선택, 중복 선택에 따라 다르게 진행
    std::vector<Folder> stretchedFolders;
    if (this->images[0].multipleSelectionFolders.empty())
    {
        // 2-1-1. 단일 선택은 하위 폴더와 현재 폴더를 1대1로 연결
        for (auto& folder : this->folders)
        for (auto& vF : folder.__stretch()) {
            stretchedFolders.emplace_back(*this, false, true);
            stretchedFolders[stretchedFolders.size()-1].folders.push_back(vF);
        }
        
        // 2-1-2. 자기 폴더만 추가 (하위 폴더가 없는것 취급)
        //stretchedFolders.emplace_back(*this, false, true);
    }
    else
    {
        // 2-2. 다중 선택은 하위 폴더들 중복해서 연결
        // 2-2-1. 하위 폴더에서 모든 폴더 조합 뽑기
        std::vector<std::vector<Folder>> vvF;
        for (auto& folder : this->folders)
            vvF.push_back(folder.__stretch());
        
        // 2-2-2. 가능한 모든 폴더 조합 갯수 구하기
        // {a,b,c} -> (a), (b), (c),
        //            (a,b), (b,c), (a,c), (b, a), (c, a), (c, b)
        //            (a,b,c), (a,c,b), (b,a,c), (b,c,a), (c,a,b), (c,b,a)
        std::vector<std::vector<unsigned char>> combinations;
        this->__calculate_all_combination(combinations, (unsigned char)this->folders.size());
        
        // 2-2-3. 하위 폴더와 현재 폴더들을 연결
        // (a,b,c) <- [a), (b), (c), (a,b), (b,c), ..., (a,b,c), (a,c,b), ..., (c,b,a)]
        for (auto& combination : combinations) {
            /*
            // 현재 조합을 수행할 폴더들 추출
            std::vector<std::vector<Folder>> vvFSelected;
            for (auto& index : combination) {
                vvFSelected.push_back(vvF[index]);
            }
            */
            // 현재 폴더 조합을 몇 번 반복할 것인지 계산
            // (a,b,c) * ?
            size_t count = 1;
            for (auto& index : combination) {
                count = count * vvF[index].size();
            }
            // 현재 폴더 조합에서 반복 시작
            // (a,b,c), (a,b,c), ..., (a,b,c)
            for (size_t i=0; i<count; i++)
            {
                // 현재 폴더 복사본 생성
                Folder newFolder(*this, false, true);
                size_t discount = 1;
                
                // 하위 폴더 뽑기
                // (a1, b1, c1) <--(pick_by_index)-- (a{a1,a2,...}, b{b1,b2,...}, c{c1,c2,...})
                for (size_t j=0; j<combination.size(); j++)
                {
                    if (j>0) {
                        discount = discount * vvF[combination[j-1]].size();
                    }
                    
                    // 폴더 연결
                    newFolder(Folder(vvF[combination[j]][(i/discount) % vvF[combination[j]].size()], true, true));
                }
                
                // 생성된 목록에 추가
                stretchedFolders.push_back(newFolder);
            }
        }
    }
    
    // 3. 반환
    return stretchedFolders;
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
    
    // 1. 논리적 분할 -> 논리적 재구성 -> 반환
    for (auto& splitedFolder : this->__split())
    for (auto& stretchedFolder : splitedFolder.__stretch()) {
        reconstructionFolders.push_back(stretchedFolder);
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
        std::cout << std::endl;
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
