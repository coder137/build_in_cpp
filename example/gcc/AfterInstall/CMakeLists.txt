cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# NOTE, Might be needed for Linux
set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)

project(simple)

# Package dependencies
# fmt is imported by spdlog by default
find_package(fmt_package NAMES "fmt" REQUIRED)
find_package(spdlog_package NAMES "spdlog" REQUIRED)
find_package(flatbuffers_package NAMES "Flatbuffers" "flatbuffers" REQUIRED)
find_package(taskflow_package NAMES "Taskflow" "taskflow" REQUIRED)
find_package(CLI11_package NAMES "CLI11" REQUIRED)
find_package(tiny_process_library_package NAMES "tiny-process-library" REQUIRED)

find_package(env_package NAMES "env" REQUIRED)
find_package(toolchain_package NAMES "toolchain" REQUIRED)
find_package(toolchain_specialized_package NAMES "toolchain_specialized" REQUIRED)
find_package(target_package NAMES "target" REQUIRED)
find_package(target_specialized_package NAMES "target_specialized" REQUIRED)
find_package(plugins_package NAMES "plugins" REQUIRED)
find_package(args_package NAMES "args" REQUIRED)

find_package(buildcc_package NAMES "buildcc" REQUIRED)

message("Find package: ${fmt_package_DIR}")
message("Find package: ${spdlog_package_DIR}")
message("Find package: ${flatbuffers_package_DIR}") #
message("Find package: ${taskflow_package_DIR}")
message("Find package: ${CLI11_package_DIR}")
message("Find package: ${tiny_process_library_package_DIR}") #

message("Find package: ${env_package_DIR}") #
message("Find package: ${toolchain_package_DIR}")
message("Find package: ${toolchain_specialized_package_DIR}")
message("Find package: ${target_package_DIR}") #
message("Find package: ${target_specialized_package_DIR}")

message("Find package: ${plugins_package_DIR}") #
message("Find package: ${args_package_DIR}") # 

message("Find package: ${buildcc_package_DIR}")

# build executable
add_executable(build build.cpp)
target_include_directories(build PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(build PRIVATE 
    buildcc
)
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(build PRIVATE -Wl,--allow-multiple-definition)
endif()


# Add your constants file for the environment
configure_file(constants.h.in ${CMAKE_BINARY_DIR}/generated/constants.h @ONLY)
