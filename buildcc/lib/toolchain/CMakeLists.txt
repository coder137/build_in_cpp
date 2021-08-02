set(TOOLCHAIN_SRCS
    include/toolchain/toolchain.h
)
if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TOOLCHAIN_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>    
    )
endif()

if (${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("toolchain")
    add_library(toolchain INTERFACE
        ${TOOLCHAIN_SRCS}
    )
    target_include_directories(toolchain INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )

    # Toolchain install
    if (${BUILDCC_INSTALL})
        install(TARGETS toolchain DESTINATION lib EXPORT toolchainConfig)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
        install(EXPORT toolchainConfig DESTINATION lib/cmake/toolchain)
    endif()
endif()
