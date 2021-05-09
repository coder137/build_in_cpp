add_library(toolchain_specialized INTERFACE)
target_include_directories(toolchain_specialized INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(toolchain_specialized INTERFACE 
    toolchain
)

# toolchain_specialized Install
if (${BUILDCC_INSTALL})
    install(TARGETS toolchain_specialized DESTINATION lib EXPORT toolchain_specializedConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/toolchain_gcc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/toolchain_msvc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT toolchain_specializedConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/toolchain_specialized")
endif()
