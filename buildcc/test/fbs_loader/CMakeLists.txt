file(COPY Simple.exe.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(TEST_NAME "test_fbs_loader")
add_executable(
    ${TEST_NAME}
    ${TEST_NAME}.cpp
)
target_link_libraries(${TEST_NAME} PRIVATE
    target
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(${TEST_NAME} PRIVATE -g -fprofile-arcs -ftest-coverage -O0)
target_link_options(${TEST_NAME} PRIVATE -g -fprofile-arcs -ftest-coverage -O0)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
