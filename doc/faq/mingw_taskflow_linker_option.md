# MinGW Duplication

`inline static` variable causes multiple definition on the **10.2.0 MINGW/MSYS2 GCC Compiler (Windows)**

## Complete Error

- `taskflow/taskflow/core/executor.hpp`
```cpp
private:

inline static thread_local PerThread _per_thread;
```

- Error message
```
[build] FAILED: example/hybrid/hybrid_simple_example.exe 
[build] cmd.exe /C "cd . && D:\Software\msys64\mingw64\bin\g++.exe -g  example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj -o example\hybrid\hybrid_simple_example.exe -Wl,--out-implib,example\hybrid\libhybrid_simple_example.dll.a -Wl,--major-image-version,0,--minor-image-version,0  buildcc/libbuildcc.a  buildcc/targets/gcc/libtarget_gcc.a  buildcc/targets/msvc/libtarget_msvc.a  buildcc/plugins/libplugins.a  buildcc/lib/target/libtarget.a  buildcc/lib/env/libenv.a  spdlog/libspdlogd.a  fmt/libfmtd.a  buildcc/lib/toolchain/libtoolchain.a  flatbuffers/libflatbuffers.a  -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 && cd ."
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset af4f3
[build] buildcc/targets/msvc/libtarget_msvc.a(executable_target_msvc.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 3c547
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 19eb0a
[build] example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset dfd2a
[build] buildcc/plugins/libplugins.a(clang_compile_commands.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 3b478
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset e91ab
[build] buildcc/lib/target/libtarget.a(target.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 3c325
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset f5c3c
[build] buildcc/lib/target/libtarget.a(source.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 31775
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset c801f
[build] buildcc/lib/target/libtarget.a(include_dir.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 335e8
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset d18ce
[build] buildcc/lib/target/libtarget.a(build.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 94bb4
[build] buildcc/lib/target/libtarget.a(flags.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset 7d277
[build] buildcc/lib/target/libtarget.a(recheck_states.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: D:/Software/msys64/mingw64/bin/../lib/gcc/x86_64-w64-mingw32/10.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: DWARF error: could not find variable specification at offset ed699
[build] buildcc/lib/target/libtarget.a(fbs_storer.cpp.obj):D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: multiple definition of `TLS init function for tf::Executor::_per_thread'; example/hybrid/CMakeFiles/hybrid_simple_example.dir/build.cpp.obj:D:\Repositories\build_in_cpp\build/../taskflow/taskflow/core/executor.hpp:193: first defined here
[build] collect2.exe: error: ld returned 1 exit status
```

## Solution

- https://github.com/msys2/MINGW-packages/issues/2303
- Similar issue when adding the Taskflow library
- Error message added above

```cmake
# Solution
if (${MINGW})
    message(WARNING "-Wl,--allow-multiple-definition for MINGW")
    target_link_options(hybrid_simple_example PRIVATE -Wl,--allow-multiple-definition)
endif()
```
