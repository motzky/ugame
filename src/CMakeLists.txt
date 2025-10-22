add_library(
    gamelib STATIC
    primitives/debug_lines.cpp
    primitives/entity.cpp
)


if(WIN32)
add_library(imguilib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp

    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)
else()
add_library(imguilib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)
endif()

add_subdirectory(core)
add_subdirectory(events)
add_subdirectory(game)
add_subdirectory(graphics)
add_subdirectory(loaders)
add_subdirectory(math)
add_subdirectory(messaging)
add_subdirectory(physics)
add_subdirectory(resources)
add_subdirectory(scripting)
add_subdirectory(scheduler)
add_subdirectory(tlv)
add_subdirectory(utils)

add_executable(
    nathan_stream
    main.cpp
)

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

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
        PRIVATE glfw/window.cpp glfw/debug_ui.cpp posix/file.cpp graphics/text_factory.cpp
    )
    target_sources(
        imguilib
        PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )
endif()

target_include_directories(imguilib PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(nathan_stream PUBLIC ${stb_lib_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})
target_include_directories(gamelib PUBLIC ${stb_lib_SOURCE_DIR} ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR} ${JoltPhysics_SOURCE_DIR} ${lua_SOURCE_DIR})

if(${Freetype_FOUND})
target_include_directories(gamelib PUBLIC ${FREETYPE_INCLUDE_DIRS})
else()
target_include_directories(gamelib PUBLIC ${freetype_INCLUDE_DIRS})
endif()

target_compile_features(gamelib PUBLIC cxx_std_23)
target_compile_features(nathan_stream PUBLIC cxx_std_23)

if(MSVC)
target_include_directories(gamelib PUBLIC third_party)
target_compile_options(gamelib PUBLIC /W4 /WX /Debug /Od)
target_compile_definitions(gamelib PRIVATE -DWIN32 -D_WIN32 -DNOMINMAX)
target_compile_options(nathan_stream PUBLIC  /W4 /WX /Debug /Od)
target_compile_definitions(nathan_stream PRIVATE -DWIN32 -D_WIN32)

target_include_directories(gamelib PRIVATE ${zstd_SOURCE_DIR}/lib)
target_include_directories(nathan_stream PRIVATE ${zstd_SOURCE_DIR}/lib)
else()
target_compile_options(gamelib PUBLIC -Wall -Wextra -Wpedantic -Werror -pedantic-errors)
target_compile_options(nathan_stream PUBLIC -Wall -Wextra -Wpedantic -Werror -pedantic-errors)
endif()

if(NOT ${Freetype_FOUND})
target_include_directories(gamelib PRIVATE ${freetype_SOURCE_DIR}/include)
endif()

if(WIN32)
target_link_libraries(
    gamelib
    PUBLIC imguilib Jolt opengl32 lua
    PRIVATE libzstd_static ${FREETYPE_LIBRARY_NAME}
)
target_link_libraries(
    nathan_stream
    PUBLIC gamelib
    PRIVATE opengl32
)
elseif(UNIX)
target_link_libraries(
    gamelib
    PUBLIC imguilib Jolt lua
    PRIVATE GL glfw Backtrace::Backtrace stdc++exp libzstd_static ${FREETYPE_LIBRARY_NAME}
)
target_link_libraries(
    nathan_stream
    PUBLIC gamelib 
    PRIVATE Backtrace::Backtrace stdc++exp
)
endif()

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/resources
    COMMAND $<TARGET_FILE:resource_packer> ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/resources
    COMMENT "Building resources"
)
add_custom_target(resources DEPENDS ${PROJECT_BINARY_DIR}/resources COMMENT "Building resources")
add_dependencies(resources resource_packer gamelib)

add_dependencies(nathan_stream resources)

install(TARGETS nathan_stream DESTINATION bin COMPONENT application)
install(FILES ${PROJECT_BINARY_DIR}/resources DESTINATION bin COMPONENT application)