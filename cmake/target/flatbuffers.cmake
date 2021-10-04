
# TODO, Update FLATBUFFERS option to conditionally compile FLATC

# Set flatbuffer specific options here
set(FLATBUFFERS_BUILD_CPP17 ON CACHE BOOL "Flatbuffers C++17 support")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Flatbuffers build tests")
set(FLATBUFFERS_BUILD_FLATC ${BUILDCC_FLATBUFFERS_FLATC} CACHE BOOL "Flatbuffers build flatc")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "Flatbuffers build flathash")
set(FLATBUFFERS_BUILD_FLATLIB OFF CACHE BOOL "Flatbuffers build flatlib")
set(FLATBUFFERS_LIBCXX_WITH_CLANG OFF CACHE BOOL "Flatbuffers LIBCXX")
set(FLATBUFFERS_INSTALL ON CACHE BOOL "Enable the installation of targets.")
set(FLATBUFFERS_ENABLE_PCH ${BUILDCC_PRECOMPILE_HEADERS} CACHE BOOL "Flatbuffers PCH")
add_subdirectory(third_party/flatbuffers)

set(FBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include/flatbuffers")
set(FBS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include")
file(GLOB FLATBUFFERS_INCLUDE_HEADERS "${FBS_DIR}/*.h")
if(${BUILDCC_PRECOMPILE_HEADERS})
    list(APPEND FLATBUFFERS_INCLUDE_HEADERS ${FBS_DIR}/pch/pch.h)
endif()

add_library(flatbuffers_header_only INTERFACE
    ${FLATBUFFERS_INCLUDE_HEADERS}
)
target_include_directories(flatbuffers_header_only INTERFACE 
    $<BUILD_INTERFACE:${FBS_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if (${BUILDCC_INSTALL})
    install(TARGETS flatbuffers_header_only DESTINATION lib EXPORT flatbuffers_header_onlyConfig)
    install(DIRECTORY ${FBS_INCLUDE_DIR} DESTINATION "include")
    install(EXPORT flatbuffers_header_onlyConfig DESTINATION "lib/cmake/flatbuffers_header_only")
endif()
