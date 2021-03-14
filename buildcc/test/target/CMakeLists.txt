configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/generated/constants.h @ONLY)

set(TEST_NAME "test_target")
add_executable(
    ${TEST_NAME}
    ${TEST_NAME}.cpp
    ${BUILDCC_DIR}/src/target.cpp
    ${BUILDCC_DIR}/src/fbs_utils.cpp
    ${BUILDCC_DIR}/src/fbs_loader.cpp
)
target_include_directories(${TEST_NAME} PRIVATE
    generated
    ${BUILDCC_DIR}/include
    ${BUILDCC_DIR}/generated
)
target_link_libraries(${TEST_NAME} PRIVATE
    flatbuffers
    spdlog
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(${TEST_NAME} PRIVATE -g -fprofile-arcs -ftest-coverage -O0)
target_link_options(${TEST_NAME} PRIVATE -g -fprofile-arcs -ftest-coverage -O0)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
