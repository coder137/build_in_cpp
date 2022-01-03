Hybrid
=======

Real world tests combining multiple compilers

.. note:: In the Hybrid examples we use the **Args** and **Register** module to abstract away ``Taskflow`` and ``CLI11`` APIs. To see the lowlevel usage please see the lowlevel **[compiler]** tests.

Single Boilerplate
-------------------

We are only using a single Toolchain - Target pair

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
        // Args module to get data from the command line or .toml file passed in through --config
        Args args;

        // Register your [toolchain.{name}]
        // In this case it will be [toolchain.gcc]
        ArgToolchain arg_gcc;
        args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);

        // Parse the arguments from the command line
        args.Parse(argc, argv);

        // Register module 
        Register reg(args);
        reg.Clean(clean_cb);

        // TODO, Write your target builds here
        // See examples below
    }

    static void clean_cb() {
        env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
        fs::remove_all(env::get_project_build_dir());
    }


.. code-block:: toml
    :linenos:

    # Required parameters
    root_dir = "" # build executable meant to be invoked from the current directory
    build_dir = "_build" # Creates this directory relative to where you invoke your build executable

    # Optional parameters
    loglevel = "trace" # trace, debug, info, warning, critical
    clean = true # calls clean_cb if true, user specifies how their project must be cleaned

    # Toolchain
    # Valid configurations are
    # build = false, test = false
    # build = true, test = false
    # build = true, test = true
    [toolchain.gcc]
    build = true
    test = true


Single
-------

Compile a single source with a single GCC compiler.

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
        // See Single Boilerplate

        Toolchain_gcc gcc;
        ExecutableTarget_gcc hello_world("hello_world", gcc, "");

        // Select your builds and tests using the .toml files
        reg.Build(arg_gcc.state, hello_world_build_cb, hello_world);
        reg.Test(arg_gcc.state, "{executable}", hello_world);

        // Build Target
        reg.RunBuild();

        // Test Target
        reg.RunTest();
    }

    static void hello_world_build_cb(BaseTarget &target) {
        target.AddSource("main.cpp", "src");
        target.Build();
    }


Multiple Boilerplate
-----------------------

We are using multiple Toolchain - Target pairs

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
        // Args module to get data from the command line or .toml file passed in through --config
        Args args;

        // Register your [toolchain.{name}]
        // In this case it will be [toolchain.gcc] and [toolchain.msvc]
        ArgToolchain arg_gcc;
        ArgToolchain arg_msvc;
        args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
        args.AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);
        // NOTE, You can add more toolchains here as per your project requirement

        // Parse the arguments from the command line
        args.Parse(argc, argv);

        // Register module 
        Register reg(args);
        reg.Clean(clean_cb);

        // TODO, Write your target builds here
        // See examples below
    }

    static void clean_cb() {
        env::log_info(EXE, fmt::format("Cleaning {}", env::get_project_build_dir()));
        fs::remove_all(env::get_project_build_dir());
    }


.. code-block:: toml
    :linenos:

    # Required parameters
    root_dir = "" # build executable meant to be invoked from the current directory
    build_dir = "_build" # Creates this directory relative to where you invoke your build executable

    # Optional parameters
    loglevel = "trace" # trace, debug, info, warning, critical
    clean = true # calls clean_cb if true, user specifies how their project must be cleaned

    # Toolchain
    # Valid configurations are
    # build = false, test = false
    # build = true, test = false
    # build = true, test = true
    [toolchain.gcc]
    build = true
    test = true

    # If we are building on Windows make these true
    [toolchain.msvc]
    build = false
    test = false

.. note:: On Windows, make sure you install the Build Tools properly and invoke ``vcvarsall.bat amd64`` or equivalent from the command line to activate your toolchain.

Simple 
-------

Similar to lowlevel GCC Flags example for both the GCC and MSVC compiler

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
        // See Multiple Boilerplate

        Toolchain_gcc gcc;
        Toolchain_msvc msvc;

        ExecutableTarget_gcc g_cppflags("cppflags", gcc, "files");
        ExecutableTarget_msvc m_cppflags("cppflags", msvc, "files");
        ExecutableTarget_gcc g_cflags("cflags", gcc, "files");
        ExecutableTarget_msvc m_cflags("cflags", msvc, "files");

        // Select your builds and tests using the .toml files
        reg.Build(arg_gcc.state, cppflags_build_cb, g_cppflags);
        reg.Build(arg_msvc.state, cppflags_build_cb, m_cppflags);
        reg.Build(arg_gcc.state, cflags_build_cb, g_cflags);
        reg.Build(arg_msvc.state, cflags_build_cb, m_cflags);

        // Test steps
        reg.Test(arg_gcc.state, "{executable}", g_cppflags);
        reg.Test(arg_msvc.state, "{executable}", m_cppflags);
        reg.Test(arg_gcc.state, "{executable}", g_cflags);
        reg.Test(arg_msvc.state, "{executable}", m_cflags);

        // Build Target
        reg.RunBuild();

        // Test Target
        reg.RunTest();
    }

    static void cppflags_build_cb(BaseTarget &cppflags) {
        cppflags.AddSource("main.cpp", "src");
        cppflags.AddSource("src/random.cpp");
        cppflags.AddIncludeDir("include", true);

        // Toolchain specific code goes here
        switch (cppflags.GetToolchain().GetId()) {
        case ToolchainId::Gcc: {
            cppflags.AddPreprocessorFlag("-DRANDOM=1");
            cppflags.AddCppCompileFlag("-Wall");
            cppflags.AddCppCompileFlag("-Werror");
            cppflags.AddLinkFlag("-lm");
            break;
        }
        case ToolchainId::Msvc: {
            cppflags.AddPreprocessorFlag("/DRANDOM=1");
            cppflags.AddCppCompileFlag("/W4");
            break;
        }
        default:
            break;
        }

        cppflags.Build();
    }

    static void cflags_build_cb(BaseTarget &cflags) {
        cflags.AddSource("main.c", "src");

        // Toolchain specific code goes here
        switch (cflags.GetToolchain().GetId()) {
        case ToolchainId::Gcc: {
            cflags.AddPreprocessorFlag("-DRANDOM=1");
            cflags.AddCCompileFlag("-Wall");
            cflags.AddCCompileFlag("-Werror");
            cflags.AddLinkFlag("-lm");
            break;
        }
        case ToolchainId::Msvc: {
            cflags.AddPreprocessorFlag("/DRANDOM=1");
            cflags.AddCCompileFlag("/W4");
            break;
        }
        default:
            break;
        }

        cflags.Build();
    }

Foolib
-------

For library developers 

.. admonition:: Scenario

    Say suppose you are a library developer who has created an amazing ``Foo`` library. How would you easily specifiy your project build to be used by yourself and end users?

    **Solution**: Create Header / Source segregations. For example. ``build.foo.h`` and ``build.foo.cpp``
    End users can now create their own ``build.[project].cpp`` file and compile ``build.foo.cpp`` along with their source and use appropriate APIs are provided by your files.

    Depending on the complexity of your project the library developer can provide multiple APIs with different options that need to be selected at run time / compile time.

**Header**

.. code-block:: cpp
    :linenos:

    #pragma once

    #include "buildcc.h"

    void fooTarget(buildcc::BaseTarget &target, const fs::path &relative_path);

**Source**

.. code-block:: cpp
    :linenos:

    #include "build.foo.h"

    void fooTarget(buildcc::BaseTarget &target, const fs::path &relative_path) {
        target.AddSource(relative_path / "src/foo.cpp");
        target.AddIncludeDir(relative_path / "src", true);
    }


External Lib
-------------

For end users consuming third party libraries 

.. admonition:: Scenario

    User would like to use the third party library ``Foo`` in their codebase. The ``Foo`` library resides in a different directory as visualized below.

.. uml::

    @startmindmap
    * [folder]
    ** external_lib 
    *** [project_root]
    ** foolib
    @endmindmap

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 18

    #include "build.foo.h"

    int main(int argc, char ** argv) {
        // See Multiple Boilerplate

        // Build steps
        Toolchain_gcc gcc;
        Toolchain_msvc msvc;

        ExecutableTarget_gcc g_foolib("foolib", gcc, "");
        ExecutableTarget_msvc m_foolib("foolib", msvc, "");

        reg.Build(arg_gcc.state, foolib_build_cb, g_foolib);
        reg.Build(arg_msvc.state, foolib_build_cb, m_foolib);

        reg.RunBuild();
    }

    static void foolib_build_cb(BaseTarget &target) {
        fooTarget(target, "../foolib");
        target.AddSource("main.cpp");
        target.Build();
    }

Custom Target
----------------

For super customized targets and toolchains 

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 12,13,30,34,35,36

    int main(int argc, char ** argv) {

        Args args;
        ArgToolchain arg_gcc;
        ArgToolchain arg_msvc;
        ArgToolchain toolchain_clang_gnu;
        ArgTarget target_clang_gnu;
        args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
        args.AddToolchain("msvc", "Generic msvc toolchain", arg_msvc);

        // Add your custom toolchain - target to the command line
        args.AddToolchain("clang_gnu", "Clang GNU toolchain", toolchain_clang_gnu);
        args.AddTarget("clang_gnu", "Clang GNU target", target_clang_gnu);

        args.Parse(argc, argv);

        // Additional boilerplate

        // Supplied at compile time
        Toolchain_gcc gcc;
        Toolchain_msvc msvc;

        ExecutableTarget_gcc g_foolib("foolib", gcc, "");
        ExecutableTarget_msvc m_foolib("foolib", msvc, "");

        reg.Build(arg_gcc.state, foolib_build_cb, g_foolib);
        reg.Build(arg_msvc.state, foolib_build_cb, m_foolib);

        // Get custom toolchain from the command line, supplied at run time
        BaseToolchain clang = toolchain_clang_gnu.ConstructToolchain();

        // Get compile_command and link_command from the command line
        TargetConfig config;
        config.compile_command = target_clang_gnu.compile_command;
        config.link_command = target_clang_gnu.link_command;
        Target_custom c_foolib("CFoolib.exe", TargetType::Executable, clang, "", config);
        reg.Build(toolchain_clang_gnu.state, foolib_build_cb, c_foolib);
        
        // Build targets
        reg.RunBuild();
    }

    static void foolib_build_cb(BaseTarget &target) {
        target.AddSource("src/foo.cpp");
        target.AddIncludeDir("src", true);
        target.AddSource("main.cpp");
        target.Build();
    }

.. code-block:: toml
    :linenos:
    :emphasize-lines: 4,18

    # See Multiple boilerplate .toml file

    # Custom toolchain added here
    [toolchain.clang_gnu]
    build = true
    test = true

    # Custom toolchain added here, supplied during runtime
    id = "Clang"
    name = "clang_gnu"
    asm_compiler = "llvm-as"
    c_compiler = "clang"
    cpp_compiler = "clang++"
    archiver = "llvm-ar"
    linker = "ld"

    # Custom target added here
    [target.clang_gnu]
    compile_command = "{compiler} {preprocessor_flags} {include_dirs} {compile_flags} -o {output} -c {input}"
    link_command = "{cpp_compiler} {link_flags} {compiled_sources} -o {output} {lib_dirs} {lib_deps}"


PrecompileHeader
----------------

Precompile header usage with GCC and MSVC compilers 

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 8,9,10,36,37,38

    // Modified Lowlevel GCC Flags example for PCH
    
    static void cppflags_build_cb(BaseTarget &cppflags) {
    cppflags.AddSource("main.cpp", "src");
    cppflags.AddSource("random.cpp", "src");
    cppflags.AddIncludeDir("include", true);

    cppflags.AddPch("pch/pch_cpp.h");
    cppflags.AddPch("pch/pch_c.h");
    cppflags.AddIncludeDir("pch", true);

    // Toolchain specific code goes here
    switch (cppflags.GetToolchain().GetId()) {
    case ToolchainId::Gcc: {
        cppflags.AddPreprocessorFlag("-DRANDOM=1");
        cppflags.AddCppCompileFlag("-Wall");
        cppflags.AddCppCompileFlag("-Werror");
        cppflags.AddLinkFlag("-lm");
        break;
    }
    case ToolchainId::Msvc: {
        cppflags.AddPreprocessorFlag("/DRANDOM=1");
        cppflags.AddCppCompileFlag("/W4");
        break;
    }
    default:
        break;
    }

    cppflags.Build();
    }

    static void cflags_build_cb(BaseTarget &cflags) {
    cflags.AddSource("main.c", "src");

    cflags.AddPch("pch/pch_c.h");
    cflags.AddIncludeDir("pch", false);
    cflags.AddHeader("pch/pch_c.h");

    // Toolchain specific code goes here
    switch (cflags.GetToolchain().GetId()) {
    case ToolchainId::Gcc: {
        cflags.AddPreprocessorFlag("-DRANDOM=1");
        cflags.AddCCompileFlag("-Wall");
        cflags.AddCCompileFlag("-Werror");
        cflags.AddLinkFlag("-lm");
        break;
    }
    case ToolchainId::Msvc: {
        cflags.AddPreprocessorFlag("/DRANDOM=1");
        cflags.AddCCompileFlag("/W4");
        break;
    }
    default:
        break;
    }

    cflags.Build();
    }

Dependency Chaining
---------------------

Chain **Generators** and **Targets** using the ``Register`` module 

.. code-block:: cpp

    int main(int argc, char ** argv) {
        // See Multiple Boilerplate

        Toolchain_gcc gcc;
        Toolchain_msvc msvc;

        BaseGenerator cpp_generator("cpp_generator", "");
        reg.Build(cpp_generator_cb, cpp_generator);

        ExecutableTarget_gcc g_cpptarget("cpptarget", gcc, "");
        reg.Build(arg_gcc.state, cpp_target_cb, g_cpptarget, cpp_generator);

        ExecutableTarget_msvc m_cpptarget("cpptarget", msvc, "");
        reg.Build(arg_msvc.state, cpp_target_cb, m_cpptarget, cpp_generator);
        
        // cpp_generator runs before g_cpptarget and m_cpptarget
        reg.Dep(g_cpptarget, cpp_generator);
        reg.Dep(m_cpptarget, cpp_generator);
    }

    // Use a python generator to create main.cpp
    static void cpp_generator_cb(BaseGenerator &generator) {
        generator.AddOutput("{gen_build_dir}/main.cpp", "main_cpp");
        generator.AddCommand("python3 {gen_root_dir}/python/gen.py --source_type cpp "
                            "--destination {main_cpp}");
        generator.Build();
    }

    // Use main.cpp generated by the python script to compile your target
    static void cpp_target_cb(BaseTarget &cpptarget,
                          const BaseGenerator &cpp_generator) {
        const fs::path main_cpp =
            fs::path(cpp_generator.GetValueByIdentifier("main_cpp"))
                .lexically_relative(env::get_project_root_dir());
        cpptarget.AddSource(main_cpp);
        cpptarget.Build();
    }


Target Info
-------------

* Target Info usage to store Target specific information
* Example usage for Header Only targets, however it can store information for all Target inputs
* Common information used between multiple targets can be stored into a `TargetInfo` instance
