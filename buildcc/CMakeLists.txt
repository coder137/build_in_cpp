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
