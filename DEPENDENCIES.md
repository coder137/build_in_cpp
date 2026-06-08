# Dependencies

These third party libraries are added as submodules since they aren't meant to be modified by this project.

### Adding a submodule

`git submodule add [git_url] third_party/[foldername]`

### Removing a submodule

- `git rm --cached path_to_submodule` (no trailing slash)
- Delete relevant line from `.gitmodules` file
- Delete relevant section from `.git/config` file

## Main

- fmt (Formatting)
- spdlog (Logging)
- json (Serialization)
- CLI11 (Argument Parsing)
- Taskflow (Parallel Programming)

## Unit Testing

- cpputest (Unit Testing / Mocking)

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
