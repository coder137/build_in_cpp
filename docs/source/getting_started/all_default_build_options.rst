Build Options for "scripts"
============================

We can pass in configuration parameters through the Command Line but writing them inside a ``.toml`` file and passing it through the ``--config`` flag is much easier.

Command Line options
---------------------

.. code-block:: shell

    Options:
    -h,--help                   Print this help message and exit
    --help-all                  Expand individual options.
    --config                    Read <config>.toml files.
    [Option Group: Root]
    Options:
        --clean                     Clean artifacts
        --loglevel ENUM:value in {warning->3,info->2,debug->1,critical->5,trace->0} OR {3,2,1,5,0}
                                    LogLevel settings
        --root_dir TEXT REQUIRED    Project root directory (relative to current directory)
        --build_dir TEXT REQUIRED   Project build dir (relative to current directory)

    Subcommands:
    toolchain
    Select Toolchain
    Supported Toolchains:
        gcc                         Generic gcc toolchain

    target
    Select Target

TOML file options
-------------------

Relate the options below with the **Command Line options** above.

You can also read the `CLI11 README <https://github.com/CLIUtils/CLI11>`_

.. code-block:: toml

    # Root Options
    clean = true # true, false
    loglevel = "trace" # "trace", "debug", "info", "warning", "critical"
    root_dir = "" # REQUIRED
    build_dir = "" # REQUIRED

    # Subcommand

    # Host Toolchain Options
    [toolchain.gcc] # DEPENDS on user

    # Run time way to select your build and test options during registration
    # Valid options
    # build = false, test = false, target not built or tested
    # build = true, test = false, target built but not tested
    # build = true, test = true, target built and tested (users responsiblity for a testable target)
    build = true # REQUIRED
    test = true # REQUIRED

    # Run time way to change the compiler on the fly
    # Not recommended
    # Prefer to use the specialized Toolchains during Compile time (See build.cpp in examples)
    # id = "gcc"
    # name = "x86_64-linux-gnu"
    # asm_compiler = "as"
    # c_compiler = "gcc"
    # cpp_compiler = "g++"
    # archiver = "ar"
    # linker = "ld"

    # TODO, Add more options to narrow down search when multiple toolchains are installed


.. note:: These are the default build options during "script" mode. 
    
    Users can also add custom arguments using CLI11 using the ``.Ref()`` API and using them in the ``.toml`` file.
    
    Please make sure to read the `CLI11 README <https://github.com/CLIUtils/CLI11>`_ for those APIs.
