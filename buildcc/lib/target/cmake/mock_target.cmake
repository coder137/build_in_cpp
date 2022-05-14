add_library(mock_target STATIC
    ${COMMON_TARGET_SRCS}
    # Custom Generator mocks
    mock/custom_generator/runner.cpp
    mock/custom_generator/recheck_states.cpp

    # Generator mocks
    mock/generator/runner.cpp

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
    Taskflow

    mock_toolchain

    CppUTest
    CppUTestExt
    ${TEST_LINK_LIBS}
)

# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(mock_target PUBLIC -Wl,--allow-multiple-definition)
endif()
