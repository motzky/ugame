add_library(
    gamelib STATIC
    camera.cpp
    primitives/debug_lines.cpp
    primitives/entity.cpp
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

add_subdirectory(core)
add_subdirectory(events)
add_subdirectory(graphics)
add_subdirectory(loaders)
add_subdirectory(physics)
add_subdirectory(tlv)

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

target_compile_features(
    nathan_stream
    PUBLIC cxx_std_23
)

if(MSVC)
target_compile_options(
    gamelib
    PUBLIC /std:c++latest /Wall /WX
)
target_compile_definitions(
    gamelib
    PRIVATE
    -DWIN32 -D_WIN32
)
target_compile_options(
    nathan_stream
    PUBLIC /std:c++latest /Wall /WX
)
target_compile_definitions(
    nathan_stream
    PRIVATE
    -DWIN32 -D_WIN32
)
else()
target_compile_options(
    gamelib
    PUBLIC -std=c++23 -Wall -Wextra -Wpedantic -Werror -pedantic-errors
)
target_compile_options(
    nathan_stream
    PUBLIC -std=c++23 -Wall -Wextra -Wpedantic -Werror -pedantic-errors
)
endif()

if(WIN32)
target_link_libraries(
    gamelib
    PUBLIC
        imguilib
        Jolt
    PRIVATE
        GL
        stdc++exp
)
target_link_libraries(
    nathan_stream
    PUBLIC
        gamelib
    PRIVATE
        GL
        stdc++exp        
)
elseif(UNIX)
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
endif()
