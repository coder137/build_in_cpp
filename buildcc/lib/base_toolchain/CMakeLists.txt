project(base_toolchain)

add_library(base_toolchain)
target_sources(base_toolchain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cpp
)
target_include_directories(base_toolchain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# TODO, Add Mocks here if required
