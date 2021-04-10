# Library
add_subdirectory(lib/env)
add_subdirectory(lib/toolchain)
add_subdirectory(lib/target)

# Specialized

add_library(buildcc INTERFACE)
target_include_directories(buildcc INTERFACE 
    include
)
target_link_libraries(buildcc INTERFACE 
    env
    target
    toolchain
    # TODO, Add specialized targets
    # GCC, Clang, MSVC
    # TODO, Add specialized toolchains
    # GCC, Clang, MSVC
)
