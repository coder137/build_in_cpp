add_library(mock_target STATIC)
target_include_directories(mock_target PUBLIC
    ${target_SOURCE_DIR}/include
    ${target_SOURCE_DIR}/mock
    ${target_BINARY_DIR}/generated
)

target_sources(mock_target PUBLIC
    ${target_SOURCE_DIR}/src/target/target.cpp
    ${target_SOURCE_DIR}/src/target/source.cpp
    ${target_SOURCE_DIR}/src/target/include_dir.cpp
    ${target_SOURCE_DIR}/src/target/lib_dep.cpp
    ${target_SOURCE_DIR}/src/target/build.cpp
    ${target_SOURCE_DIR}/mock/target/recheck_states.cpp

    ${target_SOURCE_DIR}/src/fbs/fbs_loader.cpp
    ${target_SOURCE_DIR}/src/fbs/fbs_storer.cpp

    ${target_SOURCE_DIR}/src/util/util.cpp
    ${target_SOURCE_DIR}/mock/util/command.cpp
)

target_compile_options(mock_target PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
target_link_options(mock_target PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
target_link_libraries(mock_target PUBLIC 
    flatbuffers 
    mock_env
    toolchain

    CppUTest
    CppUTestExt
    gcov
)
add_dependencies(mock_target fbs_to_header)
