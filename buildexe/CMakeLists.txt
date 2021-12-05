add_executable(buildexe
    buildexe.cpp
)
target_link_libraries(buildexe PRIVATE buildcc)

# TODO, Add this only if MINGW is used
# https://github.com/msys2/MINGW-packages/issues/2303
# Similar issue when adding the Taskflow library
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(buildexe PRIVATE -Wl,--allow-multiple-definition)
endif()

add_custom_target(run_buildexe_help
    COMMAND buildexe --help-all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# [Immediate Mode] Tpl example WIN MSVC
add_custom_target(run_buildexe_im_tpl_win_msvc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/tpl_win.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_msvc_win.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)

# [Immediate Mode] Tpl example LINUX GCC
add_custom_target(run_buildexe_im_tpl_linux_gcc
    COMMAND buildexe --help-all
    COMMAND buildexe --config ${CMAKE_CURRENT_SOURCE_DIR}/example_configs/tpl_linux.toml --config ${CMAKE_CURRENT_SOURCE_DIR}/../bootstrap/config/toolchain_gcc_linux.toml 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM USES_TERMINAL
)
