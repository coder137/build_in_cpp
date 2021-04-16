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
add_executable(test_target_lib_dep
    test_target_lib_dep.cpp
)
target_link_libraries(test_target_lib_dep PRIVATE target_interface)

# Test target preprocessor flags
add_executable(test_target_preprocessor_flags
    test_target_preprocessor_flags.cpp
)
target_link_libraries(test_target_preprocessor_flags PRIVATE target_interface)

# Test target c compile flags
add_executable(test_target_c_compile_flags
    test_target_c_compile_flags.cpp
)
target_link_libraries(test_target_c_compile_flags PRIVATE target_interface)

# TODO, Test target cpp compile flags

# TODO Test target link flags

# Tests
add_test(NAME test_base_target COMMAND test_base_target)
add_test(NAME test_target_source COMMAND test_target_source)
add_test(NAME test_target_include_dir COMMAND test_target_include_dir)
add_test(NAME test_target_lib_dep COMMAND test_target_lib_dep)
add_test(NAME test_target_preprocessor_flags COMMAND test_target_preprocessor_flags)
add_test(NAME test_target_c_compile_flags COMMAND test_target_c_compile_flags)
