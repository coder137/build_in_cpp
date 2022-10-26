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

.. doxygenstruct:: buildcc::ToolchainCompilerInfo

Example for Default Toolchain
------------------------------

.. code-block:: cpp
    :linenos:

    BaseToolchain arm_gcc(ToolchainId::Gcc, "arm-none-eabi-gcc", "arm-none-eabi-as", "arm-none-eabi-gcc", "arm-none-eabi-g++", "arm-none-eabi-ar", "arm-none-eabi-ld");

    // Toolchain::Find is only used to return a list of paths where the ToolchainExecutables are found
    // NOTE: All ToolchainExecutables must be found in a single directory for it to be present in the list
    {
        ToolchainFindConfig find_config;
        // Modify it here if needed
        auto found_toolchains = arm_gcc.Find(find_config);
    }

    // Runs Toolchain::Find
    // Selects first found toolchain (update ToolchainVerifyConfig if you want to select a different toolchain for verification)
    // Runs a pre-added ToolchainId::GCC verification function
    // If Verification Fails: Terminates the program
    // Else: Updates the arm_gcc ToolchainExecutables to the full path
    // i.e `arm-none-eabi-gcc` becomes `{host_absolute_path}/arm-none-eabi-gcc{host_executable_extension}`
    {
        ToolchainVerifyConfig verify_config;
        // Modify it here if needed
        arm_gcc.Verify(verify_config);
    }

Example for Custom Toolchain
----------------------------

.. code-block:: cpp
    :linenos:

    BaseToolchain custom_toolchain(ToolchainId::Custom, "custom_new_toolchain", "assembler", "c_compiler", "cpp_compiler", "archiver", "linker");

    // Toolchain::Find similar to previous example

    // Find all the relevant toolchains on your host system
    // Selects the first found toolchain
    // Runs a verification function on the selected toolchain depending on the `ToolchainId`
    Toolchain::AddVerificationFunc(ToolchainId::Custom,
    [](const ToolchainExecutables & executables) -> buildcc::env::optional<ToolchainCompilerInfo> {
        // Use executables to get compiler_version and target_arch
        if (success) {
            ToolchainCompilerInfo info;
            info.compiler_version = "compiler_version";
            info.target_arch = "target_arch";
            return info;
        } else {
            return {};
        }
    }, "custom_verification_func")

    ToolchainVerifyConfig verify_config;
    verify_config.verification_identifier = "custom_verification_func";
    custom_toolchain.Verify(verify_config);

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
