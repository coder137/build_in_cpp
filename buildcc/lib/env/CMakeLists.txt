# Env lib
add_library(env
    src/env.cpp
    src/logging.cpp
    include/env.h
    include/assert_fatal.h
    include/logging.h
)
target_include_directories(env PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(env PUBLIC
    spdlog::spdlog
)
target_compile_options(env PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(env PRIVATE ${BUILD_LINK_FLAGS})

# Env test
if (${TESTING})
    add_library(mock_env STATIC)
    target_sources(mock_env PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/logging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/env.cpp
    )
    target_include_directories(mock_env PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(mock_env PUBLIC
        spdlog::spdlog
    )
endif()

# Env install
if (${BUILDCC_INSTALL})
    install(TARGETS env DESTINATION lib EXPORT envConfig)
    install(FILES         
        ${CMAKE_CURRENT_SOURCE_DIR}/include/assert_fatal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/env.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/logging.h
        DESTINATION "${BUILDCC_INSTALL_HEADER_PREFIX}"
    )
    install(EXPORT envConfig DESTINATION "${BUILDCC_INSTALL_LIB_PREFIX}/env")
endif()
