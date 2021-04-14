configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

add_library(target_interface STATIC)
target_include_directories(target_interface PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target_interface PUBLIC
    mock_target
)


# Test base target
add_executable(test_base_target
    test_base_target.cpp
)
target_link_libraries(test_base_target PRIVATE target_interface)

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

# Test target static lib
add_executable(test_target_static_lib
    test_target_static_lib.cpp
)
target_link_libraries(test_target_static_lib PRIVATE target_interface)

# # Test target dynamic lib
# add_executable(test_target_dynamic_lib
#     test_target_dynamic_lib.cpp
# )
# target_link_libraries(test_target_dynamic_lib PRIVATE target_interface)

# TODO, Test target toolchain preprocessor flags

# TODO, Test target toolchain compile flags

# TODO Test target toolchain link flags

# Tests
add_test(NAME test_base_target COMMAND test_base_target)
add_test(NAME test_target_source COMMAND test_target_source)
add_test(NAME test_target_include_dir COMMAND test_target_include_dir)
add_test(NAME test_target_static_lib COMMAND test_target_static_lib)
# add_test(NAME test_target_dynamic_lib COMMAND test_target_dynamic_lib)
