configure_file(constants.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/constants.h @ONLY)

add_library(target_interface INTERFACE)
target_include_directories(target_interface INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_link_libraries(target_interface INTERFACE
    mock_target
)

# Toolchain

add_executable(test_toolchain_flag_api
    test_toolchain_flag_api.cpp
)
target_link_libraries(test_toolchain_flag_api PRIVATE target_interface)

add_test(NAME test_toolchain_flag_api COMMAND test_toolchain_flag_api)

# Interfaces
add_executable(test_serialization_interface
    test_serialization_interface.cpp
)
target_link_libraries(test_serialization_interface PRIVATE target_interface)

add_test(NAME test_serialization_interface COMMAND test_serialization_interface
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Common
add_executable(test_target_state
    test_target_state.cpp
)
target_link_libraries(test_target_state PRIVATE target_interface)

add_test(NAME test_target_state COMMAND test_target_state)

# Generator
add_executable(test_custom_generator
    test_custom_generator.cpp
)
target_link_libraries(test_custom_generator PRIVATE target_interface)

add_executable(test_file_generator
    test_file_generator.cpp
)
target_link_libraries(test_file_generator PRIVATE target_interface)

add_executable(test_template_generator
    test_template_generator.cpp
)
target_link_libraries(test_template_generator PRIVATE target_interface)

add_test(NAME test_custom_generator COMMAND test_custom_generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(NAME test_file_generator COMMAND test_file_generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(NAME test_template_generator COMMAND test_template_generator
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Target friend

add_executable(test_compile_object
    test_compile_object.cpp
)
target_link_libraries(test_compile_object PRIVATE target_interface)

add_test(NAME test_compile_object COMMAND test_compile_object
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

add_executable(test_target_sync
    test_target_sync.cpp
)
target_link_libraries(test_target_sync PRIVATE target_interface)

add_executable(test_target_failure_states
    test_target_failure_states.cpp
)
target_link_libraries(test_target_failure_states PRIVATE target_interface)

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

add_test(NAME test_target_sync COMMAND test_target_sync)

add_test(NAME test_target_failure_states COMMAND test_target_failure_states)
