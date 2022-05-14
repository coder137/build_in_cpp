set(TARGET_SRCS 
    ${COMMON_TARGET_SRCS}

    src/custom_generator/recheck_states.cpp

    src/target/recheck_states.cpp
    src/target/tasks.cpp
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TARGET_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("target")
    add_library(target
        ${TARGET_SRCS}
    )
    target_include_directories(target PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(target PUBLIC 
        toolchain
        Taskflow
    )
    target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(target PRIVATE ${BUILD_LINK_FLAGS})
endif()
