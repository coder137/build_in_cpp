cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Build_in_Cpp)

# Set flatbuffer specific options here
set(FLATBUFFERS_BUILD_CPP17 ON CACHE BOOL "Flatbuffers C++17 support")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "Flatbuffers disable tests")
add_subdirectory(flatbuffers)
