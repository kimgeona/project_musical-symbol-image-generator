#include <msig_Algorithm_DST.hpp>

namespace msig {


// 생성자
DSTree::DSTree()
{
    using namespace std;
    using namespace std::filesystem;
    
    // 초기화
    this->root_dir  = path("");             // root 디렉토리
    this->target    = vector<string>();     // 확장자 목록
    this->nodes     = map<path, DSNode>();  // 파일 트리
    this->pre       = path("");             // 현재 위치
}
DSTree::DSTree(std::string root_dir, std::vector<std::string> target_extension)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 디렉토리 확인
    if (exists(path(root_dir)) && is_directory(path(root_dir)))
    {
        this->root_dir  = path(root_dir);    // 최상위 폴더 주소
        this->pre       = path(root_dir);    // 현재 노드 주소
        this->target    = target_extension;  // 대상 확장자 목록
    }
    else throw std::runtime_error("DSTree::DSTree() : 잘못된 주소 입니다.");
    
    // 디렉토리를 순회하며 파일들 nodes에 추가하기
    for (auto& d : recursive_directory_iterator(path(this->root_dir)))
    {
        // 대상 파일인지 확인
        if (check_file(d.path())) continue;
        
        // DSTree에 추가
        this->nodes[d.path()] = DSNode(d.path().string());
    }
    
    // nodes 추가된 내용 확인
    if (nodes.empty()) throw std::runtime_error("DSTree::DSTree() : 해당 디렉토리에서 불러올 파일들이 없습니다.");
}


// 연산자
bool DSTree::operator==(const DSTree& other) const
{
    return ((this->root_dir==other.root_dir) &&
            (this->target==other.target) &&
            (this->nodes==other.nodes) &&
            (this->pre==other.pre));
}


// 확인 함수
inline int DSTree::check_file(std::filesystem::path p, bool available_check)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 파일인지 확인
    if (!exists(p)) return -1;
    
    // 파일인지 확인
    if (!is_regular_file(p)) return -1;
    
    // 대상 확장자인지 확인
    if (find(target.begin(), target.end(), p.extension().string()) == target.end()) return -1;
    
    // -@ -# 문자 포함 되어 있는지
    if (p.parent_path().filename().string().find("-@")==string::npos &&
        p.parent_path().filename().string().find("-#")==string::npos) return -1;
    
    // 사용 가능 여부 확인
    if (available_check && (nodes[p].available==false)) return -1;
    
    return 0;
}
inline int DSTree::check_directory(std::filesystem::path p, bool available_check)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 폴더인지 확인
    if (!exists(p)) return -1;
    
    // 폴더인지 확인
    if (!is_directory(p)) return -1;
    
    // -@ -# 문자 포함 되어 있는지
    if (p.filename().string().find("-@")==string::npos &&
        p.filename().string().find("-#")==string::npos) return -1;
    
    // 사용 가능 여부 확인
    if (available_check && get_files(p).empty()) return -1;;
    
    return 0;
}


// 목록 구하기 함수
std::vector<std::filesystem::path>  DSTree::get_files(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 파일 목록
    vector<path> list_file;
    
    // 주소 확인
    if (!exists(dir) || !is_directory(dir))
        throw std::runtime_error("DSTree::get_files() : 주소가 잘못되었습니다.");
    
    // 주소 순회
    for (auto& d : directory_iterator(dir))
    {
        // 대상 파일인지 확인(사용 가능 여부 포함)
        if (check_file(d.path(), true)) continue;
        
        // 파일 추가
        list_file.push_back(d.path());
    }
    
    return list_file;
}
std::vector<std::filesystem::path>  DSTree::get_folders(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 디렉토리 목록
    vector<path> list_dir;
    
    // 주소 확인
    if (!exists(dir) || !is_directory(dir))
        throw std::runtime_error("DSTree::get_folders() : 주소가 잘못되었습니다.");
    
    // 주소 순회
    for (auto& d : directory_iterator(dir))
    {
        // 대상 폴더인지 확인(사용 가능 여부 포함)
        if (check_directory(d.path(), true)) continue;
        
        // 폴더 추가
        list_dir.push_back(d.path());
    }
    
    return list_dir;
}
std::vector<std::filesystem::path>  DSTree::get(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 파일 목록
    vector<path> list_file;
    
    // 주소 확인
    if (!exists(dir) || !is_directory(dir))
        throw std::runtime_error("DSTree::get_available() : 주소가 잘못되었습니다.");
    
    // 선택 가능한 파일들 구하기
    for (auto& d : get_folders(dir))
        for (auto& f : get_files(d))
            list_file.push_back(f);
    
    return list_file;
}


// 수정 함수
void                                DSTree::pruning(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (!exists(dir))
        throw std::runtime_error("DSTree::pruning() : 주소가 잘못되었습니다.");
    
    // 파일일 경우
    if (is_regular_file(dir))
    {
        // 대상 파일인지 확인(사용 가능 여부 포함)
        if (check_file(dir, true)) return;
        
        // 대상 파일 사용 가능 여부 비활성화
        nodes[dir].available = false;
        return;
    }
    
    // 폴더일 경우
    if (is_directory(dir))
    {
        // 현재 디렉토리에 있는 파일 순회
        for (auto& f : directory_iterator(dir))
        {
            // 대상 파일인지 확인(사용 가능 여부 포함)
            if (check_file(f.path(), true)) continue;
            
            // 대상 파일 사용 가능 여부 비활성화
            nodes[f.path()].available = false;
        }
        return;
    }
    
    throw std::runtime_error("DSTree::pruning() : 주소가 잘못되었습니다.");
}
void                                DSTree::grafting(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 디렉토리인지 확인
    if (!exists(dir))
        throw std::runtime_error("DSTree::grafting() : 주소가 잘못되었습니다.");
    
    // 파일일 경우
    if (is_regular_file(dir))
    {
        // 대상 파일인지 확인(사용 가능 여부 포함)
        if (check_file(dir)) return;
        
        // 대상 파일 사용 가능 여부 비활성화
        nodes[dir].available = true;
        return;
    }
    
    // 폴더일 경우
    if (is_directory(dir))
    {
        // 현재 디렉토리에 있는 파일 순회
        for (auto& f : directory_iterator(dir))
        {
            // 대상 파일인지 확인(사용 가능 여부 포함)
            if (check_file(f.path())) continue;
            
            // 대상 파일 사용 가능 여부 비활성화
            nodes[f.path()].available = true;
        }
        return;
    }
    
    throw std::runtime_error("DSTree::grafting() : 주소가 잘못되었습니다.");
}
void                                DSTree::pre_refresh()
{
    using namespace std;
    using namespace std::filesystem;
    
    // root_dir 탈출 방지
    if (this->pre==this->root_dir) return;
    
    // pre 새로 고쳐야 하는지 확인
    vector<path>list_file = get(this->pre);
    
    // pre 한단계 위로 빠져나가기
    if (list_file.empty())
    {
        pre = pre.parent_path();
        pre_refresh();
    }
}


// 사용자
int                                 DSTree::select(std::filesystem::path dir)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 존재하는 파일인지 확인
    if (!exists(dir) || !is_regular_file(dir))
        throw std::runtime_error("DSTree::select() : 주소가 잘못되었습니다.");
    
    // 선택 가능한 파일들 불러오기
    vector<path> list_file = get(pre);
    
    // 선택한 파일(dir)을 목록(list_file)에서 찾을 수 없는 경우
    if (find(list_file.begin(), list_file.end(), dir)==list_file.end())
    {
        return -1;
    }
    
    // pre 값 갱신
    pre = dir.parent_path();
    
    // @ 의존성 가지치기 : 현재 부모 폴더와 같은 디렉토리에 있는 다른 폴더들도 가지치기
    if (dir.parent_path().string().find("-@")!=string::npos)
    {
        for (auto& p : directory_iterator(dir.parent_path().parent_path()))
            if (is_directory(p.path())) pruning(p.path());
    }
    
    // # 의존성 가지치기 : 현재 부모 폴더만 가지치기
    if (dir.parent_path().string().find("-#")!=string::npos)
    {
        pruning(dir.parent_path());
    }
    
    // pre 위치 새로고침
    pre_refresh();
    
    return 0;
}
int                                 DSTree::selectable()
{
    using namespace std;
    using namespace std::filesystem;
    
    if (get(pre).empty())   return 0;
    else                    return 1;
}
std::vector<std::filesystem::path>  DSTree::get()
{
    using namespace std;
    using namespace std::filesystem;
    
    return get(pre);
}
void                                DSTree::reset()
{
    using namespace std;
    using namespace std::filesystem;
    
    // pre 위치 초기화
    pre = root_dir;
    
    // 모든 파일 availabel==true로 바꿈
    for(auto& pair : nodes)
    {
        pair.second.available = true;
    }
}
void                                DSTree::state(std::vector<std::filesystem::path> list, bool available_state)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 전체 상태 설정
    if (available_state)    for (auto& pair : nodes) pair.second.available=false;   // 전체 비활성화
    else                    for (auto& pair : nodes) pair.second.available=true;    // 전체 활성화
    
    // list에 명시된 파일이나 폴더 상태 설정
    if (available_state)    for (auto& p : list) grafting(p);   // 선택된 목록 활성화
    else                    for (auto& p : list) pruning(p);    // 선택된 목록 비활성화
}
std::vector<std::vector<std::filesystem::path>> DSTree::get_list(const std::vector<std::vector<std::filesystem::path>>& list, std::filesystem::path p)
{
    using namespace std;
    using namespace std::filesystem;
    
    // 변수들
    vector<vector<path>>    list_1 = list;          // 부모 위치의 리스트
    vector<path>            list_2 = get_files(p);  // 현재 위치의 파일들
    vector<vector<path>>    list_pre;               // 현재 위치의 리스트
    vector<vector<path>>    list_return;            // list_return = 현재 위치의 리스트 + 자식 위치의 리스트
    
    // 현재 위치의 리스트 계산 방법
    //
    // list_total      list_pre    list_return
    //
    // hello world 0   a b         hello world 0 a
    // hello world 1               hello world 0 b
    // hello world 2               hello world 1 a
    // hello world 3               hello world 1 b
    //                             hello world 2 a
    //                             hello world 2 b
    //                             hello world 3 a
    //                             hello world 3 b
    // 현재 위치 리스트 구하기
    for (auto& l1 : list_1)
    {
        for (auto& l2 : list_2)
        {
            vector<path> tmp(l1);
            tmp.push_back(l2);
            list_pre.push_back(tmp);
        }
    }
    if (list_1.empty())
    {
        for (auto& l2 : list_2)
        {
            vector<path> tmp;
            tmp.push_back(l2);
            list_pre.push_back(tmp);
        }
    }
    
    // 현재 위치의 리스트 추가
    for (auto&v : list_pre)
    {
        list_return.push_back(v);
    }
    
    // 자식 위치의 리스트 구하기
    for (auto& d : get_folders(p))
    {
        // 자식 위치의 리스트 추가
        for (auto& v : get_list(list_pre, d))
        {
            list_return.push_back(v);
        }
    }
    
    return list_return;
}


}
