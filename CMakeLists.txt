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
include(cmake/target/flatbuffers.cmake)

include(cmake/target/fmt.cmake)

include(cmake/target/spdlog.cmake)

include(cmake/target/cli11.cmake)

include(cmake/target/taskflow.cmake)

include(cmake/target/tpl.cmake)

if (${TESTING})
    include(cmake/target/cpputest.cmake)
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
    add_subdirectory(example/hybrid/pch)
endif()
