
#include <msig_Algorithm.hpp>

namespace MSIG {
namespace Algorithm {

MusicalSymbol::MusicalSymbol(std::filesystem::path imagePath, bool makingConfigData, int width, int height, int staffPadding)
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

MusicalSymbol::operator bool() const
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

MusicalSymbol
MusicalSymbol::operator& (const MusicalSymbol& other) const
{
    // 합성하려는 두 이미지의 같은 픽셀 위치에서 가장 작은(어두운) 값이 선택됨.
    // 256 & 256 -> 256
    // 256 & 194 -> 194
    // 194 & 0   -> 0
    // 0   & 0   -> 0
    
    // 1. 두 이미지 준비
    MusicalSymbol thisImage = this->copy();
    MusicalSymbol otherImage = other.copy();
    thisImage.as_default();
    otherImage.as_default();
    
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

MusicalSymbol
MusicalSymbol::operator| (const MusicalSymbol& other) const
{
    // 합성하려는 두 이미지의 같은 픽셀 위치에서 가장 큰(밝은) 값이 선택됨.
    // 256 | 256 -> 256
    // 256 | 194 -> 256
    // 194 | 0   -> 194
    // 0   | 0   -> 0
    
    // 1. 두 이미지 준비
    MusicalSymbol thisImage = this->copy();
    MusicalSymbol otherImage = other.copy();
    thisImage.as_default();
    otherImage.as_default();
    
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

cv::Mat
MusicalSymbol::__rendering(int x, int y, double degree, double scale,
                           bool extensionSize,
                           bool auxiliaryStaff,
                           bool auxiliaryCenter)
{
    namespace fs = std::filesystem;
    
    //
    cv::Mat newImage((extensionSize ? diagonal : height), (extensionSize ? diagonal : width), CV_8UC1, cv::Scalar(255));
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

void
MusicalSymbol::__save_config()
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

void
MusicalSymbol::edit_config()
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
            case 'a': this->x += 5; break;  // 좌로 이동
            case 'd': this->x -= 5; break;  // 우로 이동
            case 'w': this->y += 5; break;  // 상으로 이동
            case 's': this->y -= 5; break;  // 하로 이동
            case 'q': this->degree+=3.0;  break;  // 반시계 회전
            case 'e': this->degree-=3.0;  break;  // 시계 회전
            case 'z': this->scale-=0.2;   break;  // 축소
            case 'c': this->scale+=0.2;   break;  // 확대
            // 빠르게
            case 'A': this->x+=1;         break;  // 좌로 이동
            case 'D': this->x-=1;         break;  // 우로 이동
            case 'W': this->y+=1;         break;  // 상으로 이동
            case 'S': this->y-=1;         break;  // 하로 이동
            case 'Q': this->degree+=1.0;  break;  // 반시계 회전
            case 'E': this->degree-=1.0;  break;  // 시계 회전
            case 'Z': this->scale-=0.1;   break;  // 축소
            case 'C': this->scale+=0.1;   break;  // 확대
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

void
MusicalSymbol::as_default()
{
    namespace fs = std::filesystem;
    
    //
    original = false;
    
    // 이미지 수정
    image = image.clone();
    image = Processing::Matrix::mat_rotate(image, degree, x, y);
    image = Processing::Matrix::mat_scale(image, scale, x, y);
}

void
MusicalSymbol::show()
{
    //
    cv::Mat previewImage = __rendering(x, y, degree, scale, true, false, false);
    
    //
    cv::imshow("MusicalSymbol preview", previewImage);
    cv::waitKey();
    cv::destroyWindow("MusicalSymbol preview");
}

cv::Mat
MusicalSymbol::rendering(bool extensionSize, bool auxiliaryStaff, bool auxiliaryCenter)
{
    return __rendering(x, y, degree, scale, extensionSize, auxiliaryStaff, auxiliaryCenter);
}

MusicalSymbol
MusicalSymbol::copy() const
{
    MusicalSymbol ms = *this;
    ms.image = ms.image.clone();
    ms.original = false;
    return ms;
}

}
}
