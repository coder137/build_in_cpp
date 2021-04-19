project(env)

add_library(env
    src/env.cpp
    src/logging.cpp
    include/env.h
    include/assert_fatal.h
    include/logging.h
)
target_include_directories(env PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(env PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(env PRIVATE ${BUILD_LINK_FLAGS})

# Private stuff here
target_link_libraries(env PRIVATE
    spdlog::spdlog
)

# Mock
if (${TESTING})
    add_library(mock_env STATIC)
    target_sources(mock_env PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/logging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/env.cpp
    )
    target_include_directories(mock_env PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()
