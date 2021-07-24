# Target
m_clangtidy("target")
add_library(target
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
    src/util/execute.cpp

    include/target.h
    include/target/fbs_loader.h
    include/target/path.h
    include/target/util.h
    include/target/command.h
)
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
target_link_libraries(target PRIVATE
    tiny-process-library::tiny-process-library
)

target_include_directories(target PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target PRIVATE ${BUILD_LINK_FLAGS})
add_dependencies(target fbs_to_header)
