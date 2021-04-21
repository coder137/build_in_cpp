# After Install

- Create installation packages via CMake install / cpack
- Install and add bin to system path
- Use installed libraries and packages via `find_package`
- Create your `./build` executable

> NOTE, Make sure the compiler used to install / create cpack package is the same when creating `./build` executable

# Easy install Method

```bash
git clone <this repo>

# Or use your generator
cmake -B build -G Ninja

cd build
cpack -G ZIP
```

- Extract generated ZIP file to a location of your choice
- Add `<location>/bin` to system PATH


# Create your `build`script

- See `AfterInstall` CMakeLists.txt to `find_package` for library dependencies
- See `build.cpp` for complete example
