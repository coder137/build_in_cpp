set(TARGET_SRCS 

    src/util/util.cpp

    src/fbs/fbs_loader.cpp
    src/fbs/fbs_storer.cpp

    src/target/target.cpp
    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/lib.cpp
    src/target/build.cpp
    src/target/flags.cpp

    src/target/recheck_states.cpp
    src/target/tasks.cpp

    include/target/path.h
    include/target/util.h
    include/target/target_loader.h
    include/target/target.h
)

if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    target_sources(buildcc PRIVATE
        ${TARGET_SRCS}
    )
    target_include_directories(buildcc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_include_directories(buildcc PRIVATE
        ${SCHEMA_BUILD_DIR}
    )
    add_dependencies(buildcc fbs_to_header)
endif()

if(${BUILDCC_BUILD_AS_INTERFACE})
    m_clangtidy("target")
    add_library(target
        ${TARGET_SRCS}
    )
    target_include_directories(target PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${BUILDCC_INSTALL_HEADER_PREFIX}>
    )
    target_link_libraries(target PUBLIC 
        env
        command
        toolchain
        flatbuffers_header_only
        Taskflow
    )

    target_include_directories(target PRIVATE
        ${SCHEMA_BUILD_DIR}
    )
    target_compile_options(target PRIVATE ${BUILD_COMPILE_FLAGS})
    target_link_options(target PRIVATE ${BUILD_LINK_FLAGS})
    add_dependencies(target fbs_to_header)
endif()
