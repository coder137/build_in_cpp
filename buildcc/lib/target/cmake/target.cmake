# Target
m_clangtidy("target")
add_library(target)
target_include_directories(target PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
)
target_link_libraries(target PUBLIC 
    env
    toolchain
    flatbuffers
    Taskflow
)

target_sources(target PRIVATE
    src/target/target.cpp
    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/lib.cpp
    src/target/build.cpp
    src/target/flags.cpp

    src/target/recheck_states.cpp
    src/target/tasks.cpp

    src/fbs/fbs_loader.cpp
    src/fbs/fbs_storer.cpp

    src/util/util.cpp
    src/util/command.cpp

    include/target.h
    include/internal/fbs_loader.h
    include/internal/path.h
    include/internal/util.h
)
target_include_directories(target PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target PRIVATE ${BUILD_LINK_FLAGS})
add_dependencies(target fbs_to_header)
