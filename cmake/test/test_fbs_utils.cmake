# RELATIVE_TO: ${CMAKE_SOURCE_DIR}/buildcc
file(COPY src/test/Simple.exe.bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(
    test_fbs_utils

    src/test/test_fbs_utils.cpp
    src/fbs_utils.cpp
    src/target.cpp
)
target_include_directories(test_fbs_utils PRIVATE
    generated
    fbs
    include
)
target_link_libraries(test_fbs_utils PRIVATE
    flatbuffers
    spdlog
    CppUTest
    CppUTestExt
    gcov
)
target_compile_options(test_fbs_utils PRIVATE -g -fprofile-arcs -ftest-coverage -O0)
target_link_options(test_fbs_utils PRIVATE -g -fprofile-arcs -ftest-coverage -O0)
add_test(NAME test_fbs_utils COMMAND test_fbs_utils)
