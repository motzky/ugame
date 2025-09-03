add_executable(
    nathan_stream
    buffer_writer.cpp
    buffer.cpp
    camera.cpp
    entity.cpp
    exception.cpp
    key_event.cpp
    main.cpp
    material.cpp
    mesh.cpp
    mouse_button_event.cpp
    mouse_event.cpp
    renderer.cpp
    resource_loader.cpp
    shader.cpp
    texture.cpp
    texture_sampler.cpp

)

add_library(imguilib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

IF(WIN32)
    target_sources(
        nathan_stream
        PRIVATE win32/window.cpp win32/debug_ui.cpp
    )
    target_sources(
        imguilib
        PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
ELSEIF(UNIX)
    target_sources(
        nathan_stream
        PRIVATE glfw/window.cpp glfw/debug_ui.cpp
    )
    target_sources(
        imguilib
        PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )
ENDIF()

target_include_directories(imguilib PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(nathan_stream PUBLIC ${stb_lib_SOURCE_DIR} ${imgui_SOURCE_DIR})

target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)
target_compile_options(
    nathan_stream
    PUBLIC -std=c++23 -Wall -Wextra -Wpedantic -Werror -pedantic-errors
)

target_link_libraries(
    nathan_stream
    PUBLIC
        imguilib
    PRIVATE
        glfw
        GL
        Backtrace::Backtrace
        stdc++exp
)