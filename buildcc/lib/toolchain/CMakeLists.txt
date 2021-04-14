project(toolchain)

add_library(toolchain)
target_sources(toolchain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cpp
)
target_include_directories(toolchain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(toolchain PRIVATE ${BUILD_COMPILE_FLAGS})
# TODO, Add Mocks here if required
