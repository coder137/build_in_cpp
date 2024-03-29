# Examples

## Definition

- HOST: Your host operating system (Linux, Windows, Mac ...) and its compiler suite (GCC, MSVC, Clang ...).
- TARGET: The build targets that are generated using different compilers i.e GNU, MSVC, Clang, Cross compilation toolchains etc

## Steps

1. Since we are writing our build 'scripts' in C++, our `build.cpp` first needs to be **COMPILED** (Similar to CMake generate)
2. Next we need to **RUN** our `build.exe` executable (Similar to CMake build)

> Think compile(buildcc)/generate(cmake) -> run(buildcc)/build(cmake)

```bash
# Compile build.cpp === Generate CMake project
g++ build.cpp -o build.exe -lbuildcc.a -ltarget.a ...
cmake -B build -G Ninja

# Run ./build.exe === Build CMake project
./build.exe --config build.toml --options
cmake --build build
```

## Compilers

- GCC/GNU
- MSVC
- Clang

> TODO, Add more compilers

## Operating Systems

- [x] Windows
  - [x] MSYS MINGW 10.2.0
  - [x] MSVC v16.8.2
  - [x] Clang (x86_64-w64-windows-gnu) 11.0 
  - [x] Clang (x86_64-pc-windows-msvc) 11.0
- [x] WSL (Ubuntu 20.04 LTS)
  - [x] GCC 9.3.0
  - [x] Clang (x86_64-pc-linux-gnu) 10.0.0
- [ ] Mac

> TODO, Test buildcc on more operating systems and compilers

## Basic Steps

- Write your `build.cpp` 'script'
- CMake is used to bootstrap and compile the executable, ex. `build.cpp` -> `./build.exe`
- Run the **build executable** to generate your targets (executable, static/dynamic libs)
- Every **target** should use a compatible **toolchain**
  - gcc targets should supply appropriate gcc toolchain
  - custom targets should supply appropriate custom toolchain
  - NOTE: Make sure that these toolchains are added to system PATH

> For MSVC, Make sure you use `vcvarsall.bat` to initialize your environment

# Proof of Concept Tests

Multi hosts but only one target compiler used

> NOTE, See the distinction between **HOST** and **TARGET**

## [target] Gcc

- [x] Simple
  - Only 1 source file 
- [x] IncludeDir
  - 1 source + 1 include directory 
- [x] StaticLib
  - Static lib + Executable 
- [x] DynamicLib
  - Dynamic Lib + Executable 
- [x] Flags
  - Custom Preprocessor, C/CPP, Link flags 
- [x] AfterInstall
  - Installing buildcc via CMake and consuming packages 
- [x] Plugins
  - Community based plugin support poc 
  - [x] ClangCompileCommands 
  - [ ] ClangFormat
  - [ ] Taskflow graph visualizer

## [target] MSVC

> TODO, Understand how MSVC compilation using `cl.exe` occurs

- [x] Executable
  - Similar to flags example 
- [x] StaticLib
  - MSVC StaticLib + Executable 
- [x] DynamicLib
  - MSVC DynamicLib + Executable 

## [target] MinGW

- [x] Executable
- [x] StaticLib
- [x] DynamicLib

# Real world Tests (Hybrid)

Multi hosts and multi targets

- For actual testing, use these examples as a starting point
- The proof of concept examples are low level tests

> NOTE, See the distinction between **HOST** and **TARGET**

- Tested with **host** windows and linux with **host compilers** gcc, clang and msvc to generate **hybrid targets** using different compilers i.e gcc, clang and msvc

**Current state of examples**

- [x] Single
  - Compile a single source with `Register` and `Args` module
- [x] Simple
  - Similar to Flags example with `Register` and `Args` module 
- [x] Foolib
  - For library developers 
- [x] External Lib
  - For end users consuming third party libraries 
- [x] Custom Target
  - For super customized targets and toolchains 
- [x] Generic
  - Select library type and target-toolchain type at runtime
- [x] PCH
  - Precompile header usage with GCC and MSVC compilers 
- [x] Dependency Chaining
  - Chain `Generators` and `Targets` using the `Register` module 
- [x] Target Info
  - Target Info usage to store Target specific information
  - Example usage for Header Only targets, however it can store information for all Target inputs
  - Common information used between multiple targets can be stored into a `TargetInfo` instance
- [ ] Debugging
- [ ] Cross Compilation with Unit Testing
