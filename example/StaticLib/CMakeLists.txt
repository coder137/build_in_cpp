cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(simple)

# buildcc should be added as a Shared Library, or Object Library
# Link time for Static libraries is huge
set(BUILDCC_TESTING OFF CACHE BOOL "BuildCC Testing")
add_subdirectory(../../ BuildCC)

# build executable
add_executable(build build.cpp)
target_include_directories(build PRIVATE ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(build PRIVATE buildcc)

# Add your constants file for the environment
configure_file(constants.h.in ${CMAKE_BINARY_DIR}/generated/constants.h @ONLY)

# # Convert binary serialized targets to readable json format
# set(FLATC_CONVERSION_OPTIONS
#     --strict-json
#     --defaults-json
# )
# add_custom_target(convert_bin_to_json
#     COMMAND flatc -o ${CMAKE_CURRENT_SOURCE_DIR} --json ${FLATC_CONVERSION_OPTIONS} --raw-binary ${BuildCC_SOURCE_DIR}/buildcc/fbs/target.fbs -- ${CMAKE_CURRENT_SOURCE_DIR}/Simple.exe.bin
# )
