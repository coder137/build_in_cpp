set(TOOLCHAIN_SPECIALIZED_SRCS
    include/toolchains/toolchain_gcc.h
    include/toolchains/toolchain_msvc.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TOOLCHAIN_SPECIALIZED_SRCS}
    )
    target_include_directories(buildcc INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if (${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("toolchain_specialized")
    add_library(toolchain_specialized INTERFACE
        ${TOOLCHAIN_SPECIALIZED_SRCS}
    )
    target_include_directories(toolchain_specialized INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(toolchain_specialized INTERFACE 
        toolchain
    )

    # toolchain_specialized Install
    if (${BUILDCC_INSTALL})
        install(TARGETS toolchain_specialized DESTINATION lib EXPORT toolchain_specializedConfig)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
        install(EXPORT toolchain_specializedConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/toolchain_specialized")
    endif()
endif()
