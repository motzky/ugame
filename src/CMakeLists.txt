add_executable(
    nathan_stream
    camera.cpp
    entity.cpp
    exception.cpp
    main.cpp
    mesh.cpp
    material.cpp
    renderer.cpp
    shader.cpp
    window.cpp
)

target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)
target_compile_options(
    nathan_stream
    PUBLIC -std=c++23 -Wall -Wextra -pedantic-errors
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        glfw
        GL
        Backtrace::Backtrace
        stdc++exp
)