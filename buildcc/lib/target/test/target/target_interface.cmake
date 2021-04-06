add_library(target_interface INTERFACE)
target_sources(target_interface INTERFACE
    ${target_SOURCE_DIR}/src/target.cpp
    ${target_SOURCE_DIR}/src/toolchain.cpp

    ${target_SOURCE_DIR}/src/fbs_loader.cpp
    ${target_SOURCE_DIR}/src/fbs_storer.cpp

    ${target_SOURCE_DIR}/src/util.cpp

    ${target_SOURCE_DIR}/src/static_target.cpp
    ${target_SOURCE_DIR}/src/dynamic_target.cpp   
)
target_include_directories(target_interface INTERFACE
    ${target_SOURCE_DIR}/include
    ${target_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target_interface INTERFACE
    mock_env
    flatbuffers
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(target_interface INTERFACE ${TEST_COMPILE_FLAGS}
)
target_link_options(target_interface INTERFACE ${TEST_LINK_FLAGS})
