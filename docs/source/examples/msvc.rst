MSVC
=====

Lowlevel MSVC Tests

Executable
-----------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,6

    // MSVC specialized Toolchain
    Toolchain_msvc msvc;

    // MSVC specialized targets
    // Creates `Simple.exe`
    ExecutableTarget_msvc exetarget("Simple", msvc, "");
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
    :emphasize-lines: 2,6,15

    // MSVC specialized Toolchain
    Toolchain_msvc msvc;

    // MSVC specialized targets
    // Creates `librandom.lib`
    StaticTarget_msvc statictarget("librandom", msvc, "");
    statictarget.AddSource("src/random.cpp");
    statictarget.AddIncludeDir("include", true);
    statictarget.Build();

    // MSVC specialized targets
    // Creates `Simple.exe`
    ExecutableTarget_msvc exetarget("Simple", msvc, "");
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
    :emphasize-lines: 2,6,15

    // MSVC specialized Toolchain
    Toolchain_msvc msvc;

    // MSVC specialized targets
    // Creates `librandom.lib` and `librandom.lib.dll`
    DynamicTarget_msvc dynamictarget("librandom", msvc, "");
    dynamictarget.AddSource("src/random.cpp");
    dynamictarget.AddIncludeDir("include", true);
    dynamictarget.Build();

    // MSVC specialized target
    // Creates `Simple.exe` which uses the above dynamic library
    ExecutableTarget_msvc exetarget("Simple", msvc, "");
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
        fs::copy(dynamictarget.GetDllPath(),
                exetarget.GetTargetPath().parent_path() /
                dynamictarget.GetDllPath().filename());
    }


.. note:: To use the ``DynamicTarget_msvc`` generated library with ``ExecutableTarget_msvc`` we also need to copy the generated ``librandom.lib.dll`` library to the executable directory.
