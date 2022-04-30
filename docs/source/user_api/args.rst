Args
=====

args.h
-------

.. doxygenclass:: buildcc::Args

.. doxygenstruct:: buildcc::ArgCustom

.. doxygenstruct:: buildcc::ArgToolchainState

.. doxygenclass:: buildcc::ArgToolchain

.. doxygenstruct:: buildcc::ArgTarget

Example
---------

.. code-block:: cpp
    :linenos:

    using namespace buildcc;

    struct CustomData : ArgCustom {
        void Add(CLI::App & app) override {
            // setup your app from data1, data2, data3, data...
            // NOTE: The Add method should not be invoked by the user
            // NOTE: The Add method is only expected to be invoked once, not multiple times.
        }

        std::string data1;
        int data2;
        float data3;
        // etc
    };

    int main(int argc, char ** argv) {
        ArgToolchain arg_gcc_toolchain;
        ArgCustomData custom_data;
        Args::Init()
            .AddToolchain("gcc", "Generic GCC toolchain", arg_gcc_toolchain)
            .AddCustomCallback([](CLI::App &app) {});
            .AddCustomData(custom_data);
            .Parse(argc, argv);

        // Root
        Args::GetProjectRootDir(); // Contains ``root_dir`` value
        Args::GetProjectBuildDir(); // Contains ``build_dir`` value
        Args::GetLogLevel(); // Contains ``loglevel`` enum
        Args::Clean(); // Contains ``clean`` value

        // Toolchain
        // .build, .test
        arg_gcc_toolchain.state;
        // .id, .name, .asm_compiler, .c_compiler, .cpp_compiler, .archiver, .linker -> BaseToolchain
        auto &gcc_toolchain = arg_gcc_toolchain;
        gcc_toolchain.SetToolchainInfoFunc(GlobalToolchainInfo::Get(gcc_toolchain.id));
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

