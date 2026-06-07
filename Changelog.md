# CHANGELOG

## Long Term goals

- [Discussion] Supported plugin requirements by users
- [Discussion] Customizability requirements by users
- [Discussion] Target and Generator interfaces for standardization by compilers. (White paper)
- [Community Support] MacOS testing and CI/CD

## 0.2.x

- `Append*` APIs
- `Add*WithFormat` or `Append*WithFormat` APIs

## 0.1.4

- Config options updates as per Target requirements
  - Update configs to use ``fmt::format`` with format specifiers for "{prefix}{value}{suffix}" for customizability. For example: `/D{preprocessor}` for msvc or `-D{preprocessor}` for gcc etc
- Target specialized clang
  - Clang behaves differently depending on its backend
  - Option 1: Consider adding more options to ``ToolchainId`` and different Clang specializations. For example: ``Target_gcc_clang`` or ``Target_msvc_clang`` or ``Target_mingw_clang`` etc
  - Option 2: Consider making a ``Target_clang`` that changes behaviour as per the ``target_triple_architecture`` present in the ``toolchain``
  - What other flavours of clang are present?

## 0.1.3

- Make a common interface / internal library which contains all utility functions and libraries
- New generators
  - Currently we only have a simple Generator which is similar to our FileIOGenerator (input -> subprocess commands -> outputs)
  - Read the ``faq`` generators to make more varied and robust generators.

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
