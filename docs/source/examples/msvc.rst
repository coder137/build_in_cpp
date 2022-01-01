MSVC
=====

Lowlevel MSVC Tests

Executable
-----------

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 2,5

    // MSVC specialized Toolchain
    Toolchain_msvc msvc;

    // MSVC specialized targets
    ExecutableTarget_msvc target("Simple", msvc, "");

    // Common BaseTarget API
    target.GlobSources("src");
    target.AddIncludeDir("include", true);
    target.Build();

StaticLib
----------

DynamicLib
-----------
