add_library(mock_target STATIC
    src/target/target.cpp
    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/lib.cpp
    src/target/build.cpp
    src/target/flags.cpp
    mock/target/recheck_states.cpp
    mock/target/tasks.cpp

    src/fbs/fbs_loader.cpp
    src/fbs/fbs_storer.cpp

    src/util/util.cpp
    src/util/command.cpp
    mock/util/execute.cpp
)
target_include_directories(mock_target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_compile_options(mock_target PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
target_link_options(mock_target PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
target_link_libraries(mock_target PUBLIC 
    flatbuffers_header_only
    Taskflow

    mock_env
    toolchain

    CppUTest
    CppUTestExt
    gcov
)

# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(mock_target PUBLIC -Wl,--allow-multiple-definition)
endif()

add_dependencies(mock_target fbs_to_header)
