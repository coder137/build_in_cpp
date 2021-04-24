# Simple example

Builds an executable which only contains one `main.cpp` source file

**Features currently present**

- Compile and store serialized `.bin` file
- Recompile when timestamp for `main.cpp` is newer (recently edited)

**Errors thrown**

- Checks for duplicate sources
- Checks for incorrect sources

# Running the example

- Build using cmake

```
cmake -B build -G Ninja
cmake --build build
```

- Run the build file
```
cd build
.\build.exe
```

- Convert the serialized `.bin` file to `.json` to read output
```
cmake --build build --target convert_bin_to_json
```
