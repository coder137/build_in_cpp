add_executable(buildcc_lib_bootstrap
    main.buildcc.cpp
)
target_sources(buildcc_lib_bootstrap PRIVATE
    src/build_flatbuffers.cpp
    src/build_nlohmann_json.cpp
    src/build_cli11.cpp
    src/build_fmtlib.cpp
    src/build_spdlog.cpp
    src/build_taskflow.cpp
    src/build_tpl.cpp
    src/build_tl_optional.cpp

    src/build_buildcc.cpp
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

# Linux GCC
add_custom_target(run_buildcc_lib_bootstrap_linux_gcc
    COMMAND buildcc_lib_bootstrap --help-all
    COMMAND buildcc_lib_bootstrap --config ${CMAKE_CURRENT_SOURCE_DIR}/config_default.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/config/toolchain_linux_gcc.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# Win GCC/MINGW
add_custom_target(run_buildcc_lib_bootstrap_win_gcc
    COMMAND buildcc_lib_bootstrap --help-all
    COMMAND buildcc_lib_bootstrap --config ${CMAKE_CURRENT_SOURCE_DIR}/config_default.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/config/toolchain_win_gcc.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# Win MSVC
add_custom_target(run_buildcc_lib_bootstrap_win_msvc
    COMMAND buildcc_lib_bootstrap --help-all
    COMMAND buildcc_lib_bootstrap --config ${CMAKE_CURRENT_SOURCE_DIR}/config_default.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/config/toolchain_win_msvc.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)
