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
add_subdirectory(targets/gcc)

# First party plugins
add_subdirectory(plugins)

add_library(buildcc INTERFACE
    include/buildcc.h
)
target_include_directories(buildcc INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(buildcc INTERFACE 
    target
    
    toolchain_specialized

    # TODO, Add specialized targets
    # GCC, Clang, MSVC
    target_gcc

    plugins
)

if (${BUILDCC_INSTALL})
    install(TARGETS buildcc DESTINATION lib EXPORT buildccConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/include/buildcc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    install(EXPORT buildccConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/buildcc")
endif()
