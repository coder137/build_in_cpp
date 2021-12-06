if (${TESTING})
    add_library(mock_command
        mock/execute.cpp
        src/command.cpp
    )
    target_include_directories(mock_command PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mock
    )
    target_link_libraries(mock_command PUBLIC
        fmt::fmt-header-only
        mock_env
        CppUTest
        CppUTestExt
        gcov
    )
    target_compile_options(mock_command PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_command PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})

    # Tests
    add_executable(test_command test/test_command.cpp)
    target_link_libraries(test_command PRIVATE mock_command)
    
    add_test(NAME test_command COMMAND test_command)
endif()

set(COMMAND_SRCS
    src/execute.cpp
    src/command.cpp
    include/command/command.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${COMMAND_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("command")
    add_library(command
        ${COMMAND_SRCS}
    )
    target_include_directories(command PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(command PRIVATE 
        fmt::fmt-header-only
        tiny-process-library::tiny-process-library

        env
    )
    target_compile_options(command PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(command PRIVATE ${BUILD_LINK_FLAGS})
endif()

if (${BUILDCC_INSTALL})
    if(${BUILDCC_BUILD_AS_INTERFACE})
        install(TARGETS command DESTINATION lib EXPORT commandConfig)
        install(EXPORT commandConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/command")
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
