
# Feature

- [ ] Bootstrapping
  - [x] Via CMake
  - [ ] Via buildcc
- [x] Subprocess/Process
  - Tiny Process Library
  - Reproc
  - Ninja Subprocess 
- [ ] Command/Subprocess class to construct a specialized query
  - Currently, `internal::command` uses `std::system` and command tokens are passed in through `std::vector` (no sanitization or security)
  - This class must also be easy enough to be used by users to construct external commands.
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

- [ ] `fs::path::string_type` conversion to `std::string`
  - In windows `fs::path::string_type` is `std::wstring`, discuss potential pitfalls for conversion and storing as `std::string` 
- [ ] Aggregated strings stored in Target vs `std::insert` on `std::vector` and `std::unordered_set`
- [ ] Handling exceptions and generating fatal exceptions
  - Discuss different strategies for exceptions i.e throw, std::error_code etc 
- [ ] `std::string` vs `std::string_view` usage
- [ ] Static library vs Shared Library when linking buildcc (See **Software Architecture** section)
  - Static library linking is extremely slow on certain compilers
- [ ] Third party library optimization
  - flatbuffers
  - Do not create static/dynamic **fmt** or **spdlog** library, use INTERFACE header only library. We might need to write our own cmake script for INTERFACE and INSTALL.
- [ ] C++20 constexpr `std::string` and `std::vector` usage

# Tests

- [ ] 100% Line Coverage
- [ ] Improve Branch Coverage
- [ ] Benchmark example CMake vs BuildCC
- [ ] Speed profiling `subprocess` vs `std::system` with gprof and qcachegrind
  - NOTE, Since we have Taskflow for parallel programming, we do not need to construct a multi-threaded subprocess.
  - Subprocess should typically replicate `std::system` functionality while offering better security.

# Examples and Demos

- [ ] Cross compiling
- [ ] Debugging using VSCode
- [ ] Debugging using GDB
