# Env test
if (${TESTING})
    add_library(mock_env STATIC
        mock/logging.cpp
        mock/assert_fatal.cpp

        src/env.cpp
        src/task_state.cpp

        src/command.cpp
        mock/execute.cpp
    )
    target_include_directories(mock_env PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/include
    )
    target_link_libraries(mock_env PUBLIC
        fmt::fmt-header-only
        Taskflow

        CppUTest
        CppUTestExt
        ${TEST_LINK_LIBS}
    )
    target_compile_options(mock_env PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_env PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})

    # Tests
    add_executable(test_env_util test/test_env_util.cpp)
    target_link_libraries(test_env_util PRIVATE mock_env)

    add_executable(test_task_state test/test_task_state.cpp)
    target_link_libraries(test_task_state PRIVATE mock_env)

    add_executable(test_command test/test_command.cpp)
    target_link_libraries(test_command PRIVATE mock_env)

    add_test(NAME test_env_util COMMAND test_env_util)
    add_test(NAME test_task_state COMMAND test_task_state)
    add_test(NAME test_command COMMAND test_command)
endif()

set(ENV_SRCS
    src/env.cpp
    src/assert_fatal.cpp
    src/logging.cpp
    include/env/assert_fatal.h
    include/env/assert_throw.h
    include/env/env.h
    include/env/logging.h
    include/env/util.h

    include/env/host_os.h
    include/env/host_compiler.h
    include/env/host_os_util.h

    src/task_state.cpp
    include/env/task_state.h

    src/command.cpp
    src/execute.cpp
    include/env/command.h
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
        tiny-process-library::tiny-process-library
    )
endif()

if (${BUILDCC_INSTALL})
    if (${BUILDCC_BUILD_AS_INTERFACE})
        install(TARGETS env DESTINATION lib EXPORT envConfig)
        install(EXPORT envConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/env")
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
