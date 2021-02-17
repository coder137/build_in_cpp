cmake_minimum_required(VERSION 3.17.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(simple)

# buildcc should be added as a Shared Library, or Object Library
# Link time for Static libraries is huge
add_subdirectory(../../ BuildCC)

# build executable
add_executable(build build.cpp)
target_include_directories(build PRIVATE generated)
target_link_libraries(build PRIVATE buildcc)

# TODO, Generate a header file that contains important variables
set(CONTENT "\
// clang-format off
#pragma once \


inline constexpr char const * BUILD_SCRIPT_SOURCE = \"${CMAKE_CURRENT_SOURCE_DIR}\"\;\

inline constexpr char const * BUILD_SCRIPT_EXE = \"${CMAKE_CURRENT_BINARY_DIR}\"\;\

inline constexpr char const * BUILD_SCRIPT_FOLDER = \"${CMAKE_CURRENT_SOURCE_DIR}/buildcc\"\;\
")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/generated/constants.h 
    ${CONTENT}
)

# Convert binary serialized targets to readable json format
add_custom_target(convert_bin_to_json
    COMMAND flatc -o ${CMAKE_CURRENT_SOURCE_DIR} --json --strict-json --raw-binary ${BuildCC_SOURCE_DIR}/buildcc/fbs/target.fbs -- ${CMAKE_CURRENT_SOURCE_DIR}/Simple.exe.bin
)
