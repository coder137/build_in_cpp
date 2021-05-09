# Target_msvc lib
add_library(target_msvc
    executable_target_msvc.cpp
    executable_target_msvc.h
)
target_include_directories(target_msvc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target_msvc PRIVATE
    target
)
target_compile_options(target_msvc PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target_msvc PRIVATE ${BUILD_LINK_FLAGS})

# TODO, Add Target_msvc tests

# Target_msvc Install
if (${BUILDCC_INSTALL})
    install(TARGETS target_msvc DESTINATION lib EXPORT target_msvcConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/executable_target_msvc.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT target_msvcConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_msvc")
endif()
