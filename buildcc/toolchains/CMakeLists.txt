set(TOOLCHAIN_SPECIALIZED_SRCS
    src/toolchain_infos.cpp
    include/toolchains/toolchain_infos.h

    src/toolchain_gcc.cpp
    include/toolchains/toolchain_gcc.h
    include/toolchains/toolchain_mingw.h

    src/toolchain_msvc.cpp
    include/toolchains/toolchain_msvc.h

    src/toolchain_aggregate.cpp
    include/toolchains/toolchain_generic.h
    include/toolchains/toolchain_custom.h

    include/toolchains/toolchain_specialized.h
)

if (${TESTING})
    add_library(mock_toolchain_specialized
        ${TOOLCHAIN_SPECIALIZED_SRCS}
    )
    target_include_directories(mock_toolchain_specialized PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_options(mock_toolchain_specialized PUBLIC 
    ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS}
    )
    target_link_options(mock_toolchain_specialized PUBLIC 
        ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS}
    )
    target_link_libraries(mock_toolchain_specialized PUBLIC 
        mock_toolchain

        CppUTest
        CppUTestExt
        ${TEST_LINK_LIBS}
    )

    add_executable(test_toolchain_specialized test/test_toolchain_specialized)
    target_link_libraries(test_toolchain_specialized PRIVATE mock_toolchain_specialized)

    add_test(NAME test_toolchain_specialized COMMAND test_toolchain_specialized)
endif()

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TOOLCHAIN_SPECIALIZED_SRCS}
    )
    target_include_directories(buildcc PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("toolchain_specialized")
    add_library(toolchain_specialized STATIC
        ${TOOLCHAIN_SPECIALIZED_SRCS}
    )
    target_include_directories(toolchain_specialized PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(toolchain_specialized PUBLIC 
        toolchain
    )
endif()

if (${BUILDCC_INSTALL})
    if (${BUILDCC_BUILD_AS_INTERFACE})
        # toolchain_specialized Install
        install(TARGETS toolchain_specialized DESTINATION lib EXPORT toolchain_specializedConfig)
        install(EXPORT toolchain_specializedConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/toolchain_specialized")
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
