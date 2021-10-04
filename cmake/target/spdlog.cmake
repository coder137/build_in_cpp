# set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Spdlog built as dynamic library")
set(SPDLOG_INSTALL ON CACHE BOOL "Spdlog install")
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "Spdlog FMT external lib")
set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "Spdlog FMT header only external lib")
set(SPDLOG_ENABLE_PCH ${BUILDCC_PRECOMPILE_HEADERS} CACHE BOOL "Spdlog PCH")
add_subdirectory(third_party/spdlog)
# TODO, Remove spdlog library generation and install target
# set_target_properties(spdlog PROPERTIES EXCLUDE_FROM_ALL ON)
