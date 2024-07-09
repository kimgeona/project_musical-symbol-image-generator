#include <msig_MSIG.hpp>

namespace msig
{


MSIG::MSIG(std::filesystem::path default_dataset_dir, std::filesystem::path new_dataset_dir)
{
    // 플랫폼 설정
#ifdef _WIN32
    system("chcp 65001");
#endif
    
    // 데이터셋 주소 저장
    dir_dataset         = default_dataset_dir;
    dir_dataset_config  = dir_dataset / std::filesystem::path("symbol_dataset_config.txt");
    dir_new_dataset     = new_dataset_dir;
    dir_new_dataset_csv = dir_new_dataset / std::filesystem::path("label.csv");
    
    // 초기화
    count_csv = -1;
    
    // 정보 출력
    print_info();
    print_platform();
}

MSIG::~MSIG()
{
    // CSV 파일 닫기
    if (csv.is_open()) csv.close();
}


void MSIG::print_info()
{
    // 프로그램 정보 출력
    std::cout << R"(
MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM
M"""""`'"""`YM MP""""""`MM M""M MM'"""""`MM
M  mm.  mm.  M M  mmmmm..M M  M M' .mmm. `M
M  MMM  MMM  M M.      `YM M  M M  MMMMMMMM
M  MMM  MMM  M MMMMMMM.  M M  M M  MMM   `M
M  MMM  MMM  M M. .MMM'  M M  M M. `MMM' .M
M  MMM  MMM  M Mb.     .dM M  M MM.     .MM
MMMMMMMMMMMMMM MMMMMMMMMMM MMMM MMMMMMMMMMM
)" << std::endl;
    std::cout << "  Musical Symbol Image Generator(" << MSIG_VERSION << ")" << std::endl << std::endl;
    std::cout << "  - Version: " << MSIG_VERSION << std::endl;
}

void MSIG::print_platform()
{
    // 플랫폼 출력
#ifdef __MACH__
    std::cout << "  - Platform: MacOS" << std::endl << std::endl << std::endl;
#endif
#ifdef _WIN32
    std::cout << "  - Platform: Windows" << std::endl << std::endl << std::endl;
#endif
}


int  MSIG::prepare_default_dataset()
{
    using namespace std::filesystem;
    
    // 기본 데이터셋 이미지들 확인
    std::cout << "  기본 데이터셋의 이미지들을 확인합니다." << std::endl;
    
    // 기본 데이터셋 폴더 확인
    if (!exists(dir_dataset) || !is_directory(dir_dataset))
    {
        std::cout << "  ! 기본 데이터셋 폴더 " << dir_dataset << "가 존재하지 않습니다." << std::endl;
        return -1;
    }
    
    // 기본 데이터셋 폴더의 config 파일 확인
    if (!exists(dir_dataset_config) || !is_regular_file(dir_dataset_config))
    {
        std::cout << "  ! 기본 데이터셋 폴더의 config 파일 " << dir_dataset_config.filename() << "이 존재하지 않아 새로 생성합니다." << std::endl;
        std::fstream(dir_dataset_config.string(), std::ios::out|std::ios::app).close();
    }
    
    // 악상 기호 제대로 생성되는지 확인
    for (auto& p : recursive_directory_iterator(dir_dataset))
    {
        // dataset_dir에서 확인되는 이미지 파일들만
        if (exists(p.path()) && is_regular_file(p.path()) && p.path().extension()==".png")
        {
            try
            {
                // 악상 기호 생성
                msig::MusicalSymbol ms(p, dir_dataset_config);
            }
            catch (const std::runtime_error& e)
            {
                if (e.what() == std::string("msig::MusicalSymbol::MusicalSymbol() : 악상 기호 객체를 생성하는 과정에서 문제가 있습니다."))
                {
                    std::cout << "  ! 기본 데이터셋의 악상 기호 이미지 확인 과정에서 문제가 있습니다." << std::endl;
                    return -1;
                }
                else throw;
            }
        }
    }
    
    std::cout << std::endl;
    return 0;
}

int  MSIG::prepare_DST()
{
    using namespace std::filesystem;
    
    // DST 알고리즘 준비
    std::cout << "  DST 알고리즘을 준비합니다." << std::endl;
    
    // 기본 데이터셋 폴더 확인
    if (!exists(dir_dataset) || !is_directory(dir_dataset))
    {
        std::cout << "  ! 기본 데이터셋 폴더 " << dir_dataset << "가 존재하지 않습니다." << std::endl;
        return -1;
    }
    
    // DST 생성
    try
    {
        dst = msig::DST((dir_dataset / path("complete")).string());
    }
    catch (const std::runtime_error& e)
    {
        if (e.what() == std::string("msig::DSTFolder::DSTFolder() : 비어있는 경로이거나 존재하지 않는 폴더로 DSTFolder 객체를 생성할 수 없습니다."))
        {
            std::cout << "  ! 기본 데이터셋 폴더 " << dir_dataset << "를 이용하여 DST를 생성할 수 없습니다." << std::endl;
            return -1;
        }
        else throw;
    }
    
    std::cout << std::endl;
    return 0;
}

int  MSIG::prepare_Canvas()
{
    using namespace std::filesystem;
    
    // Canvas 준비
    std::cout << "  악상 기호를 그릴 Canvas를 준비합니다." << std::endl;
    
    // 사용 가능한 코어 갯수 구하기
    number_of_thread = std::thread::hardware_concurrency(); // 사용 가능한 코어 수 구하기
    if (number_of_thread < 1) number_of_thread = 1;
    
    // 사용 가능한 코어 개수 만큼 Canvas 생성
    for (unsigned int i=0; i<number_of_thread; i++) // 사용 가능한 코어 수 만큼 캔버스 생성
    {
        // 기본 데이터셋 폴더 확인
        if (!exists(dir_dataset) || !is_directory(dir_dataset))
        {
            std::cout << "  ! 기본 데이터셋 폴더 " << dir_dataset << "가 존재하지 않습니다." << std::endl;
            return -1;
        }
        
        // 캔버스 생성
        try
        {
            canvases.emplace_back(dir_dataset, 192, 512); // (데이터셋 주소, 이미지 크기)
        }
        catch (const std::runtime_error& e)
        {
            if (e.what() == std::string("msig::Canvas::Canvas() : 캔버스를 생성하는 과정에서 문제가 있습니다."))
            {
                std::cout << "  ! 캔버스를 생성하는 과정에서 문제가 있습니다." << std::endl;
            }
            else throw;
        }
    }
    
    std::cout << std::endl;
    return 0;
}

int  MSIG::prepare_csv()
{
    using namespace std::filesystem;
    
    // 공유 자원 잠금
    std::lock_guard<std::mutex> lock(mtx_io);
    
    // 이미 CSV 파일이 열려있다면 건너뛰기
    if (csv.is_open()) return 1;
    
    // 레이블 생성
    labels = dst.get_labels();
    
    // 폴더 존재 확인
    if (!exists(dir_new_dataset)) create_directory(dir_new_dataset);
    
    // CSV 파일 존재 여부에 따라 다르게 준비
    if (exists(dir_new_dataset_csv))
    {
        // CSV 파일 열기
        csv = std::fstream(dir_new_dataset_csv.string(), std::ios::in);
        if (!csv)
        {
            std::runtime_error("  ! CSV 파일을 열 수 없습니다.");
        }
        
        // CSV 파일 읽기(라인 수 세기)
        std::string line;
        while (getline(csv, line)) if (!line.empty()) count_csv++;
        count_csv--;
        csv.close();
        
        // CSV 파일 열기
        csv = std::fstream(dir_new_dataset_csv.string(), std::ios::out);
        if (!csv.is_open())
        {
            std::runtime_error("  ! CSV 파일을 생성할 수 없습니다.");
        }
        
        // 헤더가 존재하지 않는다면
        if (count_csv < 0)
        {
            // CSV 헤더 생성
            std::string csv_header = "name";
            for (auto& s : labels) csv_header += "," + s;
            
            // CSV 헤더 저장
            csv << csv_header;
            
            // CSV 카운트 초기화
            count_csv = 0;
        }
    }
    else
    {
        // CSV 파일 생성
        csv = std::fstream(dir_new_dataset_csv.string(), std::ios::out);
        if (!csv.is_open())
        {
            std::runtime_error("  ! CSV 파일을 생성할 수 없습니다.");
        }
        
        // CSV 헤더 생성
        std::string csv_header = "name";
        for (auto& s : labels) csv_header += "," + s;
        
        // CSV 헤더 저장
        csv << csv_header;
        
        // CSV 카운트 초기화
        count_csv = 0;
    }

    return 0;
}


int MSIG::making_dataset()
{
    // 악상기호를 그리기 위한 설정
    std::cout << std::endl << "  사용자 설정을 진행합니다." << std::endl;
    
    // 각 악상 기호 조합 깊이별로 생성할 비율 입력받기
    std::vector<double> rate;
    while (true)
    {
        std::string input;
        std::cout << "  > 각 깊이별로 생성 비율 입력(종료는 exit): ";
        std::getline(std::cin, input);
        
        // 종료 조건 확인
        if (input == "exit") return 1;
        
        // 비어있는지 확인
        if (input == "") continue;
        
        // 입력받은 문자열 전처리
        input = msig::my_trim(input);           // 좌우 공백 제거
        auto vs = msig::my_split(input, " ");   // " "을 기준으로 문자열 분리
        
        // rate 벡터에 입력받은 값들 추가
        try
        {
            for (auto& s : vs)
            {
                double tmp = std::stod(s);
                if (tmp < 0.0 || tmp > 1.0) throw std::runtime_error("  ! 입력되는 값들은 0~1 사이의 값이어야 합니다.");
                rate.push_back(tmp);
            }
        }
        catch (const std::exception& e)
        {
            rate.clear();   // 입력 받은 rate 초기화
            continue;       // 다시 입력 받기
        }
        
        break;
    }
    
    // 생성되는 이미지 개수 계산
    std::cout << "  - 생성되는 이미지 개수를 계산합니다. ";
    all_combination = dst.list(rate);
    
    // 진행 여부 확인
    while (true)
    {
        char c;
        std::cout << "  > 진행 하시겠습니까? (y/n): ";
        std::cin >> c;
        std::cin.ignore(100, '\n');
        if      (c == 'y')  break;
        else if (c == 'n')  return 1;
        else                continue;
    }
    
    // 악상 기호 데이터셋 생성 시작
    std::cout << std::endl << std::endl << "  악상 기호 이미지 데이터셋을 생성합니다. ";
    
    // 5초 타이머
    for (int i=5; i>=0; i--)
    {
        std::cout << i << " ";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << std::endl;

    // CSV 파일 생성
    prepare_csv();
    
    // 스레딩 시작
    for (unsigned int i=0; i<number_of_thread; i++) threads.emplace_back(&MSIG::thread_processing, this, i, all_combination.size());
    
    // 스레딩 작업 기다리기
    for (unsigned int i=0; i<number_of_thread; i++) threads[i].join();
    
    // 악상 기호 데이터셋 생성 완료
    std::cout << "  - 생성 완료." << std::endl << std::endl;
    
    return 0;
}


int  MSIG::append_to_csv(std::string data)
{
    // 공유 자원 잠금
    std::lock_guard<std::mutex> lock(mtx_io);
    
    // 이미 CSV 파일이 열려있다면 건너뛰기
    if (!csv.is_open()) return -1;
    
    // CSV 데이터 뒤에 추가
    csv << std::endl << data;

    return 0;
}


void MSIG::thread_processing(int canvas_number, const long data_size)
{
    using namespace std;
    using namespace std::filesystem;
    
    while (true)
    {
        // 악상 기호 조합 정보
        vector<path> vp;
        
        {
            // 공유 자원 잠금
            lock_guard<mutex> lock(mtx_data);
            
            // 남은 작업량 확인
            if (all_combination.empty()) break;
            
            // 작업할 내용 추출
            vp = all_combination.front();
            all_combination.pop();
        }
        
        // 캔버스에 그리기
        for (auto& p : vp) canvases[canvas_number].select(p);
        
        // 데이터셋 폴더 확인
        if (!exists(dir_new_dataset)) create_directory(dir_new_dataset);
        
        // 정보 생성
        std::filesystem::path   image_dir = naming();                                       // 이미지 이름
        std::string             image_label = labeling(image_dir.filename().string(), vp);  // 이미지 레이블
        
        // 공유 자원 얻기
        int pre=0;
        {
            lock_guard<mutex> lock(mtx_data);
            
            // 진행정도 계산
            pre = (int)(((double)(data_size - all_combination.size()) / (double)data_size) * 100.0);
        }
        
        // 이미 존재하는 이미지는 건너뛰기
        if (exists(image_dir))
        {
            // 건너뛰기
            printf("(%d%%) pass : %s\n", pre, image_dir.string().c_str());
        }
        else
        {
            // 이미지 저장
            canvases[canvas_number].save(image_dir.string());
            printf("(%d%%) save : %s\n", pre, image_dir.string().c_str());
            
            // CSV 데이터 저장
            append_to_csv(image_label);
        }
        
        // 선택 초기화
        canvases[canvas_number].select_celar();
    }
}

std::filesystem::path   MSIG::naming()
{
    // 공유 자원 잠금
    std::lock_guard<std::mutex> lock(mtx_count);
    
    // 갯수
    static int count = 0;
    
    // 현재 갯수 리턴
    return dir_new_dataset / std::filesystem::path(std::to_string(count++) + ".png");
}

std::string             MSIG::labeling(std::string name, const std::vector<std::filesystem::path>& v)
{
    using namespace std;
    
    // 레이블 목록 복사
    std::vector<std::string> vs = labels;
    
    // 레이블 목록에서 일치하는 것 찾기
    for (auto& p : v)
    {
        // 레이블 목록(vs)에 조합(p)이 존재하는지 찾기
        auto iter = find(vs.begin(), vs.end(), my_split(p.filename().stem().string(), "~")[0]);
        
        // 찾았다면
        if (iter != vs.end()) *iter = "";
    }
    
    // 찾은 것들은 1로, 못찾은 것들은 0로 csv 데이터 생성
    for (auto& v : vs)
    {
        if (v == "")    name += ",1";  // 찾음
        else            name += ",0";  // 못찾음
    }
    
    // 생성된 csv 데이터 반환
    return name;
}


}
