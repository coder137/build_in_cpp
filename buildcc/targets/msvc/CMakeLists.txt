# Target_msvc lib
m_clangtidy("target_msvc")
add_library(target_msvc INTERFACE
    executable_target_msvc.h
    static_target_msvc.h
    dynamic_target_msvc.h
)
target_include_directories(target_msvc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target_msvc INTERFACE
    target
)

# TODO, Add Target_msvc tests

# Target_msvc Install
if (${BUILDCC_INSTALL})
    install(TARGETS target_msvc DESTINATION lib EXPORT target_msvcConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/executable_target_msvc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/static_target_msvc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_target_msvc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT target_msvcConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_msvc")
endif()
