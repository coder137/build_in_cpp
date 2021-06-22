# Target_specialized lib
m_clangtidy("target_specialized")
add_library(target_specialized INTERFACE
    target_constants.h
    target_gcc.h
    target_msvc.h
    target_generic.h
)
target_include_directories(target_specialized INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target_specialized INTERFACE
    target
)

# TODO, Add target_specialized tests

# target_specialized Install
if (${BUILDCC_INSTALL})
    install(TARGETS target_specialized DESTINATION lib EXPORT target_specializedConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/target_constants.h
        ${CMAKE_CURRENT_SOURCE_DIR}/target_gcc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/target_msvc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/target_generic.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT target_specializedConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_specialized")
endif()
