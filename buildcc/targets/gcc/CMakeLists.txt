project(target_gcc)

add_library(target_gcc)
target_sources(target_gcc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/static_target.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_target.cpp
)
target_include_directories(target_gcc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(target_gcc PRIVATE
    env
    toolchain
    target
)
target_compile_options(target_gcc PRIVATE ${BUILD_COMPILE_FLAGS})

if (BUILDCC_GCC_TESTING)
    add_subdirectory(test)
endif()
