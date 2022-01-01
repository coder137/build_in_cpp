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
    ExecutableTarget_msvc target("Simple", msvc, "");

    // Common BaseTarget API
    target.GlobSources("src");
    target.AddIncludeDir("include", true);
    target.Build();

StaticLib
----------

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

    // Common BaseTarget API
    dynamictarget.AddSource("src/random.cpp");
    dynamictarget.AddIncludeDir("include", true);
    dynamictarget.Build();

    // MSVC specialized target
    // Creates `Simple.exe` which uses the above dynamic library
    ExecutableTarget_msvc target_msvc("Simple", msvc, "");

    // Common BaseTarget API
    target_msvc.AddSource("src/main.cpp");
    target_msvc.AddIncludeDir("include", true);
    target_msvc.AddLibDep(dynamictarget);
    target_msvc.Build();

.. note:: To use the ``DynamicTarget_msvc`` generated library with ``ExecutableTarget_msvc`` we also need to copy the generated ``librandom.lib.dll`` library to the executable directory.
