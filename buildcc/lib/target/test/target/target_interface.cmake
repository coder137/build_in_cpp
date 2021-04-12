add_library(target_interface STATIC)

target_sources(target_interface PUBLIC
    ${target_SOURCE_DIR}/src/target/target.cpp

    ${target_SOURCE_DIR}/src/fbs/fbs_loader.cpp
    ${target_SOURCE_DIR}/src/fbs/fbs_storer.cpp

    ${target_SOURCE_DIR}/src/util/util.cpp
    ${target_SOURCE_DIR}/src/util/command.cpp
)

target_include_directories(target_interface PUBLIC
    ${target_SOURCE_DIR}/include
    ${target_BINARY_DIR}/generated
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(target_interface PUBLIC
    mock_env
    toolchain

    flatbuffers

    CppUTest
    CppUTestExt
    gcov
)

target_compile_options(target_interface PUBLIC ${TEST_COMPILE_FLAGS}
)

target_link_options(target_interface PUBLIC ${TEST_LINK_FLAGS})

add_dependencies(target_interface fbs_to_header)
