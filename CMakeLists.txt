cmake_minimum_required(VERSION 3.22)

project(
    nathan_stream
    VERSION 0.1
    DESCRIPTION "A simple C++ project following Nathan's stream"
    LANGUAGES C CXX
)

include_directories(include)

add_subdirectory(src)

