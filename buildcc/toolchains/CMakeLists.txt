add_library(toolchain_specialized INTERFACE
    toolchain_gcc.h
    # TODO, msvc
    # TODO, clang
)
target_include_directories(toolchain_specialized INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(toolchain_specialized INTERFACE 
    toolchain
)
