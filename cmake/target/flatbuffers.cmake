
# TODO, Update FLATBUFFERS option to conditionally compile FLATC

# Set flatbuffer specific options here
set(FLATBUFFERS_BUILD_CPP17 ON CACHE BOOL "Flatbuffers C++17 support")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Flatbuffers build tests")
set(FLATBUFFERS_BUILD_FLATC ${BUILDCC_FLATBUFFERS_FLATC} CACHE BOOL "Flatbuffers build flatc")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "Flatbuffers build flathash")
set(FLATBUFFERS_BUILD_FLATLIB ON CACHE BOOL "Flatbuffers build flatlib")
set(FLATBUFFERS_LIBCXX_WITH_CLANG OFF CACHE BOOL "Flatbuffers LIBCXX")
set(FLATBUFFERS_INSTALL ON CACHE BOOL "Enable the installation of targets.")
set(FLATBUFFERS_ENABLE_PCH ${BUILDCC_PRECOMPILE_HEADERS} CACHE BOOL "Flatbuffers PCH")
add_subdirectory(third_party/flatbuffers)
