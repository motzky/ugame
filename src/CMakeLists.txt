add_executable(
    nathan_stream
    main.cpp
)

target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)
target_compile_options(
    nathan_stream
    PUBLIC -Wall -Wextra -Wpedantic
)