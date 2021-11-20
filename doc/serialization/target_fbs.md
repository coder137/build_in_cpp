# Understanding `target.fbs`

See [target.fbs](../../buildcc/schema/target.fbs)

## Definition

See Target APIs present in the [../../buildcc/lib/target/include/target/api](../../buildcc/lib/target/include/target/api) folder

- A Target is two stage procedure consisting of compiling (preprocessing + compiling) and linking.
- During compiling we can compile 
    - Header files to PCH files
    - Source files to Object files
- During linking we take our objects and create a Target

## Correlation with schema file

### Metadata

- `type` is the ssociated target type
    - Executable
    - Static Library
    - Dynamic Library
    - C++20 Modules (Future)

### Physically present files

- `source_files` are input files that will be compiled to object files
- `header_files` are tracked for better rebuilds
    - See [faq/include_dir_vs_header_files](../faq/include_dir_vs_header_files.md)
- `pch_files` are tracked for certain headers that need to be aggregated to a single PCH output.
    - If any one of these pch files change we need to recompile our PCH output.
- `lib_deps` are Targets of type 
    - Static Library
    - Dynamic Library
    - C++20 Modules (Future)

### Links

- `external_lib_deps` are library links
    - For example: -lpthread, -lm in GCC

### Directories

- `include_dirs` are include directories to find header files used in source files.
- `lib_dirs`: are library directories to find external lib deps during linking.

### Flags

- `preprocessor_flags` are preprocessor macros added during compiling
- `common_compile_flags` are compile flags added to PCH, ASM, C and C++ sources
- `pch_compile_flags` are compile flags only added
    - if pch is enabled i.e pch files are added
    - during pch compile command stage
- `pch_object_flags` are compile flags only added
    - if pch is enabled i.e pch files are added 
    - during object compile command stage
    - NOTE: This is because most source -> object files depend on the generated pch files
- `asm_compile_flags` are specific compile flags added to ASM sources
- `c_compile_flags` are specific compile flags added to C sources
- `cpp_compile_flags` are specific compile flags added to CPP sources
- `link_flags` are specific link flags added during `link_command` stage

### User defined dependencies

- `compile_dependencies` are user compile dependencies that cause all sources to recompile if any compile dependency is REMOVED, ADDED or UPDATED
- `link_dependencies` are user link dependencies that cause target to recompile if any link dependency is REMOVED, ADDED or UPDATED

> TODO, Add pch_dependencies
