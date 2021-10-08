# Target Segregation

- `target.cpp`
  - Common target specific utility functions
  - Assertion functions
  - Validity checks
- `target_loader.cpp`
  - Load from flatbuffer schema file
- `target_storer.cpp`
  - Store to flatbuffer schema file

## Inputs to Target

- `source.cpp`
  - Source File
- `include_dir.cpp`
  - Include Dir
  - Header File
- `lib.cpp`
  - Lib Dir
  - Lib File (full path Target supplied)
  - External Lib File (relative link using -l)
- `flags.cpp`
  - PreprocessorFlags
  -  CommonCompileFlags
  -   AsmCompileFlags
  -   CCompileFlags
  -   CppCompileFlag
  -   LinkFlags


## Target states

- `recheck_states.cpp`
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
- `build.cpp`
  - Setup tasks by using the above files
  - [ ] PrecompileHeader (pch) files
  - [x] Object
  - [x] Target
- `tasks.cpp`
  - Runs the tasks using Taskflow
  - Contains mock equivalent for CppUTest
