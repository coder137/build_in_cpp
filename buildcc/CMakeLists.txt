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

# First party plugins
add_subdirectory(plugins)

m_clangtidy("buildcc")
add_library(buildcc STATIC)
target_sources(buildcc PRIVATE 
    src/args.cpp
    src/register.cpp
)
target_include_directories(buildcc PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(buildcc PUBLIC 
    # 
    CLI11::CLI11

    # 
    target
    
    toolchain_specialized
    target_specialized

    plugins
)
target_precompile_headers(buildcc PUBLIC include/buildcc.h)

if (${BUILDCC_INSTALL})
    install(TARGETS buildcc DESTINATION lib EXPORT buildccConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/include/buildcc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/args.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/register.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    install(EXPORT buildccConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/buildcc")
endif()
