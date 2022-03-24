set(TOOLCHAIN_SRCS
    # COMMON
    src/common/toolchain_config.cpp
    include/toolchain/common/toolchain_config.h
    include/toolchain/common/file_ext.h
    include/toolchain/common/function_lock.h

    # API
    src/api/toolchain_find.cpp
    src/api/toolchain_verify.cpp
    include/toolchain/api/toolchain_find.h
    include/toolchain/api/toolchain_verify.h
    include/toolchain/api/flag_api.h

    src/toolchain/toolchain.cpp
    include/toolchain/toolchain.h
)
if (${TESTING})
    add_library(mock_toolchain
        ${TOOLCHAIN_SRCS}
    )
    target_include_directories(mock_toolchain PUBLIC
        include
    )
    target_compile_options(mock_toolchain PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_toolchain PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
    target_link_libraries(mock_toolchain PUBLIC
        mock_schema

        CppUTest
        CppUTestExt
        ${TEST_LINK_LIBS}
    )

    add_executable(test_toolchain_id
        test/test_toolchain_id.cpp
    )
    target_link_libraries(test_toolchain_id PRIVATE
        mock_toolchain
    )

    add_executable(test_toolchain_config
        test/test_toolchain_config.cpp
    )
    target_link_libraries(test_toolchain_config PRIVATE
        mock_toolchain
    )

    add_executable(test_toolchain_find
        test/test_toolchain_find.cpp
    )
    target_link_libraries(test_toolchain_find PRIVATE 
        mock_toolchain
    )

    add_executable(test_toolchain_verify
        test/test_toolchain_verify.cpp
    )
    target_link_libraries(test_toolchain_verify PRIVATE 
        mock_toolchain
    )

    add_test(NAME test_toolchain_id COMMAND test_toolchain_id)
    add_test(NAME test_toolchain_config COMMAND test_toolchain_config)
    add_test(NAME test_toolchain_find COMMAND test_toolchain_find
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    add_test(NAME test_toolchain_verify COMMAND test_toolchain_verify
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
endif()

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TOOLCHAIN_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("toolchain")
    add_library(toolchain
        ${TOOLCHAIN_SRCS}
    )
    target_include_directories(toolchain PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(toolchain PUBLIC 
        schema
    )
endif()

if (${BUILDCC_INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
    if (${BUILDCC_BUILD_AS_INTERFACE})
    # Toolchain install
        install(TARGETS toolchain DESTINATION lib EXPORT toolchainConfig)
        install(EXPORT toolchainConfig DESTINATION lib/cmake/toolchain)
    endif()
endif()
