# Toolchain lib
m_clangtidy("toolchain")
add_library(toolchain INTERFACE
    include/toolchain/toolchain.h
)
target_include_directories(toolchain INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)

# TODO, Add Mocks here if required

# Toolchain install
if (${BUILDCC_INSTALL})
    install(TARGETS toolchain DESTINATION lib EXPORT toolchainConfig)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    install(EXPORT toolchainConfig DESTINATION lib/cmake/toolchain)
endif()
