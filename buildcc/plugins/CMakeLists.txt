# Plugin test
if (${TESTING})
add_library(mock_plugins
    src/buildcc_find.cpp
)
target_include_directories(mock_plugins PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(mock_plugins PUBLIC 
    ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS}
)
target_link_options(mock_plugins PUBLIC 
    ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS}
)
target_link_libraries(mock_plugins PUBLIC 
    mock_target

    CppUTest
    CppUTestExt
    ${TEST_LINK_LIBS}
)

# Tests
add_executable(test_buildcc_find
    test/test_buildcc_find.cpp
)
target_link_libraries(test_buildcc_find PRIVATE
    mock_plugins
)

add_test(NAME test_buildcc_find COMMAND test_buildcc_find
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
)

endif()

set(PLUGINS_SRCS
    src/clang_compile_commands.cpp
    src/buildcc_find.cpp
    include/plugins/clang_compile_commands.h
    include/plugins/buildcc_find.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${PLUGINS_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if (${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("plugins")
    add_library(plugins
        ${PLUGINS_SRCS}
    )
    target_include_directories(plugins PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(plugins PRIVATE target)

    # Target_gcc Install
    if (${BUILDCC_INSTALL})
        install(TARGETS plugins DESTINATION lib EXPORT pluginsConfig)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
        install(EXPORT pluginsConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/plugins")
    endif()
endif()

if (${BUILDCC_INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
