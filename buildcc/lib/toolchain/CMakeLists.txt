project(toolchain)

add_library(toolchain
    toolchain.cpp
    toolchain.h
)
target_include_directories(toolchain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:.>
)
target_compile_options(toolchain PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(toolchain PRIVATE ${BUILD_LINK_FLAGS})

# TODO, Add Mocks here if required
