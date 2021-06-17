# Target_gcc lib
m_clangtidy("target_gcc")
add_library(target_gcc INTERFACE
    executable_target_gcc.h
    static_target_gcc.h
    dynamic_target_gcc.h
)
target_include_directories(target_gcc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target_gcc INTERFACE
    target
)

# TODO, Add Target_gcc tests

# Target_gcc Install
if (${BUILDCC_INSTALL})
    install(TARGETS target_gcc DESTINATION lib EXPORT target_gccConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/executable_target_gcc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/static_target_gcc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_target_gcc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT target_gccConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_gcc")
endif()
