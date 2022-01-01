GCC
====

Lowlevel GCC Tests

Simple
--------

Compile a single source

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,7

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "Simple" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc target("Simple", gcc, "");
    target.AddSource("main.cpp");
    target.Build();

    // Build
    tf::Executor executor;
    executor.run(target.GetTaskflow());
    executor.wait_for_all();


IncludeDir
----------

Compile multiple sources with header files

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,7

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "IncludeDir" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc target("IncludeDir", gcc, "files");
    target.AddSource("main.cpp", "src");
    target.AddSource("src/random.cpp");
    target.AddHeader("include/random.h");
    target.AddIncludeDir("include");
    target.Build();

    // Build
    tf::Executor executor;
    executor.run(target.GetTaskflow());
    executor.wait_for_all();

StaticLib
----------

Compile static library which is used by an executable

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,7,16

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "librandom.a" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    StaticTarget_gcc statictarget("librandom", gcc, "files");
    statictarget.AddSource("src/random.cpp");
    statictarget.AddHeader("include/random.h");
    statictarget.AddIncludeDir("include");
    statictarget.Build();

    // GCC specialized targets
    // Create "statictest" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc exetarget("statictest", gcc, "files");
    exetarget.AddSource("main.cpp", "src");
    exetarget.AddIncludeDir("include");
    exetarget.AddLibDep(statictarget);
    exetarget.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // Explicitly setup your dependencies
    tf::Task statictargetTask = taskflow.composed_of(statictarget.GetTaskflow());
    tf::Task exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
    exetargetTask.succeed(statictargetTask);

    // Run
    executor.run(taskflow);
    executor.wait_for_all();

DynamicLib
-----------

Flags
------

AfterInstall
-------------

Use BuildCC with CMake

Plugins
--------
