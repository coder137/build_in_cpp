project(env)

add_library(env
    src/env.cpp
    src/logging.cpp
)
target_include_directories(env PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(env PRIVATE ${BUILD_COMPILE_FLAGS})
# Private stuff here
target_link_libraries(env PRIVATE
    spdlog::spdlog
)

# Mock
add_library(mock_env)
target_sources(mock_env PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/env.cpp
)
target_include_directories(mock_env PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
