Supported Plugins
=================

clang_compile_commands.h
------------------------

.. doxygenclass:: buildcc::plugin::ClangCompileCommands

Example
--------

.. code-block:: cpp
    :linenos:

    using namespace buildcc;

    Target foolib;
    Target hello_world;

    // Foolib and Hello world targets are both added to a single "compile_commands.json" file
    plugin::ClangCompileCommands({&foolib, &hello_world}).Generate();
