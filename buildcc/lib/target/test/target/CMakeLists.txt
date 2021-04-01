configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

include(target_interface.cmake)

# Add Test
# TODO, Update this later
set(TEST_NAME "test_target")
add_executable(
    ${TEST_NAME}
    ${TEST_NAME}.cpp
    ${TARGET_DIR}/src/static_target.cpp
    ${TARGET_DIR}/src/dynamic_target.cpp   
)
target_link_libraries(${TEST_NAME} PRIVATE
    target_interface
)

# Test target sources
add_executable(test_target_source
    test_target_source.cpp
)
target_link_libraries(test_target_source PRIVATE target_interface)

# Test target include dir
add_executable(test_target_include_dir
    test_target_include_dir.cpp
)
target_link_libraries(test_target_include_dir PRIVATE target_interface)

# TODO, Test target static lib
add_executable(test_target_static_lib
    test_target_static_lib.cpp
)
target_link_libraries(test_target_static_lib PRIVATE target_interface)

# TODO, Test target dynamic lib

# TODO, Test target toolchain preprocessor flags

# TODO, Test target toolchain compile flags

# TODO Test target toolchain link flags

# Tests
# add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
add_test(NAME test_target_source COMMAND test_target_source)
add_test(NAME test_target_include_dir COMMAND test_target_include_dir)
add_test(NAME test_target_static_lib COMMAND test_target_static_lib)
