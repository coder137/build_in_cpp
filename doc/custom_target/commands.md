# Target commands for Compile and Link

When constructing custom commands we need to supply our own pattern to the buildsystem

This is done by overriding the 3 `base::Target::Config` strings

```cpp
base::Target::Config config;

config.pch_command = "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {pch_flags} {compile_flags} -o {output} -c {input}";

config.compile_command = "{compiler} {preprocessor_flags} {include_dirs} {common_compile_flags} {compile_flags} -o {output} -c {input}";

config.link_command = "{cpp_compiler} {link_flags} {compiled_sources} -o {output} {lib_dirs} {lib_deps}";
```

- See [GCC specific overrides](../../buildcc/targets/include/targets/target_gcc.h)
- See [MSVC specific overrides](../../buildcc/targets/include/targets/target_msvc.h)

# General

The following `{}` commands are available to both `pch_command`, `compile_command` and `link_command`

See [build.cpp Target::Build API](../../buildcc/lib/target/src/target/build.cpp)

- `include_dirs`: Aggregated include directories for header files
- `lib_dirs`: Aggregated lib directories for external libraries
- `preprocessor_flags`: Preprocessor definitions
- `common_compile_flags`: Common compile flags for `PCH`, `ASM`, `C` and `CPP` files
- `link_flags`: Flags supplied during linking
- `asm_compiler`: Assembly compiler
- `c_compiler`: C compiler
- `cpp_compiler`: C++ compiler
- `archiver`: Archiver for Static Libraries
- `linker`: Linker usually used during the Linking phase / Library creation

> NOTE, When PCH is not used these flags are aggregated to an empty string ("")

- `pch_compile_flags`: PCH flags applied when compiling a PCH
- `pch_object_flags`: PCH flags applied to object files after compiling a PCH

# PCH Specific

See [compile_pch.cpp Target::ConstructPchCompileCommand API](../../buildcc/lib/target/src/target/compile_pch.cpp)

- `compiler`: Selects CPP compiler if project contains CPP source else C compiler
- `pch_flags`: Aggregated pch specific flags
- `compile_flags`: Selects CPP flags if project contains CPP source else C flags
- `output`: PCH output path
- `input`: PCH input generated path (Headers are aggregated into a .h file)

# Compile Specific

See [compile_source.cpp Target::ConstructCompileCommand API](../../buildcc/lib/target/src/target/compile_source.cpp)

- `compiler`: Automatically chosen amongst ASM, C and C++ toolchain compiler
- `compile_flags`: Automatically chosen amongst `{c/cpp}_flags`
- `output`: Object file
- `input`: Input source file

# Links Specific

See [link_target.cpp Target::ConstructLinkCommand API](../../buildcc/lib/target/src/target/link_target.cpp)

- `output`: Generated target as `Target::GetName()`
- `compiled_sources`: Aggregated object files
- `lib_deps`: External libraries and full path libraries
