add_library(env
    src/env.cpp
)
target_include_directories(env PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Private stuff here
target_link_libraries(env PRIVATE
    spdlog::spdlog
)
