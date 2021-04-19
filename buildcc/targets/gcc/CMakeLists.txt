project(target_gcc)

add_library(target_gcc
    static_target.cpp
    dynamic_target.cpp
    executable_target.h
    static_target.h
    dynamic_target.h
)
target_include_directories(target_gcc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target_gcc PRIVATE
    env
    toolchain
    target
)
target_compile_options(target_gcc PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target_gcc PRIVATE ${BUILD_LINK_FLAGS})

# Install
if (${BUILDCC_INSTALL})
    install(TARGETS target_gcc DESTINATION lib EXPORT target_gccConfig)
    install(FILES  
        ${CMAKE_CURRENT_SOURCE_DIR}/executable_target.h
        ${CMAKE_CURRENT_SOURCE_DIR}/static_target.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_target.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT target_gccConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_gcc")
endif()
