add_library(plugins
    clang_compile_commands.cpp
    clang_compile_commands.h
)
target_include_directories(plugins PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(plugins PRIVATE target)

# Target_gcc Install
if (${BUILDCC_INSTALL})
    install(TARGETS plugins DESTINATION lib EXPORT pluginsConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/clang_compile_commands.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT pluginsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/plugins")
endif()
