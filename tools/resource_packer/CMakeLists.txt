add_executable(resource_packer
    main.cpp
)
target_include_directories(resource_packer PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tools)

target_link_libraries(resource_packer gamelib)

