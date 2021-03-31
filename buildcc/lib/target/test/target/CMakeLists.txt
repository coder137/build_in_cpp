configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

include(target_interface.cmake)

# Add Test
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

# TODO, Test target include dir

# TODO, Test target static lib

# TODO, Test target dynamic lib

# TODO, Test target toolchain preprocessor flags

# TODO, Test target toolchain compile flags

# TODO Test target toolchain link flags

# Tests
# add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
add_test(NAME test_target_source COMMAND test_target_source)
