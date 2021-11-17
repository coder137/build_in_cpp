set(TARGET_SRCS 
    # Interfaces
    include/target/loader_interface.h
    include/target/builder_interface.h

    # Utils
    src/util/util.cpp
    include/target/path.h
    include/target/util.h

    # Common
    src/common/target_config.cpp
    src/common/target_state.cpp
    include/target/common/target_file_ext.h
    include/target/common/target_config.h
    include/target/common/target_state.h
    include/target/common/target_env.h
    include/target/common/target_type.h

    # API
    src/api/copy_api.cpp
    include/target/api/copy_api.h

    # Generator
    src/generator/generator_loader.cpp
    src/generator/generator_storer.cpp
    src/generator/generator.cpp
    src/generator/task.cpp
    src/generator/recheck_states.cpp
    include/target/generator_loader.h
    include/target/generator.h

    # Target friend
    src/target/friend/compile_pch.cpp
    src/target/friend/compile_object.cpp
    src/target/friend/link_target.cpp
    include/target/friend/compile_pch.h
    include/target/friend/compile_object.h
    include/target/friend/link_target.h

    # Target
    src/target/target.cpp
    src/target/target_loader.cpp
    src/target/target_storer.cpp
    include/target/target_loader.h
    include/target/target_storer.h
    include/target/target.h

    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/pch.cpp
    src/target/lib.cpp
    src/target/flags.cpp
    src/target/additional_deps.cpp

    src/target/recheck_states.cpp

    src/target/copy.cpp

    src/target/build.cpp
    src/target/tasks.cpp
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
