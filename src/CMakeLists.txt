add_executable(
    nathan_stream
    main.cpp
    window.cpp
)

target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)
target_compile_options(
    nathan_stream
    PUBLIC -Wall -Wextra -Wpedantic
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        Vulkan::Vulkan
        glfw
)