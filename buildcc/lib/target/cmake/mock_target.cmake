add_library(mock_target STATIC)
target_include_directories(mock_target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_sources(mock_target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/target.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/source.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/include_dir.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/lib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/build.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/target/flags.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/target/recheck_states.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbs/fbs_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbs/fbs_storer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/util/command.cpp
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
