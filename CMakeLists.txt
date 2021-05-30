cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BuildCC 
    DESCRIPTION "Build ASM,C,C++ files in C++"
    HOMEPAGE_URL "https://github.com/coder137/build_in_cpp"
    VERSION 0.1.1.0
    LANGUAGES CXX
)

option(BUILDCC_TESTING "Enable BuildCC Testing" OFF)
option(BUILDCC_GCC_TESTING "Enable Buildcc GCC Testing" OFF)
option(BUILDCC_INSTALL "Enable Buildcc Installation" ON)
option(BUILDCC_EXAMPLES "Enable Buildcc Examples" OFF)

option(BUILDCC_CLANGTIDY "Enable ClangTidy" ON)

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

# https://gitlab.kitware.com/cmake/cmake/-/issues/20512
# Issue with MSVC CL Compilers
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${BUILDCC_CLANGTIDY})
    set(CLANGTIDY ON)
    message("Enabling clang-tidy")
else()
    set(CLANGTIDY OFF)
    message("ClangTidy only supported on GNU compiler")
endif()
include(cmake/tool/clangtidy.cmake)

# TODO, Update FLATBUFFERS option to conditionally compile FLATC

# Set flatbuffer specific options here
set(FLATBUFFERS_BUILD_CPP17 ON CACHE BOOL "Flatbuffers C++17 support")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Flatbuffers build tests")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "Flatbuffers build flathash")
set(FLATBUFFERS_BUILD_FLATLIB ON CACHE BOOL "Flatbuffers build flatlib")
set(FLATBUFFERS_LIBCXX_WITH_CLANG OFF CACHE BOOL "Flatbuffers LIBCXX")
# set(FLATBUFFERS_BUILD_SHAREDLIB ON CACHE BOOL "Flatbuffers built as dynamic library")
add_subdirectory(flatbuffers)

set(FMT_INSTALL ON CACHE BOOL "Fmt install")
add_subdirectory(fmt)

# set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Spdlog built as dynamic library")
set(SPDLOG_INSTALL ON CACHE BOOL "Spdlog install")
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Spdlog FMT external lib")
add_subdirectory(spdlog)

include(cmake/target/cli11.cmake)

set(TF_BUILD_TESTS OFF CACHE BOOL "TF Tests")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "TF Examples")
add_subdirectory(taskflow)

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

include(cmake/flags/build_flags.cmake)
add_subdirectory(buildcc)

# Custom Targets
if (${TESTING})
    include(cmake/coverage/lcov.cmake)
    include(cmake/coverage/gcovr.cmake)
endif()

if (${BUILDCC_INSTALL})
    message("Project Version: ${PROJECT_VERSION}")
    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
    include(CPack)
endif()

if (${BUILDCC_EXAMPLES})
    add_subdirectory(example/hybrid)
endif()
