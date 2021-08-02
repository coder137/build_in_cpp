# Install constants
set(BUILDCC_INSTALL_LIB_PREFIX "lib/cmake")
set(BUILDCC_INSTALL_HEADER_PREFIX "include/buildcc")

# Environment
add_subdirectory(lib/env)

# Toolchain
add_subdirectory(lib/toolchain)
add_subdirectory(toolchains)

# Targets
add_subdirectory(lib/target)
add_subdirectory(targets)

# Buildcc libs
add_subdirectory(lib/args)

# First party plugins
add_subdirectory(plugins)

if (${BUILDCC_BUILD_AS_INTERFACE})
    add_library(buildcc_i INTERFACE
        buildcc.h
    )
    target_include_directories(buildcc_i INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(buildcc_i INTERFACE
        target
        args
        plugins
        toolchain_specialized
        target_specialized
    )
    if(${BUILDCC_PRECOMPILE_HEADERS})
        target_precompile_headers(buildcc_i INTERFACE buildcc.h)    
        target_precompile_headers(buildcc_i INTERFACE ${FBS_DIR}/pch/pch.h)
    endif()
endif()

if (${BUILDCC_INSTALL})
    if(${BUILDCC_BUILD_AS_INTERFACE})
        install(TARGETS buildcc_i DESTINATION lib EXPORT buildcc_iConfig)
        install(EXPORT buildcc_iConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/buildcc_i")
    endif()

    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/buildcc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
