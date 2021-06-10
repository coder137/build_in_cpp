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

- Provides helpful utility functions
- `flatbuffer` can be used to generate strict + efficient json
  - `flatbuffer` has a few limitations when generation JSON
  - For example: an array cannot be a root when writing your schema.
- `flexbuffer` can be used to generate flexibile + efficient JSON.

# Fmtlib and Spdlog

- Fmtlib and Spdlog are extremely popular libraries for formatting and logging.
- Fmtlib is also used as a dependency in Spdlog which helps reduce library interdependencies.

# Taskflow

- Very efficient and easy to use parallel programming APIs
- Easy to setup dependencies between two or more `Tasks` or `Taskflows`
  - This ensures that parts of the build that need to be independent can be built in parallel.
  - Automatically uses the C++ Thread library for speedup.

# CLI11

- Full feature argument parsing library
- Has support for custom subcommands
- Has support for configuration files in `.toml` or `.ini` format for ease of use
- Also has future support for merging more than one configuration file similar to `Meson build`. [CLI11 issue to merge more than 1 configuration file](https://github.com/CLIUtils/CLI11/issues/486)

# CppUTest

- See [Unit-Testing and Mocking list](https://github.com/coder137/build_in_cpp/issues/3)
- Google Test is amazing but Google mock is not as flexible
  - To create mocks in Google mock everything must be made virtual in a class
  - Google mock can also be intrusive in certain situations. I did not want ANY dependency to testing and mocking frameworks in the core code base.
- Catch2 is another alternative to Google Test, unfortunately it does not have its own mocking framework and would need one of the following.
  - Trompeloeil (Compared Catch2 + Trompoliel to the CppUTest suite)
  - FFF (More useful to mock out C code)
  - FakeIt (Similar to Google mock)

## Advantages

- CppUTest and CppUMock is provided as a single package with great compatibility.
- Less number of library dependencies to manage
- CppUMock is extremely customizable

## Disadvantages

- CppUTest does not have AS MANY features as Google Test
- CppUMock needs a lot of boilerplate (read _customizable_)
  - Thankfully mocking is used very sparingly in the code base

## Usage

Unit-Tests and Mocking is used to check the **behaviour** of the core Target class i.e (Compile and Link strategies).

- CppUTest is used for general assertions
- CppUMock is used for mock out underlying process/system calls and instead verify that the function is being called accurately.
- CppUMock is also used to verify rebuild states and to see if certain parts of the code are being accurately hit (through `expect` APIs).
