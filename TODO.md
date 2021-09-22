
# Feature

- [ ] Bootstrapping
  - [x] Via CMake (Static Library)
  - [ ] Via CMake (Dynamic Library)
  - [ ] builcc bootstrap (Executable)
  - [ ] Via buildcc bootstrap (Static Library)
  - [ ] Via buildcc bootstrap (Dynamic Library)
- [ ] Command/Subprocess Redirect stdout and stderr for Subprocess
- [ ] PrecompileHeader support
- [ ] C++20 module support
  - Understand flags
  - Understand procedure for GCC, MSVC and Clang
- [ ] Plugin - ClangFormat
- [ ] Plugin - Graph Visualizer

# User QOL

- [ ] Getter APIs for Target
- [ ] Append Setter APIs for Target
- [ ] `find_program` option in Toolchain
- [ ] Custom executables stored in Toolchain 

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
- [ ] Codecoverage
  - [x] Codecov
  - [ ] Coveralls
- [x] Codacy

# Optimization

- [ ] Aggregated strings stored in Target vs `std::insert` on `std::vector` and `std::unordered_set`
- [ ] `std::string` vs `std::string_view` usage
- [ ] C++20 constexpr `std::string` and `std::vector` usage

# Tests

- [x] 100% Line Coverage
- [ ] Improve Branch Coverage
- [ ] Benchmark example CMake vs BuildCC
- [ ] Speed profiling `subprocess` vs `std::system` with gprof and qcachegrind
  - NOTE, Since we have Taskflow for parallel programming, we do not need to construct a multi-threaded subprocess.
  - Subprocess should typically replicate `std::system` functionality while offering better security.

# Examples and Demos

- [ ] Cross compiling
- [ ] Debugging using VSCode
- [ ] Debugging using GDB
