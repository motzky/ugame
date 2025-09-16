FetchContent_MakeAvailable(googletest)

include(GoogleTest)

mark_as_advanced(BUILD_GMOCK BUILD_GTEST gtest_hide_internal_symbols)

add_executable(unit_tests
    camera_tests.cpp
    chain_tests.cpp
    matrix3_tests.cpp
    matrix4_tests.cpp
    quaternion_tests.cpp
    tlv_entry_tests.cpp
    tlv_reader_tests.cpp
    tlv_writer_tests.cpp
    vector3_tests.cpp
    vector4_tests.cpp
)

target_link_libraries(unit_tests gmock_main gamelib)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)

