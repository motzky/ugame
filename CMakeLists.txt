cmake_minimum_required(VERSION 3.30)

project(
    nathan_stream
    VERSION 0.1
    DESCRIPTION "A simple C++ project following Nathan's stream"
    LANGUAGES C CXX
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

FetchContent_GetProperties(googletest)

FetchContent_Declare(
    stb_lib
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234
)

FetchContent_MakeAvailable(stb_lib)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.6
)

FetchContent_MakeAvailable(imgui)


find_package(glfw3 REQUIRED)
find_package(Backtrace REQUIRED)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS -std=c++23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FETCHCONTENT_QUIET FALSE)

include_directories(include)

add_subdirectory(src)

enable_testing()
include(CTest)
add_subdirectory(tests)
