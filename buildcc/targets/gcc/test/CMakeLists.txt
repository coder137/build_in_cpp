configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

add_library(test_target_gcc STATIC)
target_include_directories(test_target_gcc PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_options(test_target_gcc PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
target_link_options(test_target_gcc PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
target_link_libraries(test_target_gcc PUBLIC 
    target_gcc
    env
    toolchain
    target

    CppUTest
    CppUTestExt
    gcov
)

# Tests
add_executable(test_target_static_lib test_target_static_lib.cpp)
target_link_libraries(test_target_static_lib PRIVATE test_target_gcc)
add_test(NAME test_target_static_lib COMMAND test_target_static_lib)
