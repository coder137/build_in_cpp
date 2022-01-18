Toolchain
=========

toolchain.h
------------

.. doxygenclass:: buildcc::Toolchain

.. doxygentypedef:: ToolchainId

.. doxygentypedef:: BaseToolchain

toolchain_verify.h
------------------

.. doxygenclass:: buildcc::ToolchainVerify

.. doxygenstruct:: buildcc::VerifyToolchainConfig

.. doxygenstruct:: buildcc::VerifiedToolchain

Example
--------

.. code-block:: cpp
    
    BaseToolchain custom_toolchain(ToolchainId::Custom, "custom_new_toolchain", "asm_compiler", "c_compiler", "cpp_compiler", "archiver", "linker");

    std::vector<VerifiedToolchain> verified_toolchains = custom_toolchain.Verify();
    env::assert_fatal(!verified_toolchains.empty(), "Toolchain not found");

Specialized Toolchain
=====================

toolchain_gcc.h
----------------

toolchain_msvc.h
-----------------
