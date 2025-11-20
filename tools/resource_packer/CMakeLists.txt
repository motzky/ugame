add_executable(resource_packer
    main.cpp
)
if(WIN32)
target_include_directories(resource_packer PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tools PRIVATE ${zstd_SOURCE_DIR}/lib)
else()
target_include_directories(resource_packer PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tools)
endif()
if(MSVC)
target_compile_options(resource_packer PUBLIC /W4 /WX /Debug /Od)
target_compile_definitions(resource_packer PRIVATE -DWIN32 -D_WIN32 -DNOMINMAX)
endif()


target_link_libraries(
    resource_packer
    PUBLIC 
        gamelib 
    PRIVATE
        assimp
        libzstd_static
)

add_dependencies(resource_packer gamelib)