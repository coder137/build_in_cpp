# Target commands for Compile and Link

When constructing custom commands we need to supply our own pattern to the buildsystem

This is done by overriding the 2 `virtual base::Target` functions

**IMPORTANT: Make sure they are string literals** 
```cpp
std::string_view CompileCommand() const override {
  return "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} -o {output} -c {input}";
}

std::string_view Link() const override {
  return "{cpp_compiler} {link_flags} {compiled_sources} -o {output} {lib_dirs} {lib_deps}";
}
```

- See [GCC specific overrides](../../buildcc/targets/target_gcc.h)
- See [MSVC specific overrides](../../buildcc/targets/target_msvc.h)

# General

The following `{}` commands are available to both `CompileCommand` and `Link` functions

See [build.cpp Target::Build API](../../buildcc/lib/target/src/target/build.cpp)

- `include_dirs`: Aggregated include directories for header files
- `lib_dirs`: Aggregated lib directories for external libraries
- `lib_deps`: External libraries and full path libraries
- `preprocessor_flags`: Preprocessor definitions
- `link_flags`: Flags supplied during linking
- `asm_compiler`: Assembly compiler
- `c_compiler`: C compiler
- `cpp_compiler`: C++ compiler
- `archiver`: Archiver for Static Libraries
- `linker`: Linker usually used during the Linking phase / Library creation

# Compile Specific

See [source.cpp Target::CompileCommand API](../../buildcc/lib/target/src/target/source.cpp)

- `compiler`: Automatically chosen amongst ASM, C and C++ toolchain compiler
- `compile_flags`: Automatically chosen amongst `{c/cpp}_flags`
- `output`: Object file
- `input`: Input source file

# Links Specific

See [build.cpp Target::Target API](../../buildcc/lib/target/src/target/build.cpp)

- `output`: Generated target as `Target::GetName()`
- `compiled_sources`: Aggregated object files
