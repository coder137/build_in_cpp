set(TEST_NAME "test_assert_fatal")
add_executable(
    ${TEST_NAME}
    ${TEST_NAME}.cpp
)
target_include_directories(${TEST_NAME} PRIVATE
    ${TARGET_DIR}/include
)
target_link_libraries(${TEST_NAME} PRIVATE
    env
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(${TEST_NAME} PRIVATE ${TEST_COMPILE_FLAGS})
target_link_options(${TEST_NAME} PRIVATE ${TEST_LINK_FLAGS})

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
