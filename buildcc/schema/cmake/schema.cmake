# schema test
if (${TESTING})
    add_library(mock_schema STATIC
        include/schema/interface/serialization_interface.h

        include/schema/path.h

        src/custom_generator_serialization.cpp
        include/schema/custom_generator_schema.h
        include/schema/custom_generator_serialization.h

        src/target_serialization.cpp
        include/schema/target_schema.h
        include/schema/target_serialization.h
    )
    target_include_directories(mock_schema PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/include
        ${SCHEMA_BUILD_DIR}
    )
    target_link_libraries(mock_schema PUBLIC
        mock_env
        nlohmann_json::nlohmann_json
    
        CppUTest
        CppUTestExt
        ${TEST_LINK_LIBS}
    )

    target_compile_options(mock_schema PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
    target_link_options(mock_schema PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})

    # Tests
    add_executable(test_custom_generator_serialization
        test/test_custom_generator_serialization.cpp
    )
    target_link_libraries(test_custom_generator_serialization PRIVATE mock_schema)

    add_executable(test_target_serialization
        test/test_target_serialization.cpp
    )
    target_link_libraries(test_target_serialization PRIVATE mock_schema)

    add_test(NAME test_custom_generator_serialization COMMAND test_custom_generator_serialization
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    add_test(NAME test_target_serialization COMMAND test_target_serialization
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
endif()

set(SCHEMA_SRCS
    include/schema/interface/serialization_interface.h

    include/schema/path.h

    src/custom_generator_serialization.cpp
    include/schema/custom_generator_schema.h
    include/schema/custom_generator_serialization.h

    src/target_serialization.cpp
    include/schema/target_schema.h
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
        nlohmann_json::nlohmann_json
    )
    target_include_directories(schema PRIVATE
        ${SCHEMA_BUILD_DIR}
    )
    target_compile_options(schema PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(schema PRIVATE ${BUILD_LINK_FLAGS})
endif()

if (${BUILDCC_INSTALL})
    if (${BUILDCC_BUILD_AS_INTERFACE})
        install(TARGETS schema DESTINATION lib EXPORT schemaConfig)
        install(EXPORT schemaConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/schema")
    endif()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}")
endif()
