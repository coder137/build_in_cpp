cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(hybrid_generic_example)

# Bootstrap your build file using CMake
add_executable(hybrid_generic_example 
    build.cpp
    ../foolib/build.foo.cpp
    ../foolib/build.foo.h
)
target_include_directories(hybrid_generic_example PRIVATE ../foolib)
target_link_libraries(hybrid_generic_example PRIVATE buildcc)

# TODO, Add this only if MINGW is used
# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(hybrid_generic_example PRIVATE -Wl,--allow-multiple-definition)
endif()

# Run your build file
add_custom_target(run_hybrid_generic_example
    COMMAND hybrid_generic_example --help-all
    COMMAND hybrid_generic_example toolchain --help-all
    COMMAND hybrid_generic_example --config ${CMAKE_CURRENT_SOURCE_DIR}/build_generic.toml
    # COMMAND dot -Tpng graph.dot -o graph.PNG
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS hybrid_generic_example buildcc
    VERBATIM USES_TERMINAL
)
