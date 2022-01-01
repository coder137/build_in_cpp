GCC
====

Lowlevel GCC Tests

Simple
--------

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

    // Run
    tf::Executor executor;
    executor.run(target.GetTaskflow());
    executor.wait_for_all();


IncludeDir
----------

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
