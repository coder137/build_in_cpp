# Args test
if (${TESTING})
# TODO, Add for unit testing and mocking
endif()

set(ARGS_SRCS
    src/args.cpp
    src/register.cpp
    include/args/args.h
    include/args/register.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${ARGS_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("args")
    add_library(args
        ${ARGS_SRCS}
    )
    target_include_directories(args PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(args PUBLIC 
        CLI11::CLI11
    )
    target_link_libraries(args PRIVATE
        target
    )
    target_compile_options(args PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(args PRIVATE ${BUILD_LINK_FLAGS})

    # Args install
    if (${BUILDCC_INSTALL})
        install(TARGETS args DESTINATION lib EXPORT argsConfig)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
        install(EXPORT argsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/args")
    endif()
endif()

