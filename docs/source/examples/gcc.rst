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
    // Create "Simple" target (meant to use the GCC compiler)
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

DynamicLib
-----------

Flags
------

AfterInstall
-------------

Use BuildCC with CMake

Plugins
--------
