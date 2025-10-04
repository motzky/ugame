FetchContent_MakeAvailable(googletest)

include(GoogleTest)

mark_as_advanced(BUILD_GMOCK BUILD_GTEST gtest_hide_internal_symbols)

add_executable(unit_tests
    camera_tests.cpp
    compress_tests.cpp
    ensure_tests.cpp
    frustum_tests.cpp
    lua_script_tests.cpp
    lua_interop_tests.cpp
    matrix3_tests.cpp
    matrix4_tests.cpp
    message_bus_tests.cpp
    quaternion_tests.cpp
    resource_cache_tests.cpp
    script_runner_tests.cpp
    tlv_entry_tests.cpp
    tlv_reader_tests.cpp
    tlv_writer_tests.cpp
    vector3_tests.cpp
    vector4_tests.cpp
)

if(MSVC)
target_include_directories(unit_tests PRIVATE ${zstd_SOURCE_DIR}/lib)
endif()

target_link_libraries(unit_tests gmock_main gamelib)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)

add_dependencies(unit_tests gamelib)

add_custom_command(
    TARGET unit_tests
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${UNIT_TEST}$" --output-on-failure -j
)