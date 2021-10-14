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

# Target friend

add_executable(test_file_extension
    test_file_extension.cpp
)
target_link_libraries(test_file_extension PRIVATE target_interface)

add_test(NAME test_file_extension COMMAND test_file_extension
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Target

# Test base target
add_executable(test_base_target
    test_base_target.cpp
)
target_link_libraries(test_base_target PRIVATE target_interface)

# Test target pch
add_executable(test_target_pch
    test_target_pch.cpp
)
target_link_libraries(test_target_pch PRIVATE target_interface)

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


# Test target c compile flags
add_executable(test_target_flags
    test_target_flags.cpp
)
target_link_libraries(test_target_flags PRIVATE target_interface)

# Test target user deps
add_executable(test_target_user_deps
    test_target_user_deps.cpp
)
target_link_libraries(test_target_user_deps PRIVATE target_interface)

add_executable(test_target_lock
    test_target_lock.cpp
)
target_link_libraries(test_target_lock PRIVATE target_interface)

# Tests
add_test(NAME test_base_target COMMAND test_base_target)
add_test(NAME test_target_pch COMMAND test_target_pch)
add_test(NAME test_target_source COMMAND test_target_source)
add_test(NAME test_target_source_out_of_root COMMAND test_target_source_out_of_root)
add_test(NAME test_target_include_dir COMMAND test_target_include_dir)
add_test(NAME test_target_lib_dep COMMAND test_target_lib_dep)
add_test(NAME test_target_external_lib COMMAND test_target_external_lib)

add_test(NAME test_target_flags COMMAND test_target_flags)
add_test(NAME test_target_user_deps COMMAND test_target_user_deps)
add_test(NAME test_target_lock COMMAND test_target_lock)
