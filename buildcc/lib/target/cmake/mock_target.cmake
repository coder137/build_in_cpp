add_library(mock_target STATIC
    # Utils
    src/util/util.cpp

    # Generator
    src/generator/generator_loader.cpp
    src/generator/generator_storer.cpp
    src/generator/generator.cpp

    # Generator mocks
    mock/generator/task.cpp
    mock/generator/recheck_states.cpp

    # Target friend
    src/target/friend/file_extension.cpp
    src/target/friend/compile_pch.cpp
    src/target/friend/compile_object.cpp

    # Target
    src/target/target.cpp
    src/target/target_loader.cpp
    src/target/target_storer.cpp

    src/target/source.cpp
    src/target/include_dir.cpp
    src/target/pch.cpp
    src/target/lib.cpp
    src/target/flags.cpp
    src/target/additional_deps.cpp

    src/target/link_target.cpp
    src/target/build.cpp

    # Target mocks
    mock/target/recheck_states.cpp
    mock/target/tasks.cpp
)
target_include_directories(mock_target PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${SCHEMA_BUILD_DIR}
)

target_compile_options(mock_target PUBLIC ${TEST_COMPILE_FLAGS} ${BUILD_COMPILE_FLAGS})
target_link_options(mock_target PUBLIC ${TEST_LINK_FLAGS} ${BUILD_LINK_FLAGS})
target_link_libraries(mock_target PUBLIC 
    flatbuffers_header_only
    Taskflow

    mock_env
    mock_command
    toolchain

    CppUTest
    CppUTestExt
    gcov
)

# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(mock_target PUBLIC -Wl,--allow-multiple-definition)
endif()

add_dependencies(mock_target fbs_to_header)
