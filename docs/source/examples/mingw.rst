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


.. note:: Our ``ExecutableTarget_mingw`` depends on ``DynamicTarget_mingw`` and requires the ``librandom.lib.dll`` file to be present in the same folder location as the executable when running.

PrecompileHeader
-------------------

TODO
