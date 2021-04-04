cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BuildCC)

option(BUILDCC_TESTING "Enable BuildCC Testing" ON)

if (BUILDCC_TESTING)
    include(CTest)
endif()

# TODO, Update FLATBUFFERS option to conditionally compile FLATC

# Set flatbuffer specific options here
set(FLATBUFFERS_BUILD_CPP17 ON CACHE BOOL "Flatbuffers C++17 support")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Flatbuffers build tests")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "Flatbuffers build flathash")
set(FLATBUFFERS_BUILD_FLATLIB ON CACHE BOOL "Flatbuffers build flatlib")
# set(FLATBUFFERS_BUILD_SHAREDLIB ON CACHE BOOL "Flatbuffers built as dynamic library")
add_subdirectory(flatbuffers)

# set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Spdlog built as dynamic library")
add_subdirectory(spdlog)

set(C++11 ON CACHE BOOL "CppUTests C++11 support")
set(CPPUTEST_FLAGS OFF CACHE BOOL "CppUTests Flags off")
set(WERROR ON CACHE BOOL "CppUTests all errors")
set(LONGLONG ON CACHE BOOL "CppUTests Long Long support")
set(TESTS OFF CACHE BOOL "CppUTests tests off")
set(TESTS_BUILD_DISCOVER OFF CACHE BOOL "CppUTests Tests discover")
add_subdirectory(cpputest)

add_subdirectory(buildcc)

# Custom Targets
include(cmake/target/lcov.cmake)
include(cmake/target/gcovr.cmake)
