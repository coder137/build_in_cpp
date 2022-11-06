
# Installation using CMake

## BuildCC User options

- BUILDCC_INSTALL: ON
- BUILDCC_BUILD_AS_SINGLE_LIB: ON
  - Generates `libbuildcc`
- BUILDCC_BUILD_AS_INTERFACE_LIB: OFF
  - Generates `libbuildcc_i` with other `lib`s linked during compilation 
- BUILDCC_PRECOMPILE_HEADERS: OFF
- BUILDCC_EXAMPLES: OFF
  - Uses SINGLE_LIB for its examples
- BUILDCC_TESTING: ON
  - Unit testing with `ctest --output-on-failure`
  - Only active for GCC compilers
  - Provides code coverage
  - `cmake --build {builddir} --target lcov_coverage` (on linux ONLY)
  - `cmake --build {builddir} --target gcovr_coverage` (installed via pip gcovr)
- BUILDCC_CLANGTIDY: ON
  - Auto runs with CMake
- BUILDCC_CPPCHECK: ON
  - Cppcheck with `cmake --build {builddir} --target cppcheck_static_analysis`
- BUILDCC_DOCUMENTATION: ON
  - Basic Doxygen generated html pages
  - `cmake --build {builddir} --target doxygen_documentation`
- BUILDCC_NO_DEPRECATED: OFF
  - Required on certain clang arch compilers `-Wno-deprecated` flag

## Build

> NOTE: Currently, BuildCC needs to be built from source and bootstrapped using CMake.

> I aim to bootstrap BuildCC into an executable to remove the dependency on CMake.

- By default all the developer options are turned OFF.
- Only the `BUILDCC_INSTALL` option is turned on.

```bash
# Generate your project
cmake -B [Build folder] -G [Generator]
cmake -B build -G Ninja

# Build your project
cmake --build build
```

## Install

```bash
# Manually
cd [build_folder]
sudo cmake --install .

# Cpack generators
cpack --help

# ZIP
cpack -G ZIP

# Executable
cpack -G NSIS
```

> NOTE: On windows [NSIS](https://nsis.sourceforge.io/Main_Page) needs to be installed

- Install the package and add to environment PATH
- As a starting point, go through the **gcc/AfterInstall** example and **Hybrid** examples
- For more details read the `examples` README to use buildcc in different situations
