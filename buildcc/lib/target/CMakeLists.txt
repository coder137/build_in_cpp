project(target)

include(cmake/fbs_gen.cmake)

# Target
add_library(target)
target_include_directories(target PUBLIC
    include
)

target_sources(target PRIVATE
    src/target.cpp

    src/fbs_loader.cpp
    src/fbs_storer.cpp

    src/util.cpp
)
target_include_directories(target PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_libraries(target PRIVATE 
    flatbuffers 
    env
    toolchain
)
add_dependencies(target fbs_to_header)

# Tests
if (BUILDCC_TESTING)
    set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(test)
endif()
