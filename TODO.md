# Versions

# 0.1.1

Complete working proof of concept of the following

- BuildCC library
- BuildCC bootstrap "script" files (Basic)
- BuildExe executable (Standalone)

Contains the following working features

**BuildCC**
- Supported plugin
  - Clang Compile Commands
- Toolchain, Generator, TargetInfo and Target interfaces
- Specialized Toolchain for GCC, MSVC and MINGW
- Specialized Target for GCC, MSVC and MINGW

**BuildExe**
- Immediate mode
- Script mode
- Local Package Manager with git

## 0.1.2

- Serialization Interface
- Namespace changes
  - Remove ``buildcc::env``
  - We should only have 3 namespaces ``buildcc``, ``buildcc::plugin`` and ``buildcc::internal``
- Environment updates
  - Remove ``buildcc::env``
  - Refactor free / static functions and variables into classes with static members and variables. For example. ``buildcc::env::init`` should become ``buildcc::Environment::Init``
- Args and Register module updates
  - Pch command from command line
  - Make Register functions static. ``Register::Build``
  - Update ``CallbackIf``, ``Build`` and ``Test`` APIs for the ``state`` variable usage
- Unit testing and mocking for BuildExe

## 0.1.3

- Make a common interface / internal library which contains all utility functions and libraries
- New generators
  - Currently we only have a simple Generator which is similar to our FileIOGenerator (input -> subprocess commands -> outputs)
  - Read the ``faq`` generators to make more varied and robust generators.

## 0.1.4

- Config options updates as per Target requirements
  - Update configs to use ``fmt::format`` with format specifiers for "{prefix}{value}{suffix}" for customizability. For example: `/D{preprocessor}` for msvc or `-D{preprocessor}` for gcc etc
- Target specialized clang
  - Clang behaves differently depending on its backend
  - Option 1: Consider adding more options to ``ToolchainId`` and different Clang specializations. For example: ``Target_gcc_clang`` or ``Target_msvc_clang`` or ``Target_mingw_clang`` etc
  - Option 2: Consider making a ``Target_clang`` that changes behaviour as per the ``target_triple_architecture`` present in the ``toolchain``
  - What other flavours of clang are present?

## 0.2.x

- `Append*` APIs
- `Add*WithFormat` or `Append*WithFormat` APIs

## Long Term goals

- [Discussion] Supported plugin requirements by users
- [Discussion] Customizability requirements by users
- [Discussion] Target and Generator interfaces for standardization by compilers. (White paper)
- [Community Support] MacOS testing and CI/CD

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
