# Target Segregation

- [x] `target.cpp`
  - Initialization
  - Common target specific utility functions
  - Assertion functions
  - Validity checks
- [x] `base/target_loader.cpp`
  - Load from flatbuffer schema file
- [x] `base/target_storer.cpp`
  - Store to flatbuffer schema file

## Target API

Check the `include/target/api` and `src/api` folder

- [x] Copy
  - Copy src target to dest
  - Selective copy using `std::initializer<CopyOption>` 
  - `copy_api`
- [x] Source
  - `source_api` 
- [x] Header and Include Dir
  - `include_api` 
- [x] Lib and Lib Dir
  - `lib_api`
- [x] PCH
  - `pch_api`
- [x] Flags
  - `flag_api`
  - PreprocessorFlags
  - CommonCompileFlags
  - AsmCompileFlags
  - CCompileFlags
  - CppCompileFlag
  - LinkFlags
- [ ] Rebuild Deps
- [ ] Getters
- [ ] Target Info

## Inputs to Target

- [x] `additional_deps.cpp`
  - PreCompileHeader dependencies
  - Compile dependencies
  - Link dependencies 

## Target states

- [x] `recheck_states.cpp`
  - Add Recheck callback during mock calls
  - these callback functions are left blank during in release (only used during unit tests)
- Functions that are mocked during unit tests
  - SourceRemoved
  - SourceAdded
  - SourceUpdated
  - PathRemoved
  - PathAdded
  - PathUpdated
  - DirChanged
  - FlagChanged
  - ExternalLibChanged

## Target friend

- [x] file_extension.cpp
- [x] compile_pch.cpp
- [x] compile_object.cpp
- [x] link_target.cpp

## Action on Target

- [x] `build.cpp`
  - Setup tasks by using the above files
  - [ ] PrecompileHeader (pch) files
  - [x] Object
  - [x] Target
- [x] `tasks.cpp`
  - Runs the tasks using Taskflow
  - Contains mock equivalent for CppUTest
