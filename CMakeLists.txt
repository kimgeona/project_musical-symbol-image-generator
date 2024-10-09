# ==== cmake 설정 ====
cmake_minimum_required(VERSION 3.28)
project(
    Musical-Symbol-Image-Generator
    VERSION 2.0.0
    DESCRIPTION "Musical symbol image generator for machine learning."
    LANGUAGES CXX
)

# ==== 프로젝트 버전 정보를 컴파일러 옵션으로 전달합니다. ====
add_compile_definitions(MSIG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(MSIG_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(MSIG_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# ==== 패키지 ====
find_package(OpenCV)
# OpenCV include_dir : ${OpenCV_INCLUDE_DIRS}
# OpenCV Library : ${OpenCV_LIBS}

# ==== C++ 표준 설정 ====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==== include 디렉토리 설정 ====
include_directories(
    ./include
    ${OpenCV_INCLUDE_DIRS}
)

# ==== 파일 조사, 타겟 생성 ====
add_subdirectory(./include)
add_subdirectory(./lib)
add_subdirectory(./src)

# ==== 운영체제 관련 설정 ====
if(APPLE)
    # Xcode : Apple Framework 만들기
    #set_target_properties(
    #    my_library PROPERTIES
    #    FRAMEWORK TRUE
    #    FRAMEWORK_VERSION ${CMAKE_PROJECT_VERSION} # 프로젝트 버전과 동일하게 가려고 함.
    #    MACOSX_FRAMEWORK_IDENTIFIER org.cmake.my_library
    #)
    # Xcode : Working Directory 설정
    set_target_properties(
        my_program PROPERTIES
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src_io/
    )
elseif(WIN32)
    # MSVC : Working Directory 설정
    set_target_properties(
        my_program PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src_io/
    )
    target_compile_options(my_library PRIVATE "/utf-8")
    target_compile_options(my_program PRIVATE "/utf-8")
endif()

# ==== 프로젝트 설치 ====
# ...나중에 작성 예정

# ==== 변수 출력 ====
# include(CMakePrintHelpers)
# cmake_print_variables(CMAKE_PROJECT_VERSION)
# cmake_print_variables(INCLUDE_LIST)
# cmake_print_variables(LIB_LIST)
# cmake_print_variables(SRC_LIST)
