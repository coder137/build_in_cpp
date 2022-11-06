# Dependencies

These third party libraries are added as submodules since they aren't meant to be modified by this project.

`git submodule add [git_url] third_party/[foldername]`

## Main

- fmt (Formatting)
- spdlog (Logging)
- json (Serialization)
- CLI11 (Argument Parsing)
- Taskflow (Parallel Programming)

## Unit Testing

- cpputest (Unit Testing / Mocking)

## Utility

- tl_optional (Better optional support)
  - Synced with branch origin/master (May 2, 2021)
  - Commit Id: c28fcf74d207fc667c4ed3dbae4c251ea551c8c1
  - Needed fix: #45

## Tools

- [x] clangformat (auto)
- [x] gcovr
- [x] lcovr
- [x] clang-tidy
- [x] cppcheck
- [x] graphviz
- [ ] valgrind
- [ ] gprof
- [ ] callgrind
- [ ] qcachegrind (windows)
  - https://waterprogramming.wordpress.com/2017/06/08/profiling-c-code-with-callgrind/
