# 디렉토리에 있는 *.cpp 파일을 SRC_LIST 변수에 추가한다
file(
    GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# 변수를 상위 스코프로 올린다
set(SRC_LIST ${SRC_LIST} PARENT_SCOPE)

# my_program
add_executable(my_program ${INCLUDE_LIST} ${SRC_LIST})

# my_program 의 의존성 라이브러리
target_link_libraries(
    my_program PRIVATE
    my_library
    ${OpenCV_LIBS}
)