Compile Options for BuildExe
============================

We can pass in configuration parameters through the Command Line but writing them inside a ``.toml`` file and passing it through the ``--config`` flag is much easier.

Command Line options
---------------------

.. code-block:: shell

    BuildCC buildsystem
    Usage: buildexe [OPTIONS] [SUBCOMMAND]

    Options:
    -h,--help                   Print this help message and exit
    --help-all                  Expand individual options.
    --config                    Read <config>.toml files.
    --mode ENUM:value in {script->1,immediate->0} OR {1,0} REQUIRED
                                Provide BuildExe run mode
    [Option Group: Root]
    Options:
        --clean                     Clean artifacts
        --loglevel ENUM:value in {warning->3,info->2,debug->1,critical->5,trace->0} OR {3,2,1,5,0} 
                                    LogLevel settings
        --root_dir TEXT REQUIRED    Project root directory (relative to current directory)
        --build_dir TEXT REQUIRED   Project build dir (relative to current directory)
    [Option Group: Project Info]
    Options:
        --name TEXT REQUIRED        Provide Target name
        --type ENUM:value in {dynamicLibrary->2,staticLibrary->1,executable->0} OR {2,1,0} REQUIRED
                                    Provide Target Type
        --relative_to_root TEXT REQUIRED
                                    Provide Target relative to root
    [Option Group: Target Inputs]
    Options:
        --srcs TEXT ...             Provide source files
        --includes TEXT ...         Provide include dirs
        --lib_dirs TEXT ...         Provide lib dirs
        --external_libs TEXT ...    Provide external libs
        --preprocessor_flags TEXT ...
                                    Provide Preprocessor flags
        --common_compile_flags TEXT ...
                                    Provide CommonCompile Flags
        --asm_compile_flags TEXT ...
                                    Provide AsmCompile Flags
        --c_compile_flags TEXT ...  Provide CCompile Flags
        --cpp_compile_flags TEXT ...
                                    Provide CppCompile Flags
        --link_flags TEXT ...       Provide Link Flags

    Subcommands:
    toolchain
    Select Toolchain
    Supported Toolchains:
        host                        Host Toolchain

    target
    Select Target

    script
    Options:
        --configs TEXT ...          Config files for script mode

TOML file options
-------------------

Relate the options below with the **Command Line options** above.

You can also read the `CLI11 README <https://github.com/CLIUtils/CLI11>`_

.. code-block:: toml

    # Default (ungrouped) Options
    mode = "script" # REQUIRED script, immediate

    # Root Options
    clean = true # true, false
    loglevel = "trace" # "trace", "debug", "info", "warning", "critical"
    root_dir = "" # REQUIRED
    build_dir = "" # REQUIRED

    # Target Info Options
    name = "" # REQUIRED
    type = "executable" # REQUIRED, executable, staticLibrary, dynamicLibrary
    relative_to_root = "" # REQUIRED

    # Target Inputs Options
    srcs = [""]
    includes = [""]
    lib_dirs = [""]
    external_libs = [""]
    preprocessor_flags = [""]
    common_compile_flags = [""]
    asm_compile_flags = [""]
    c_compile_flags = [""]
    cpp_compile_flags = [""]
    link_flags = [""]

    # Subcommand

    # Host Toolchain Options
    [toolchain.host] # ALWAYS
    build = true # ALWAYS
    test = false # ALWAYS

    id = "gcc"
    name = "x86_64-linux-gnu"
    asm_compiler = "as"
    c_compiler = "gcc"
    cpp_compiler = "g++"
    archiver = "ar"
    linker = "ld"

    # TODO, Add more options to narrow down search when multiple toolchains are installed

    # Script Options
    [script]
    configs = ["build.toml", "custom_toolchain.toml"] # Converted to --config build.toml --config custom_toolchain.toml
