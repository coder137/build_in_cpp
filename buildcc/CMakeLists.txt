# Library
add_subdirectory(lib/env)
add_subdirectory(lib/target)

add_library(buildcc INTERFACE)
target_include_directories(buildcc INTERFACE 
    include
)
target_link_libraries(buildcc INTERFACE 
    env
    target
)

# Tests
if (BUILDCC_TESTING)
    set(BUILDCC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
