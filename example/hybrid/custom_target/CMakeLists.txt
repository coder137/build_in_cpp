cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(hybrid_customtarget_example)

# Bootstrap your build file using CMake
add_executable(hybrid_customtarget_example 
    build.main.cpp
)
target_link_libraries(hybrid_customtarget_example PRIVATE buildcc)

# TODO, Add this only if MINGW is used
# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(hybrid_customtarget_example PRIVATE -Wl,--allow-multiple-definition)
endif()

# Run your build file
add_custom_target(run_hybrid_customtarget_example_win
    COMMAND hybrid_customtarget_example --help-all
    COMMAND hybrid_customtarget_example --config ${CMAKE_CURRENT_SOURCE_DIR}/build_win.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS hybrid_customtarget_example buildcc
    VERBATIM USES_TERMINAL
)

add_custom_target(run_hybrid_customtarget_example_linux
    COMMAND hybrid_customtarget_example --help-all
    COMMAND hybrid_customtarget_example --config ${CMAKE_CURRENT_SOURCE_DIR}/build_linux.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS hybrid_customtarget_example buildcc
    VERBATIM USES_TERMINAL
)
