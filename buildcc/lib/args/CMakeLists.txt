# Args test
if (${TESTING})
# TODO, Add for unit testing and mocking
endif()

# Args lib
m_clangtidy("args")
add_library(args
    src/args.cpp
    src/register.cpp
    include/args.h
    include/register.h
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
    install(FILES         
        ${CMAKE_CURRENT_SOURCE_DIR}/include/args.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/register.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT argsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/args")
endif()
