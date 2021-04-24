cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(simple)

find_package(spdlog_package NAMES "spdlog" REQUIRED)
find_package(env_package NAMES "env" REQUIRED)

find_package(toolchain_package NAMES "toolchain" REQUIRED)

find_package(flatbuffers_package NAMES "flatbuffers" REQUIRED)
find_package(target_package NAMES "target" REQUIRED)

find_package(target_gcc_package NAMES "target_gcc" REQUIRED)

find_package(plugins NAMES "plugins" REQUIRED)

find_package(buildcc_package NAMES "buildcc" REQUIRED)

message("Find package: ${spdlog_package_DIR}")
message("Find package: ${env_package_DIR}")
message("Find package: ${toolchain_package_DIR}")
message("Find package: ${flatbuffers_package_DIR}")
message("Find package: ${target_package_DIR}")
message("Find package: ${target_gcc_package_DIR}")
message("Find package: ${plugins_DIR}")
message("Find package: ${buildcc_package_DIR}")

# build executable
add_executable(build build.cpp)
target_include_directories(build PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(build PRIVATE 
    buildcc
)

# Add your constants file for the environment
configure_file(constants.h.in ${CMAKE_BINARY_DIR}/generated/constants.h @ONLY)
