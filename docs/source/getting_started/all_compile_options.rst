Compile Options for BuildExe
============================

We can pass in configuration parameters through the Command Line but writing them inside a ``.toml`` file and passing it through the ``--config`` flag is much easiler.

.. code-block:: shell

    BuildCC buildsystem
    Usage: buildexe [OPTIONS] [SUBCOMMAND]

    Options:
    -h,--help                   Print this help message and exit
    --help-all                  Expand individual options.
    --config                    Read <config>.toml files.
    --mode ENUM:value in {script->1,immediate->0} OR {1,0} REQUIRED
                                Provide BuildExe run mode
    --name TEXT REQUIRED        Provide Target name
    --type ENUM:value in {dynamicLibrary->2,staticLibrary->1,executable->0} OR {2,1,0} REQUIRED
                                Provide Target Type
    --relative_to_root TEXT REQUIRED
                                Provide Target relative to root
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


    Root:
    --clean                     Clean artifacts
    --loglevel ENUM:value in {warning->3,info->2,debug->1,critical->5,trace->0} OR {3,2,1,5,0}
                                LogLevel settings
    --root_dir TEXT REQUIRED    Project root directory (relative to current directory)
    --build_dir TEXT REQUIRED   Project build dir (relative to current directory)

    Subcommands:
    toolchain                   Select Toolchain
    target                      Select Target
