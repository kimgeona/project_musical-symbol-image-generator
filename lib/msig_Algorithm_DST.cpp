#include <msig_Algorithm_DST.hpp>

namespace msig
{


// ==== DSTFile ==================================

// 생성자
DSTFile::DSTFile(const std::string& dir) : DSTFile(std::filesystem::path(dir)) {}
DSTFile::DSTFile(const std::filesystem::path& dir)
{
    using namespace std::filesystem;
    
    // 빈 경로인지, 존재하는 파일인지 확인
    if (dir != path() && exists(dir) && is_regular_file(dir))
    {
        this->dir = dir;
        this->state = true;
    }
    else std::runtime_error("msig::DSTFile::DSTFile() : 비어있는 경로이거나 존재하지 않는 파일로 DSTFile 객체를 생성할 수 없습니다.");
}

// bool() 연산자
DSTFile::operator bool() const
{
    return state;
}

// 상태 설정 함수
void DSTFile::set_true()
{
    this->state = true;
}
void DSTFile::set_false()
{
    this->state = false;
}

// 정보 가져오기
std::filesystem::path DSTFile::get_filename()
{
    return dir.filename();
}
std::filesystem::path DSTFile::get_parent(bool full_path)
{
    if (full_path)  return dir.parent_path();               // 부모 폴더 전체 경로
    else            return dir.parent_path().filename();    // 부모 폴더 이름만
}
std::filesystem::path DSTFile::get_dir()
{
    return dir;
}


// ==== DSTFolder ==================================

// 생성자
DSTFolder::DSTFolder(const std::string& dir, const std::vector<std::string>& folder_types, const std::vector<std::string>& file_types) : DSTFolder(std::filesystem::path(dir), folder_types, file_types) {}
DSTFolder::DSTFolder(const std::filesystem::path& dir, const std::vector<std::string>& folder_types, const std::vector<std::string>& file_types)
{
    using namespace std::filesystem;
    
    // 빈 경로인지, 존재하는지, 폴더인지 확인
    if (dir != path() && exists(dir) && is_directory(dir))
    {
        // 폴더 주소 저장
        this->dir = dir;
        
        // 폴더 타입 저장
        if (dir.filename().string().find("-#")!=std::string::npos)  duplication = true;
        else                                                        duplication = false;
        
        // 중복 갯수 카운트
        duplication_count = -1;
        
        // 현재 디렉토리내 폴더들과 파일 조사 후 추가
        for (auto& e : directory_iterator(dir))
        {
            // 디렉토리인 경우
            if (is_directory(e.path()))
            {
                // 대상 폴더 이름인지 확인
                for (auto& s : folder_types) if (e.path().filename().string().find(s)!=std::string::npos)
                {
                    // folders에 추가
                    folders.emplace_back(e.path(), folder_types, file_types);
                    
                    // for 반복문 종료
                    break;
                }
                continue;
            }
            
            // 파일인 경우
            if (is_regular_file(e.path()))
            {
                // 대상 파일 확장자인지 확인
                if (std::find(file_types.begin(), file_types.end(), e.path().filename().extension().string())!=file_types.end())
                {
                    // files에 추가
                    files.emplace_back(e.path());
                }
                continue;
            }
        }
    }
    else std::runtime_error("msig::DSTFolder::DSTFolder() : 비어있는 경로이거나 존재하지 않는 폴더로 DSTFolder 객체를 생성할 수 없습니다.");
}

// bool() 연산자
DSTFolder::operator bool() const
{
    // 현재 파일들중 사용가능한 파일이 하나라도 있으면 true 반환
    for (auto& file : files) if (file) return true;
    
    return false;
}

// 상태 설정 함수
void DSTFolder::set_true(bool recursive)
{
    if (recursive)
    {
        // 해당 폴더내 파일들 전부 true 상태로 만들기
        for (auto& f : files) f.set_true();
        
        // 해당 폴더내 폴더들 전부 true 상태로 만들기
        for (auto& f : folders) f.set_true(true);
    }
    else
    {
        // 해당 폴더내 파일들 전부 true 상태로 만들기
        for (auto& f : files) f.set_true();
    }
}
void DSTFolder::set_false(bool recursive)
{
    if (recursive)
    {
        // 해당 폴더내 파일들 전부 false 상태로 만들기
        for (auto& f : files) f.set_false();
        
        // 해당 폴더내 폴더들 전부 false 상태로 만들기
        for (auto& f : folders) f.set_false(true);
    }
    else
    {
        // 해당 폴더내 파일들 전부 false 상태로 만들기
        for (auto& f : files) f.set_false();
    }
}
void DSTFolder::set_true(std::filesystem::path dir)
{
    // 특정 파일 또는 폴더를 false 상태로 만들기
    if      (std::filesystem::is_directory(dir))    // dir이 폴더인 경우
    {
        // 해당 폴더 내 파일 전부 false로 만들기
        if (dir == this->dir) set_true(true);
    }
    else if (std::filesystem::is_regular_file(dir)) // dir이 파일인 경우
    {
        // 해당 폴더 내 파일에서 해당 파일만 false로 만들기
        for (auto& file : files) if (file.get_dir() == dir) file.set_true();
    }
    else
    {
        // 입력된 dir이 파일, 폴더를 나타내는 dir이 아닐 경우 경고문만 출력.
        std::cout << "msig::DSTFolder::set_true() 입력해주신 \"" << dir.string() << "\"가 파일 또는 디렉토리가 아니어서 건너뜁니다." << std::endl;
        return;
    }
    
    // 하위 폴더도 적용
    for (auto& folder : folders) folder.set_true(dir);
}
void DSTFolder::set_false(std::filesystem::path dir)
{
    // 특정 파일 또는 폴더를 false 상태로 만들기
    if      (std::filesystem::is_directory(dir))    // dir이 폴더인 경우
    {
        // 해당 폴더 내 파일 전부 false로 만들기
        if (dir == this->dir) set_false(true);
    }
    else if (std::filesystem::is_regular_file(dir)) // dir이 파일인 경우
    {
        // 해당 폴더 내 파일에서 해당 파일만 false로 만들기
        for (auto& file : files) if (file.get_dir() == dir) file.set_false();
    }
    else
    {
        // 입력된 dir이 파일, 폴더를 나타내는 dir이 아닐 경우 경고문만 출력.
        std::cout << "msig::DSTFolder::set_false() 입력해주신 \"" << dir.string() << "\"가 파일 또는 디렉토리가 아니어서 건너뜁니다." << std::endl;
        return;
    }
    
    // 하위 폴더도 적용
    for (auto& folder : folders) folder.set_false(dir);
}
void DSTFolder::set_duplication_count()
{
    // 중복 갯수 카운트
    duplication_count = 1;
    
    // 중복 갯수 카운트 계산 (=하위 폴더들 안 파일 갯수 총합)
    for (auto& folder : folders)
    {
        int count = folder.get_files_count();
        
        if (count == 0) std::runtime_error("msig::DSTFolder::set_duplication_count() : \"" + folder.dir.string() + "\" 폴더 안에 이미지들이 없습니다.");
        
        duplication_count *= (count+1); // 선택 안함의 경우의 수를 +1을 이용하여 추가
    }
    
    // 아무것도 선택 안하게되는 (맨 처음과 중복인 맨 마지막), (아무것도 선택 안함) 제거
    duplication_count -= 2;
}

// 정보 가져오기
bool DSTFolder::get_duplication()
{
    return duplication;
}
int  DSTFolder::get_files_count()
{
    return (int)files.size();
}
int  DSTFolder::get_folders_count()
{
    return (int)folders.size();
}

// 파일 이름 레이블 생성
std::set<std::string> DSTFolder::get_labels(bool recursive)
{
    // 레이블 집합
    std::set<std::string> labels;
    
    // 하위 폴더 레이블 조사
    if (recursive)
    {
        for (auto& folder : folders)
        {
            // 하위 폴더 레이블들
            std::set<std::string> pre_labels = folder.get_labels(recursive);
            
            // 현재 레이블 집합에 추가
            labels.insert(pre_labels.begin(), pre_labels.end());
        }
    }
    
    // 현재 폴더 레이블 조사
    for (auto& file : files)
    {
        // 파일 이름
        std::string label = file.get_filename().stem().string();
        
        // 파일 배치 정보 제거
        label = my_split(label, "~")[0];
        
        // 파일 추가 정보 제거
        //label = my_split(label, "-")[0];
        
        // 레이블 집합에 추가
        labels.insert(label);
    }
    
    return labels;
}

// 파일 하나 뽑기
std::vector<std::filesystem::path> DSTFolder::pick()
{
    using namespace std;
    using namespace std::filesystem;
    
    vector<path> vp;
    vector<path> vp_tmp;
    
    // 하위 폴더에서 파일 하나 뽑아 오기
    for (auto& folder : folders) if (folder)
    {
        // # 폴더일 경우
        if (folder.get_duplication())
        {
            // 중복 갯수 카운트 초기 설정
            if (duplication_count < 0) set_duplication_count();
            
            // 파일 뽑아서 뒤에 추가
            vp_tmp = pick(duplication_count); // 중복 조합 뽑기 진행
            vp.insert(vp.end(), vp_tmp.begin(), vp_tmp.end());
            break;
        }
        // @ 폴더일 경우
        else
        {
            // 파일 뽑아서 저장
            vp_tmp = folder.pick(); // 일반 뽑기 진행
            vp.insert(vp.end(), vp_tmp.begin(), vp_tmp.end());
            break;
        }
    }
    
    // 현재 폴더에서 파일 하나 뽑아 오기
    for (auto& file : files) if (file)
    {
        // 하위 폴더에서 받아온게 없다면
        if (vp.empty() && duplication_count < 0)
        {
            // 파일 뽑아서 뒤에 추가
            vp.push_back(file.get_dir());
            
            // 현재 파일 사용 불가능으로 만들기
            file.set_false();
            
            // 하위 폴더들 사용 가능으로 만들기
            for (auto& folder : folders) folder.set_true(true);
            
            // 반복문 종료
            break;
        }
        
        // 하위 폴더에서 받아온게 있다면
        else
        {
            // 파일 뽑아서 뒤에 추가
            vp.push_back(file.get_dir());
            
            // 반복문 종료
            break;
        }
    }
    
    // 뽑힌 파일 리턴
    return vp;
}
std::vector<std::filesystem::path> DSTFolder::pick(long n)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 뽑은 목록
    vector<path> vp;
    
    // 각 폴더안 n % files.size() 번째 요소의 주소를 vp에 추가.
    for (auto& folder : folders)
    {
        // folder에 저장되어있는 files에 접근할 인덱스 계산
        long index = n % (folder.files.size() + 1);
        
        // 다음 인덱스 계산을 위한 나눗셈
        n /= (folder.files.size() + 1);
        
        // 파일 선택 안함 인덱스라면 넘어가기
        if (index == folder.files.size()) continue;
        
        // 파일 주소 추가
        vp.push_back(folder.files[index].get_dir());
    }
    
    // 중복 뽑기 갯수 감소
    duplication_count--;
    
    // 중복 뽑기 갯수 완료하면 # 폴더들 비활성화
    if (duplication_count < 0) for (auto& folder : folders)
    {
        folder.set_false(true);
    }
    
    return vp;
}


// ==== DST ==================================

// 생성자
DST::DST(const std::string& dataset_dir) :
root(dataset_dir, {"-@", "-#"}, {".png", ".PNG"}),
gen(std::random_device{}()),
dis(0.0, 1.0) {}

// 처음 상태로 돌림
void DST::reset()
{
    root.set_true(true);
}

// 특정 악상기호 활성화
void DST::set_true(std::vector<std::filesystem::path> vp_folders)
{
    // 입력받은 파일 또는 폴더를 사용 가능한 상태로 만들기.
    for (auto& p : vp_folders) root.set_true(p);
}

// 특정 악상기호 비활성화
void DST::set_false(std::vector<std::filesystem::path> vp_folders)
{
    // 입력받은 파일 또는 폴더를 사용 불가능한 상태로 만들기.
    for (auto& p : vp_folders) root.set_false(p);
}

// 파일 이름들 레이블 가져오기
std::vector<std::string> DST::get_labels()
{
    // 파일 이름 레이블 생성
    std::set<std::string> labels = root.get_labels(true);
    
    // 파일 이름 레이블 벡터 반환
    return std::vector<std::string>(labels.begin(), labels.end());
}

// 가능한 모든 조합 구하기
std::queue<std::vector<std::filesystem::path>> DST::list(std::vector<double> rate)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 조합들 리스트
    queue<vector<path>> qvp;
    
    // 조합
    vector<path> vp;
    
    // 비율값 확인
    if (rate == std::vector<double>()) rate = std::vector<double>({1.0});
    
    // 비율 값
    double r = 1.0;
    
    // 총 뽑은 갯수
    long count=0;
    
    while (true)
    {
        // 선택 하나 뽑기
        vp = root.pick();
        count++;
        
        // 뽑힌 것이 없다면 종료
        if (vp.empty()) break;
        
        // 건너뛸 비율 선택하기
        if (vp.size() > rate.size()) r = rate[rate.size()-1];
        else                         r = rate[vp.size()-1];
        
        // 건너뛸 확률 뽑기
        if (dis(gen) > r) continue;
        
        // 거꾸로 뒤집기(재귀적으로 목록을 구했기에 뒤집어져 있음)
        std::reverse(vp.begin(), vp.end());
        
        // 리스트에 추가
        qvp.push(vp);
    }
    
    // 선택된 갯수 / 전체 갯수 출력
    std::cout << "전체 " << count << "개 중 " << qvp.size() << "개 선택됨." << std::endl;
    
    return qvp;
}


}
