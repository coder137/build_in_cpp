# Library
add_subdirectory(lib/env)
add_subdirectory(lib/toolchain)
add_subdirectory(lib/target)

# Specialized
add_subdirectory(targets/gcc)

add_library(buildcc INTERFACE)
target_include_directories(buildcc INTERFACE 
    include
)
target_link_libraries(buildcc INTERFACE 
    env
    target
    toolchain
    # TODO, Add specialized targets
    # GCC, Clang, MSVC
    # TODO, Add specialized toolchains
    # GCC, Clang, MSVC
    target_gcc
)

install(TARGETS env target toolchain target_gcc DESTINATION lib)
install(FILES  
    ${CMAKE_CURRENT_SOURCE_DIR}/include/buildcc.h
    ${env_SOURCE_DIR}/include/assert_fatal.h
    ${env_SOURCE_DIR}/include/env.h
    ${env_SOURCE_DIR}/include/logging.h
    ${toolchain_SOURCE_DIR}/toolchain.h
    ${target_SOURCE_DIR}/include/target.h
    ${target_gcc_SOURCE_DIR}/executable_target.h
    ${target_gcc_SOURCE_DIR}/static_target.h
    ${target_gcc_SOURCE_DIR}/dynamic_target.h
    DESTINATION include/buildcc
)
install(FILES  
    ${target_SOURCE_DIR}/include/internal/fbs_loader.h
    ${target_SOURCE_DIR}/include/internal/path.h
    ${target_SOURCE_DIR}/include/internal/util.h
    DESTINATION include/buildcc/internal
)
