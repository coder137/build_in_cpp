Toolchain
=========

toolchain.h
------------

.. doxygenclass:: buildcc::Toolchain

.. doxygentypedef:: BaseToolchain

toolchain_find.h
-----------------

.. doxygenclass:: buildcc::ToolchainFind

.. doxygenstruct:: buildcc::ToolchainFindConfig

toolchain_verify.h
------------------

.. doxygenclass:: buildcc::ToolchainVerify

.. doxygenstruct:: buildcc::ToolchainVerifyConfig

.. doxygenstruct:: buildcc::ToolchainCompilerInfo

Example
--------

.. code-block:: cpp
    :linenos:

    BaseToolchain custom_toolchain(ToolchainId::Custom, "custom_new_toolchain", "asm_compiler", "c_compiler", "cpp_compiler", "archiver", "linker");

    std::vector<VerifiedToolchain> verified_toolchains = custom_toolchain.Verify();
    env::assert_fatal(!verified_toolchains.empty(), "Toolchain not found");

Specialized Toolchain
=====================

toolchain_gcc.h
----------------

.. doxygenclass:: buildcc::Toolchain_gcc

toolchain_mingw.h
-----------------

.. doxygenclass:: buildcc::Toolchain_mingw

toolchain_msvc.h
-----------------

.. doxygenclass:: buildcc::Toolchain_msvc

Example
--------

.. code-block:: cpp
    :linenos:

    // Default GCC toolchain
    Toolchain_gcc gcc;

    // Default MinGW toolchain
    Toolchain_mingw mingw;

    // Default MSVC toolchain
    Toolchain_msvc msvc;
