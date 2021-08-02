set(TARGET_SPECIALIZED_SRCS
    include/targets/target_constants.h
    include/targets/target_gcc.h
    include/targets/target_msvc.h
    include/targets/target_generic.h
    include/targets/target_custom.h
    include/targets/target_utils.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TARGET_SPECIALIZED_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if (${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("target_specialized")
    add_library(target_specialized INTERFACE
        ${TARGET_SPECIALIZED_SRCS}
    )
    target_include_directories(target_specialized INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(target_specialized INTERFACE
        target
    )

    # target_specialized Install
    if (${BUILDCC_INSTALL})
        install(TARGETS target_specialized DESTINATION lib EXPORT target_specializedConfig)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
        install(EXPORT target_specializedConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/target_specialized")
    endif()
endif()
