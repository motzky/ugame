add_library(
    gamelib STATIC
    buffer_writer.cpp
    buffer.cpp
    camera.cpp
    cube_map.cpp
    exception.cpp
    frame_buffer.cpp
    key_event.cpp
    material.cpp
    mesh.cpp
    mesh_loader.cpp
    mouse_button_event.cpp
    mouse_event.cpp
    physics/box_shape.cpp
    physics/debug_renderer.cpp
    physics/jolt_utils.cpp
    physics/physics_system.cpp
    physics/shape.cpp
    physics/sphere_shape.cpp
    primitives/debug_lines.cpp
    primitives/entity.cpp
    renderer.cpp
    resource_loader.cpp
    shader.cpp
    texture.cpp
    texture_sampler.cpp
    tlv/tlv_entry.cpp
    tlv/tlv_reader.cpp
    tlv/tlv_writer.cpp
)

add_library(imguilib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

add_executable(
    nathan_stream
    main.cpp
)

if(WIN32)
    target_sources(
        gamelib
        PRIVATE win32/window.cpp win32/debug_ui.cpp win32/file.cpp
    )
    target_sources(
        imguilib
        PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
elseif(UNIX)
    target_sources(
        gamelib
        PRIVATE glfw/window.cpp glfw/debug_ui.cpp posix/file.cpp
    )
    target_sources(
        imguilib
        PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )
endif()

target_include_directories(imguilib PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(nathan_stream PUBLIC ${stb_lib_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})
target_include_directories(gamelib PUBLIC ${stb_lib_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR} ${JoltPhysics_SOURCE_DIR})

target_compile_features(
    gamelib
    PUBLIC cxx_std_23
)
target_compile_options(
    gamelib
    PUBLIC -std=c++23 -Wall -Wextra -Wpedantic -Werror -pedantic-errors
)
target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)
target_compile_options(
    nathan_stream
    PUBLIC -std=c++23 -Wall -Wextra -Wpedantic -Werror -pedantic-errors
)

target_link_libraries(
    gamelib
    PUBLIC
        imguilib
        Jolt
    PRIVATE
        GL
        Backtrace::Backtrace
        stdc++exp
)

target_link_libraries(
    nathan_stream
    PUBLIC
        gamelib
    PRIVATE
        glfw
        GL
        Backtrace::Backtrace
        stdc++exp        
)