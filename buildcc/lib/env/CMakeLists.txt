# Env test
if (${TESTING})
    add_library(mock_env STATIC
        mock/logging.cpp
        mock/assert_fatal.cpp
        src/env.cpp
    )
    target_include_directories(mock_env PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(mock_env PUBLIC
        fmt::fmt-header-only
    )
    target_compile_options(mock_env PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_env PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
endif()

set(ENV_SRCS
    src/env.cpp
    src/assert_fatal.cpp
    src/logging.cpp
    include/env/assert_fatal.h
    include/env/env.h
    include/env/logging.h
    include/env/host_os.h
    include/env/host_compiler.h
    include/env/util.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${ENV_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("env")
    add_library(env
        ${ENV_SRCS}
    )
    target_include_directories(env PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(env PUBLIC fmt::fmt-header-only)
    target_compile_options(env PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(env PRIVATE ${BUILD_LINK_FLAGS})
    target_link_libraries(env PRIVATE
        spdlog::spdlog_header_only
    )

    # Env install
    if (${BUILDCC_INSTALL})
        install(TARGETS env DESTINATION lib EXPORT envConfig)
        install(EXPORT envConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/env")
    endif()
endif()

if (${BUILDCC_INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
