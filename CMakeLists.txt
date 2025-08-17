cmake_minimum_required(VERSION 3.22)

project(
    nathan_stream
    VERSION 0.1
    DESCRIPTION "A simple C++ project following Nathan's stream"
    LANGUAGES C CXX
)

set (CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_subdirectory(src)

