m_clangtidy("plugins")
add_library(plugins
    src/clang_compile_commands.cpp
    include/plugins/clang_compile_commands.h
)
target_include_directories(plugins PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(plugins PRIVATE target)

# Target_gcc Install
if (${BUILDCC_INSTALL})
    install(TARGETS plugins DESTINATION lib EXPORT pluginsConfig)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    install(EXPORT pluginsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/plugins")
endif()
