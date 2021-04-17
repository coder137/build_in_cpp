add_library(target)
target_sources(target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/target.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/fbs_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/path.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/util.h
)
target_include_directories(target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(target PRIVATE
    src/target/target.cpp
    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/lib_dep.cpp
    src/target/build.cpp
    src/target/flags.cpp
    src/target/recheck_states.cpp

    src/fbs/fbs_loader.cpp
    src/fbs/fbs_storer.cpp

    src/util/util.cpp
    src/util/command.cpp
)
target_include_directories(target PRIVATE
   ${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
target_link_options(target PRIVATE ${BUILD_LINK_FLAGS})
target_link_libraries(target PRIVATE 
    flatbuffers 
    env
    toolchain
)
add_dependencies(target fbs_to_header)
