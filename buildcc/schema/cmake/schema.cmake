# schema test
if (${TESTING})
    add_library(mock_schema STATIC
        src/custom_generator_serialization.cpp
        src/generator_serialization.cpp
        src/target_serialization.cpp

        include/schema/private/schema_util.h
        include/schema/interface/serialization_interface.h

        include/schema/path.h
        include/schema/custom_generator_serialization.h
        include/schema/generator_serialization.h
        include/schema/target_serialization.h
    )
    target_include_directories(mock_schema PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/include
        ${SCHEMA_BUILD_DIR}
    )
    target_link_libraries(mock_schema PUBLIC
        mock_env
        flatbuffers
    
        CppUTest
        CppUTestExt
        ${TEST_LINK_LIBS}
    )
    add_dependencies(mock_schema fbs_to_header)

    target_compile_options(mock_schema PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_schema PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})

    # Tests
    add_dependencies(mock_schema fbs_to_header)
endif()

set(SCHEMA_SRCS
    src/custom_generator_serialization.cpp
    src/generator_serialization.cpp
    src/target_serialization.cpp

    include/schema/private/schema_util.h
    include/schema/interface/serialization_interface.h

    include/schema/path.h
    include/schema/generator_serialization.h
    include/schema/target_serialization.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${SCHEMA_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_include_directories(buildcc PRIVATE
        ${SCHEMA_BUILD_DIR}
    )
    add_dependencies(buildcc fbs_to_header)
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("schema")
    add_library(schema
        ${SCHEMA_SRCS}
    )
    target_include_directories(schema PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(schema PUBLIC
        env
    )
    target_include_directories(schema PRIVATE
        ${SCHEMA_BUILD_DIR}
    )
    target_compile_options(schema PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(schema PRIVATE ${BUILD_LINK_FLAGS})
    target_link_libraries(schema PRIVATE
        flatbuffers
    )
    add_dependencies(schema fbs_to_header)
endif()

if (${BUILDCC_INSTALL})
    if (${BUILDCC_BUILD_AS_INTERFACE})
        install(TARGETS schema DESTINATION lib EXPORT schemaConfig)
        install(EXPORT schemaConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/schema")
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
