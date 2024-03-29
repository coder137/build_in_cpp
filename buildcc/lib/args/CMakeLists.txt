# Args test
if (${TESTING})
add_library(mock_args
    src/args.cpp
    mock/parse.cpp
    src/register.cpp
    mock/tasks.cpp
)
target_include_directories(mock_args PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
)
target_compile_options(mock_args PUBLIC 
    ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS}
)
target_link_options(mock_args PUBLIC 
    ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS}
)
target_link_libraries(mock_args PUBLIC 
    CLI11::CLI11

    mock_target
    mock_toolchain_specialized

    CppUTest
    CppUTestExt
    ${TEST_LINK_LIBS}
)

# Tests
add_executable(test_args
    test/test_args.cpp
)
target_link_libraries(test_args PRIVATE
    mock_args
)

add_executable(test_register
    test/test_register.cpp
)
target_link_libraries(test_register PRIVATE 
    mock_args
)

add_test(NAME test_args COMMAND test_args
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
)
add_test(NAME test_register COMMAND test_register
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
)
endif()

set(ARGS_SRCS
    src/args.cpp
    src/parse.cpp
    src/register.cpp
    src/tasks.cpp
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

        toolchain_specialized
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

if (${BUILDCC_INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
