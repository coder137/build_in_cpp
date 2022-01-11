add_executable(buildexe
    buildexe.cpp

    src/build_env_home.cpp
    include/buildexe/build_env_home.h

    src/args_setup.cpp
    include/buildexe/args_setup.h

    src/toolchain_setup.cpp
    include/buildexe/toolchain_setup.h

    src/build_env_setup.cpp
    include/buildexe/build_env_setup.h
)
target_sources(buildexe PRIVATE
    ../bootstrap/src/build_buildcc.cpp
    ../bootstrap/src/build_cli11.cpp
    ../bootstrap/src/build_flatbuffers.cpp
    ../bootstrap/src/build_fmtlib.cpp
    ../bootstrap/src/build_spdlog.cpp
    ../bootstrap/src/build_taskflow.cpp
    ../bootstrap/src/build_tpl.cpp
)
target_include_directories(buildexe PRIVATE 
    include
    ../bootstrap/include
)
target_link_libraries(buildexe PRIVATE buildcc)

# TODO, Add this only if MINGW is used
# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(buildexe PRIVATE -Wl,--allow-multiple-definition)
endif()

if (${BUILDCC_INSTALL})
    install(TARGETS buildexe 
    CONFIGURATIONS Release
    RUNTIME DESTINATION bin)
endif()

add_custom_target(run_buildexe_help
    COMMAND buildexe --help-all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# [Immediate Mode] Tpl example WIN MSVC
add_custom_target(run_buildexe_im_tpl_win_msvc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/tpl_win.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_win_msvc.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# [Immediate Mode] Tpl example LINUX GCC
add_custom_target(run_buildexe_im_tpl_linux_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/tpl_linux.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_linux_gcc.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# [Script Mode] Simple example WIN GCC
add_custom_target(run_buildexe_sm_simple_win_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/sm_simple_linux.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_win_gcc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/hybrid/simple
    VERBATIM USES_TERMINAL
)

# [Script Mode] Simple example WIN MSVC
add_custom_target(run_buildexe_sm_simple_win_msvc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/sm_simple_win.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_win_msvc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/hybrid/simple
    VERBATIM USES_TERMINAL
)

# [Script Mode] Simple example LINUX GCC
add_custom_target(run_buildexe_sm_simple_linux_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/sm_simple_linux.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_linux_gcc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/hybrid/simple
    VERBATIM USES_TERMINAL
)

# [Script Mode] BuildExe libs example WIN GCC
add_custom_target(run_buildexe_libs_hybrid_win_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config compile.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_win_gcc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/buildexe/libs
    VERBATIM USES_TERMINAL
)

# [Script Mode] BuildExe libs example WIN MSVC
add_custom_target(run_buildexe_libs_hybrid_win_msvc
    COMMAND buildexe --help-all
    COMMAND buildexe --config compile.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_win_msvc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/buildexe/libs
    VERBATIM USES_TERMINAL
)

# [Script Mode] BuildExe libs example LINUX GCC
add_custom_target(run_buildexe_libs_hybrid_linux_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config compile.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_linux_gcc.toml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../example/buildexe/libs
    VERBATIM USES_TERMINAL
)
