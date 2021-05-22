cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(hybrid_simple_example)

# Bootstrap your build file using CMake
add_executable(build build.cpp)
target_include_directories(build PRIVATE ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(build PRIVATE buildcc)

# Run your build file
add_custom_target(run_hybrid_simple_example
    COMMAND build --help-all
    COMMAND build --config ${CMAKE_CURRENT_SOURCE_DIR}/build.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS build buildcc
    VERBATIM USES_TERMINAL
)
