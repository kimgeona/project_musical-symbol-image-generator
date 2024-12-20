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

#ifndef msig_Structure_hpp
#define msig_Structure_hpp

#include <msig_common.hpp>
#include <msig_Utility.hpp>

namespace MSIG {
namespace Structure {


class Image
{
public:
    double selectionProbability;
public:
    std::filesystem::path              path;
    std::string                        groupName;
    std::vector<std::filesystem::path> excludeFoldersAndImages;
    std::vector<std::filesystem::path> multipleSelectionFolders;
public:
    Image(const std::filesystem::path& imagePath);
    Image(const Image& original, bool copyExclude, bool copyMultipleSelection);
public:
    bool operator<(const Image& other) const;
public:
    void print_information();
};


class Folder
{
public:
    bool splited;
    bool stretched;
public:
    std::filesystem::path path;
    std::vector<Folder>   folders;
    std::vector<Image>    images;
public:
    Folder();
    Folder(const std::filesystem::path& folderPath);
    Folder(const Folder& original, bool copyFolders, bool copyImages);
public:
    bool        operator< (const Folder& other) const;
    int         operator() (const Folder& other);
    explicit    operator std::vector<Folder>() const;
    explicit    operator size_t() const;
private:
    std::vector<Folder> __split();
    void                __stretch();
private:
    void    __pruning(bool recursive, bool keepImageData);
    void    __deleting_rules(bool recursive, bool excludeList, bool multipleSelectionList);
    void    __calculate_all_combination(std::vector<std::vector<unsigned char>>& combinations, unsigned char numberOfThings);
private:
    int     __random_select(const int& start, const int& end) const;
    double  __generate_probability() const;
private:
    void    __save_rule(bool recursive=true);
public:
    Folder  reconstruction();
    void    pick(std::vector<std::filesystem::path>& vp) const;
    void    tree(const std::string& prefix="", size_t order=0);
    void    get_all_images_name(std::set<std::string>& allImagesNames);
};


}
}

#endif /* msig_Structure_hpp */
