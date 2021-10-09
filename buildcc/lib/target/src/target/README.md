# Target Segregation

- [x] `target.cpp`
  - Initialization
  - Common target specific utility functions
  - Assertion functions
  - Validity checks
- [x] `target_loader.cpp`
  - Load from flatbuffer schema file
- [x] `target_storer.cpp`
  - Store to flatbuffer schema file

## Inputs to Target

- [x] `source.cpp`
  - Source File
- [x] `include_dir.cpp`
  - Include Dir
  - Header File
- [x] `lib.cpp`
  - Lib Dir
  - Lib File (full path Target supplied)
  - External Lib File (relative link using -l)
- [x] `flags.cpp`
  - PreprocessorFlags
  -  CommonCompileFlags
  -   AsmCompileFlags
  -   CCompileFlags
  -   CppCompileFlag
  -   LinkFlags
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

## Action on Target

- [ ] `compile_header.cpp` (pch)
- [ ] `compile_source.cpp` (object)
- [ ] `link_target.cpp` (executable, library, C++20 module)
- [x] `build.cpp`
  - Setup tasks by using the above files
  - [ ] PrecompileHeader (pch) files
  - [x] Object
  - [x] Target
- [x] `tasks.cpp`
  - Runs the tasks using Taskflow
  - Contains mock equivalent for CppUTest
