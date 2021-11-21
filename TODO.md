
# Feature

- [ ] Bootstrapping
  - CMake is used to create BuildCC
  - We now create a BuildCC executable that creates BuildCC
  - [ ] BuildCC bootstrap executable through CMake (Static Libraries during linkage)
  - [ ] BuildCC bootstrap executable through CMake (Dynamic Libraries during linkage)
  - [ ] BuildCC bootstrap executable through BuildCC bootstrap executable (similar to the CMake executable)
- [ ] C++20 module support
  - Understand flags
  - Understand procedure for GCC, MSVC and Clang
- [ ] Plugin - BuildCCFind
  - Find executable
  - Find toolchain
- [ ] Plugin - ClangFormat
- [ ] Plugin - Graph Visualizer

# User QOL

- [ ] Append Setter APIs for Target

# Developer Tools

- [ ] Doxygen
  - Online documentation (read the docs)
  - Github pages
- [ ] CI/CD
  - [ ] Linux
    - [x] GCC
    - [ ] Clang
  - [ ] Windows
    - [x] MSVC
    - [x] Clang
    - [ ] MinGW
  - [ ] MacOS
- [x] Codacy

# Optimization

- [ ] Speed vs Memory considerations
  - Currently the project favours speed over memory usage
- [ ] `std::string` vs `std::string_view` vs `const char *` usage
  - NOTE, We cannot convert between `std::string_view` and `const char *` which makes it harder to use `std::string_view` for certain APIs

# Tests

- [ ] Improve Branch Coverage
- [ ] Profiling BuildCC using [Tracy](https://github.com/wolfpld/tracy)
- [ ] Speed comparison between CMake and BuildCC (Release)
- [ ] Speed profiling `subprocess` vs `std::system` with gprof and qcachegrind
  - NOTE, Since we have Taskflow for parallel programming, we do not need to construct a multi-threaded subprocess.
  - Subprocess should typically replicate `std::system` functionality while offering better security.

# Examples and Demos

- [ ] Cross compiling
- [ ] Debugging using VSCode
- [ ] Debugging using GDB
  - Check the [GDBFrontend](https://github.com/rohanrhu/gdb-frontend) project
