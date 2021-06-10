# Why _this_ lib

My opinion on why I chose a particular Third Party library

> Developers who would like to suggest an alternative library, raise an issue with the **license** and **advantages** clearly outlined.

# Flatbuffer

- [Flatbuffers C++ Benchmarks](https://google.github.io/flatbuffers/flatbuffers_benchmarks.html)
- Serialization is needed for optimized rebuilds
- The previous build is compared with the current build

From [target.fbs](../../buildcc/lib/target/fbs/target.fbs) we can see the different parameters that are tracked

- source files
- header files
- lib deps (built through buildcc)
- external lib deps (passed in through the `-l` flag for prebuilt libraries)
- include directories
- lib directories
- preprocessor flags
- c compile flags
- cpp compile flags
- link flags

Also see [Target::BuildRecompile() in build.cpp](../../buildcc/lib/target/src/target/build.cpp) for rechecks

# Fmtlib and Spdlog

# Taskflow

# CLI11

# CppUTest
