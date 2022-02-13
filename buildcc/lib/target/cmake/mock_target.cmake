add_library(mock_target STATIC
    ${COMMON_TARGET_SRCS}

    # Generator mocks
    src/generator/task.cpp
    mock/generator/runner.cpp
    mock/generator/recheck_states.cpp

    # Target mocks
    src/target/tasks.cpp
    mock/target/runner.cpp
    mock/target/recheck_states.cpp
)
target_include_directories(mock_target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
)

target_compile_options(mock_target PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
target_link_options(mock_target PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
target_link_libraries(mock_target PUBLIC 
    flatbuffers_header_only
    Taskflow

    mock_schema
    mock_toolchain

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
