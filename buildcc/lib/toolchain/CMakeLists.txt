# Toolchain lib
m_clangtidy("toolchain")
add_library(toolchain INTERFACE
    toolchain.h
)
target_include_directories(toolchain INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)

# TODO, Add Mocks here if required

# Toolchain install
if (${BUILDCC_INSTALL})
    install(TARGETS toolchain DESTINATION lib EXPORT toolchainConfig)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.h DESTINATION include/buildcc)
    install(EXPORT toolchainConfig DESTINATION lib/cmake/toolchain)
endif()
