MinGW
=======

Lowlevel MinGW Tests


Executable
-----------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,6

    // MINGW specialized Toolchain
    Toolchain_mingw mingw;

    // MINGW specialized targets
    // Creates `Simple.exe`
    ExecutableTarget_mingw exetarget("Simple", mingw, "");
    exetarget.GlobSources("src");
    exetarget.AddIncludeDir("include", true);
    exetarget.Build();

    // Build
    tf::Executor executor;
    executor.run(exetarget.GetTaskflow());
    executor.wait_for_all();

StaticLib
----------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,6,13

    // MINGW specialized Toolchain
    Toolchain_mingw mingw;

    // MINGW specialized targets
    // Creates `librandom.lib`
    StaticTarget_mingw statictarget("librandom", mingw, "");
    statictarget.AddSource("src/random.cpp");
    statictarget.AddIncludeDir("include", true);
    statictarget.Build();

    // MINGW specialized targets
    // Creates `Simple.exe`
    ExecutableTarget_mingw exetarget("Simple", mingw, "");
    exetarget.AddSource("src/main.cpp");
    exetarget.AddIncludeDir("include", true);
    exetarget.AddLibDep(statictarget);
    exetarget.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // Setup your dependencies explicitly
    // Here statictarget needs to be built before exetarget
    auto statictargetTask = taskflow.composed_of(statictarget.GetTaskflow());
    auto exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
    exetargetTask.succeed(statictargetTask);

    executor.run(taskflow);
    executor.wait_for_all();

DynamicLib
-----------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,6,13

    // MINGW specialized Toolchain
    Toolchain_mingw mingw;

    // MINGW specialized targets
    // Creates `librandom.lib` and `librandom.lib.dll`
    DynamicTarget_mingw dynamictarget("librandom", mingw, "");
    dynamictarget.AddSource("src/random.cpp");
    dynamictarget.AddIncludeDir("include", true);
    dynamictarget.Build();

    // MINGW specialized target
    // Creates `Simple.exe` which uses the above dynamic library
    ExecutableTarget_mingw exetarget("Simple", mingw, "");
    exetarget.AddSource("src/main.cpp");
    exetarget.AddIncludeDir("include", true);
    exetarget.AddLibDep(dynamictarget);
    exetarget.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // Setup your dependencies explicitly
    // Here dynamictarget needs to be built before exetarget
    auto dynamictargetTask = taskflow.composed_of(dynamictarget.GetTaskflow());
    auto exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
    exetargetTask.succeed(dynamictargetTask);

    executor.run(taskflow);
    executor.wait_for_all();

    // Now that both your targets are built, copy the dynamictarget DLL to the exetarget location
    // This is required for your exetarget to run properly
    if (exetarget.IsBuilt()) {
        fs::path copy_to_path =
            exetarget.GetTargetBuildDir() / dynamictarget.GetTargetPath().filename();
        fs::remove_all(copy_to_path);
        fs::copy(dynamictarget.GetTargetPath(), copy_to_path);
    }


.. note:: Our ``ExecutableTarget_mingw`` depends on ``DynamicTarget_mingw`` and requires the ``librandom.dll`` file to be present in the same folder location as the executable when running.

PrecompileHeader
-------------------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,4,7,24,25,26,39,40,41

    int main() {
        Toolchain_mingw mingw;

        ExecutableTarget_mingw g_cppflags("cppflags", mingw, "files");
        cppflags_build_cb(g_cppflags);

        ExecutableTarget_mingw g_cflags("cflags", mingw, "files");
        cflags_build_cb(g_cflags);

        tf::Executor executor;
        tf::Taskflow taskflow;

        taskflow.composed_of(g_cppflags.GetTaskflow());
        taskflow.composed_of(g_cflags.GetTaskflow());
        executor.run(taskflow);
        executor.wait_for_all();
    }

    static void cppflags_build_cb(BaseTarget &cppflags) {
        cppflags.AddSource("main.cpp", "src");
        cppflags.AddSource("random.cpp", "src");
        cppflags.AddIncludeDir("include", true);

        cppflags.AddPch("pch/pch_cpp.h");
        cppflags.AddPch("pch/pch_c.h");
        cppflags.AddIncludeDir("pch", true);

        cppflags.AddPreprocessorFlag("-DRANDOM=1");
        cppflags.AddCppCompileFlag("-Wall");
        cppflags.AddCppCompileFlag("-Werror");
        cppflags.AddLinkFlag("-lm");

        cppflags.Build();
    }

    static void cflags_build_cb(BaseTarget &cflags) {
        cflags.AddSource("main.c", "src");

        cflags.AddPch("pch/pch_c.h");
        cflags.AddIncludeDir("pch", false);
        cflags.AddHeader("pch/pch_c.h");

        cflags.AddPreprocessorFlag("-DRANDOM=1");
        cflags.AddCCompileFlag("-Wall");
        cflags.AddCCompileFlag("-Werror");
        cflags.AddLinkFlag("-lm");

        cflags.Build();
    }

