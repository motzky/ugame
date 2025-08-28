cmake_minimum_required(VERSION 3.30)

project(
    nathan_stream
    VERSION 0.1
    DESCRIPTION "A simple C++ project following Nathan's stream"
    LANGUAGES C CXX
)

find_package(glfw3 REQUIRED)
find_package(Backtrace REQUIRED)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS -std=c++23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_subdirectory(src)

