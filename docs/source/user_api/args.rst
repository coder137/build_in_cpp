Args
=====

args.h
-------

.. doxygenclass:: buildcc::Args    

.. doxygenstruct:: buildcc::ArgToolchainState

.. doxygenstruct:: buildcc::ArgToolchain

.. doxygenstruct:: buildcc::ArgTarget

Example
---------

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
        Args args;
        ArgToolchain arg_gcc_toolchain;
        args.AddToolchain("gcc", "Generic GCC toolchain", arg_gcc_toolchain);

        // TODO, Add ArgTarget example (Currently incomplete)
        args.Parse(argc, argv);

        // Root
        args.GetProjectRootDir(); // Contains ``root_dir`` value
        args.GetProjectBuildDir(); // Contains ``build_dir`` value
        args.GetLogLevel(); // Contains ``loglevel`` enum
        args.Clean(); // Contains ``clean`` value

        // Toolchain
        arg_gcc_toolchain.state; // .build, .test
        BaseToolchain gcc_toolchain = arg_gcc_toolchain.ConstructToolchain(); // .id, .name, .asm_compiler, .c_compiler, .cpp_compiler, .archiver, .linker

        // Underlying CLI11 library
        auto & app = args.Ref();
        const auto & app = args.ConstRef();

        return 0;
    }

.. code-block:: toml
    :linenos:

    # Root
    root_dir = ""
    build_dir = "_build"
    loglevel = "trace"
    clean = true

    # Toolchain
    [toolchain.gcc]
    build = true
    test = true

    id = "gcc"
    name = "x86_64-linux-gnu"
    asm_compiler = "as"
    c_compiler = "gcc"
    cpp_compiler = "g++"
    archiver = "ar"
    linker = "ld"

