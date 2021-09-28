set(PLUGINS_SRCS
    src/clang_compile_commands.cpp
    src/buildcc_find.cpp
    include/plugins/clang_compile_commands.h
    include/plugins/buildcc_find.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${PLUGINS_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if (${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("plugins")
    add_library(plugins
        ${PLUGINS_SRCS}
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
endif()

if (${BUILDCC_INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
