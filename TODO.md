
# Feature

- [ ] Bootstrapping
  - [x] Via CMake
  - [ ] Via buildcc
- [ ] Subprocess
  - Reproc
  - Subprocess.h
  - Ninja Subprocess 
- [ ] Plugin - ClangFormat
- [ ] Plugin - Graph Visualizer
- [ ] PrecompileHeader support
- [ ] C++20 module support
  - Understand flags
  - Understand procedure for GCC, MSVC and Clang

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
- [ ] Codecov
- [ ] Codacy

# Optimization

- [ ] Aggregated strings stored in Target vs `std::insert` on `std::vector` and `std::unordered_set`
- [ ] `std::string` vs `std::string_view` usage
- [ ] Static library vs Shared Library when linking
  - Static library linking is extremely slow on certain compilers
- [ ] Third party library optimization
  - spdlog
  - flatbuffers
- [ ] C++20 constexpr `std::string` and `std::vector` usage

# Tests

- [ ] 100% Line Coverage
- [ ] Benchmark example CMake vs BuildCC
- [ ] Speed profiling `subprocess` vs `std::command` with gprof and qcachegrind

# Examples and Demos

- [ ] Cross compiling
- [ ] Debugging using VSCode
- [ ] Debugging using GDB
