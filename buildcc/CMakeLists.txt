# Install constants
set(BUILDCC_INSTALL_LIB_PREFIX "lib/cmake")
set(BUILDCC_INSTALL_HEADER_PREFIX "include/buildcc")

# Library
add_subdirectory(lib/env)
add_subdirectory(lib/toolchain)
add_subdirectory(lib/target)

# Specialized
add_subdirectory(targets/gcc)

# First party plugins
add_subdirectory(plugin)

add_library(buildcc INTERFACE)
target_include_directories(buildcc INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(buildcc INTERFACE 
    env
    target
    toolchain
    # TODO, Add specialized toolchains
    # GCC, Clang, MSVC

    # TODO, Add specialized targets
    # GCC, Clang, MSVC
    target_gcc

    # TODO, Add plugins
    clang_compile_commands
)

if (${BUILDCC_INSTALL})
    install(TARGETS buildcc DESTINATION lib EXPORT buildccConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/include/buildcc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    install(EXPORT buildccConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/buildcc")
endif()
