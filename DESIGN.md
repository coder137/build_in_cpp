- [DESIGN](#design)
- [buildcc](#buildcc)
  - [Target](#target)
  - [Toolchain](#toolchain)
- [buildcc::internal](#buildccinternal)
  - [FbsLoader](#fbsloader)
  - [Path](#path)
  - [Functions](#functions)
- [buildcc::env](#buildccenv)
  - [Stores](#stores)
  - [Functions](#functions-1)

# DESIGN

# buildcc

## Target

- [x] `AddSource`
- [ ] `AppendSources`
- [ ] `AddIncludeDir`
- [ ] `AppendIncludeDirs`
- [ ] `AddLibDep`
- [ ] `AppendLibDeps`
- [ ] `AddLibDir`
- [ ] `AppendLibDirs`
- [x] `Build`

## Toolchain

NOTE: We can use multiple inheritance on this to create customized Toolchains

- [ ] `AddPreprocessorFlag`
- [ ] `AppendPreprocessorFlags`
- [ ] `AddCCompilerFlag`
- [ ] `AppendCCompilerFlags`
- [ ] `AddCppCompilerFlag`
- [ ] `AppendCppCompilerFlags`
- [ ] `AddLinkFlag`
- [ ] `AppendLinkFlags`
- [ ] `GetName`
- [ ] `GetCCompiler`
- [ ] `GetCppCompiler`
- [ ] `GetPreprocessorFlags`
- [ ] `GetCCompilerFlags`
- [ ] `GetCppCompilerFlags`
- [ ] `GetLinkFlags`

# buildcc::internal

## FbsLoader

- [x] `Load`

- [ ] `IsLoaded`
- [ ] `GetLoadedTargetType`
- [ ] `GetLoadedToolchain`
- [x] `GetLoadedSources`
- [x] `GetLoadedIncludeDirs`
- [ ] `GetLoadedLibDeps`
- [ ] `GetLoadedLibDirs`

## Path

- [x] `Path::CreateNewPath`
- [x] `Path::CreateExistingPath` 
- [x] `GetLastWriteTimestamp`
- [x] `GetPathname`

## Functions

- [x] `assert_fatal`
- [x] `assert_fatal_true`
- [x] `fbs_utils_store_target`

# buildcc::env

## Stores

- `BUILDCC_ROOT`
  - Root folder for sources
- `BUILDCC_EXE_DIR`
  - Folder for `build.exe`
- `BUILDCC_OUTPUT_DIR`
  - Stores intermediate files

## Functions

- [ ] `env::set_env`
- [ ] `env::set_log_level`
- [ ] `env::log_trace`
- [ ] `env::log_debug`
- [ ] `env::log_info`
- [ ] `env::log_warning`
- [ ] `env::log_fatal`
