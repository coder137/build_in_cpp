add_executable(buildcc_lib_bootstrap
    main.buildcc.cpp
)
target_sources(buildcc_lib_bootstrap PRIVATE
    src/buildcc_flatbuffers.cpp
    src/buildcc_cli11.cpp
    src/buildcc_fmtlib.cpp
    src/buildcc_spdlog.cpp
    src/buildcc_taskflow.cpp
    src/buildcc_tpl.cpp
)
target_include_directories(buildcc_lib_bootstrap PRIVATE
    include
)
target_link_libraries(buildcc_lib_bootstrap PRIVATE buildcc)

# TODO, Add this only if MINGW is used
# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(buildcc_lib_bootstrap PRIVATE -Wl,--allow-multiple-definition)
endif()

add_custom_target(run_buildcc_lib_bootstrap_linux
    COMMAND buildcc_lib_bootstrap --help-all
    COMMAND buildcc_lib_bootstrap --config ${CMAKE_CURRENT_SOURCE_DIR}/config_linux.toml
    # COMMAND dot -Tpng graph.dot -o graph.PNG
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)
