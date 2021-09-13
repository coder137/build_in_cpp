cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BuildCC 
    DESCRIPTION "Build ASM,C,C++ files in C++"
    HOMEPAGE_URL "https://github.com/coder137/build_in_cpp"
    VERSION 0.1.1.0
    LANGUAGES CXX
)

# User options
option(BUILDCC_INSTALL "Enable Buildcc Installation" ON)
option(BUILDCC_FLATBUFFERS_FLATC "Build Flatbuffer::Flatc Compiler" ON)

option(BUILDCC_BUILD_AS_SINGLE_LIB "Build all internal libs and modules as part of the buildcc library" ON)
option(BUILDCC_BUILD_AS_INTERFACE "Build all internal libs and modules seperately and link" OFF)

# NOTE, Conflict with Clang-Tidy on certain compilers
option(BUILDCC_PRECOMPILE_HEADERS "Enable Buildcc precompile headers" OFF)
option(BUILDCC_EXAMPLES "Enable Buildcc Examples" OFF)

# Dev options
option(BUILDCC_TESTING "Enable BuildCC Testing" OFF)

# Dev Tool options
option(BUILDCC_CLANGTIDY "Enable ClangTidy" OFF)
option(BUILDCC_CPPCHECK "Enable CppCheck" OFF)
option(BUILDCC_DOCUMENTATION "Enable Documentation" OFF)

# Compiler options
# NOTE, This option is required for clang compilers, architecture x86_64-pc-windows-msvc
# Flatbuffers library uses `std::system` internally which causes a deprecated error
option(BUILDCC_NO_DEPRECATED "Disable Deprecated" OFF)
if (${BUILDCC_NO_DEPRECATED})
    add_compile_options("-Wno-deprecated")
endif()

# Testing
set(BUILD_TESTING OFF CACHE BOOL "Third Party modules use these options")
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${BUILDCC_TESTING})
    set(TESTING ON)
    message("Enabling unit-testing")
    message("Compiler identification: ${CMAKE_CXX_COMPILER_ID}")
    include(cmake/flags/test_flags.cmake)
    include(CTest)
    enable_testing()
else()
    set(TESTING OFF)
    message("Unit-Tests only supported on GNU compiler")
    message("Compiler identification: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Tools

# https://gitlab.kitware.com/cmake/cmake/-/issues/20512
# Issue with MSVC CL Compilers
if (${BUILDCC_CLANGTIDY})
message("USER CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
    set(CLANGTIDY ON)
    message("Enabling clang-tidy")
else()
    set(CLANGTIDY OFF)
    message("ClangTidy only supported on GNU compiler")
endif()
include(cmake/tool/clangtidy.cmake)
include(cmake/tool/cppcheck.cmake)
include(cmake/tool/doxygen.cmake)

# Libraries

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
add_subdirectory(flatbuffers)
include(cmake/target/flatbuffers.cmake)

set(FMT_INSTALL ON CACHE BOOL "Fmt install")
add_subdirectory(fmt)
# TODO, Remove fmt library generation and install target
# set_target_properties(fmt PROPERTIES EXCLUDE_FROM_ALL ON)

# set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Spdlog built as dynamic library")
set(SPDLOG_INSTALL ON CACHE BOOL "Spdlog install")
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "Spdlog FMT external lib")
set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "Spdlog FMT header only external lib")
set(SPDLOG_ENABLE_PCH ${BUILDCC_PRECOMPILE_HEADERS} CACHE BOOL "Spdlog PCH")
add_subdirectory(spdlog)
# TODO, Remove spdlog library generation and install target
# set_target_properties(spdlog PROPERTIES EXCLUDE_FROM_ALL ON)

include(cmake/target/cli11.cmake)

set(TF_BUILD_TESTS OFF CACHE BOOL "TF Tests")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "TF Examples")
add_subdirectory(taskflow)

add_subdirectory(tiny-process-library)
include(cmake/target/tpl.cmake)

if (${TESTING})
    set(C++11 ON CACHE BOOL "CppUTests C++11 support")
    set(CPPUTEST_FLAGS OFF CACHE BOOL "CppUTests Flags off")
    set(WERROR ON CACHE BOOL "CppUTests all errors")
    set(LONGLONG ON CACHE BOOL "CppUTests Long Long support")
    set(TESTS OFF CACHE BOOL "CppUTests tests off")
    set(TESTS_BUILD_DISCOVER OFF CACHE BOOL "CppUTests Tests discover")
    set(VERBOSE_CONFIG OFF CACHE BOOL "Config print to screen")
    add_subdirectory(cpputest)
endif()

# Coverage

if (${TESTING})
    include(cmake/coverage/lcov.cmake)
    include(cmake/coverage/gcovr.cmake)
endif()

# Main Project

include(cmake/flags/build_flags.cmake)
add_subdirectory(buildcc)

# Installation

if (${BUILDCC_INSTALL})
    message("Project Version: ${PROJECT_VERSION}")
    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
    include(CPack)
endif()

if (${BUILDCC_EXAMPLES})
    add_subdirectory(example/hybrid/simple)
    add_subdirectory(example/hybrid/foolib)
    add_subdirectory(example/hybrid/external_lib)
    add_subdirectory(example/hybrid/custom_target)
    add_subdirectory(example/hybrid/generic)
endif()
