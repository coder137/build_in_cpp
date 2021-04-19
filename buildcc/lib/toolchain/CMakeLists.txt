# Toolchain lib
add_library(toolchain
    toolchain.cpp
    toolchain.h
)
target_include_directories(toolchain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_compile_options(toolchain PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(toolchain PRIVATE ${BUILD_LINK_FLAGS})

# TODO, Add Mocks here if required

# Toolchain install
if (${BUILDCC_INSTALL})
    install(TARGETS toolchain DESTINATION lib EXPORT toolchainConfig)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.h DESTINATION include/buildcc)
    install(EXPORT toolchainConfig DESTINATION lib/cmake/toolchain)
endif()
