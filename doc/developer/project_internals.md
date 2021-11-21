# Project Internals

## Build

### CMakePresets (from Version 3.20)

- See `CMakePresets.json` for GCC, MSVC and Clang configurations
```bash
# Generating
cmake --list-presets
cmake --preset=[your_preset]

# Building
cmake --build --list-presets
cmake --build --preset=[your_preset]

# Testing (ONLY supported on gcc)
ctest --preset=gcc_dev_all
```

### Custom Targets

```bash
# Run custom target using
cd [folder]
cmake --build . --target [custom_target]
```

**Tools**
- cppcheck_static_analysis
- doxygen_documentation
- gcovr_coverage
- lcov_coverage

**Examples**
- run_hybrid_simple_example_linux
- run_hybrid_simple_example_win
- run_hybrid_foolib_example_linux
- run_hybrid_foolib_example_win
- run_hybrid_externallib_example_linux
- run_hybrid_externallib_example_win
- run_hybrid_customtarget_example_linux
- run_hybrid_customtarget_example_win

## Install

- See the **user installation** section above

- Read [Install target](buildcc/lib/target/cmake/target_install.cmake)

Basic Installation steps
- Install `TARGETS`
- Install `HEADER FILES`
- Export `CONFIG`

## Test

- Read [Mock env](buildcc/lib/env/CMakeLists.txt)
- Read [Mock target](buildcc/lib/target/cmake/mock_target.cmake)
- Read [Test path](buildcc/lib/target/test/path/CMakeLists.txt)
- Read [Test target](buildcc/lib/target/test/target/CMakeLists.txt)
