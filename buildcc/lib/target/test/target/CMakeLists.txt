configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

add_library(target_interface INTERFACE)
target_include_directories(target_interface INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target_interface INTERFACE
    mock_target
)

# Interfaces
add_executable(test_builder_interface
    test_builder_interface.cpp
)
target_link_libraries(test_builder_interface PRIVATE target_interface)

add_test(NAME test_builder_interface COMMAND test_builder_interface)

# Generator
add_executable(test_generator
    test_generator.cpp
)
target_link_libraries(test_generator PRIVATE target_interface)

add_test(NAME test_generator COMMAND test_generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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

# Test target out of root sources
add_executable(test_target_source_out_of_root
    test_target_source_out_of_root.cpp
)
target_link_libraries(test_target_source_out_of_root PRIVATE target_interface)

# Test target include dir
add_executable(test_target_include_dir
    test_target_include_dir.cpp
)
target_link_libraries(test_target_include_dir PRIVATE target_interface)

# Test target lib dep (buildcc built)
# NOTE, Tested behaviour via static library
add_executable(test_target_lib_dep
    test_target_lib_dep.cpp
)
target_link_libraries(test_target_lib_dep PRIVATE target_interface)

# Test target lib dep (external lib -L and -l flag for gcc)
add_executable(test_target_external_lib
    test_target_external_lib.cpp
)
target_link_libraries(test_target_external_lib PRIVATE target_interface)

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


# Test target link flags
add_executable(test_target_link_flags
    test_target_link_flags.cpp
)
target_link_libraries(test_target_link_flags PRIVATE target_interface)

# Test target user deps
add_executable(test_target_user_deps
    test_target_user_deps.cpp
)
target_link_libraries(test_target_user_deps PRIVATE target_interface)

# Tests
add_test(NAME test_base_target COMMAND test_base_target)
add_test(NAME test_target_source COMMAND test_target_source)
add_test(NAME test_target_source_out_of_root COMMAND test_target_source_out_of_root)
add_test(NAME test_target_include_dir COMMAND test_target_include_dir)
add_test(NAME test_target_lib_dep COMMAND test_target_lib_dep)
add_test(NAME test_target_external_lib COMMAND test_target_external_lib)

add_test(NAME test_target_preprocessor_flags COMMAND test_target_preprocessor_flags)
add_test(NAME test_target_c_compile_flags COMMAND test_target_c_compile_flags)
add_test(NAME test_target_link_flags COMMAND test_target_link_flags)
add_test(NAME test_target_user_deps COMMAND test_target_user_deps)
