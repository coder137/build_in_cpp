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

build.foo
^^^^^^^^^^

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


build.main
^^^^^^^^^^^

.. code-block:: cpp
    :linenos:

    int main(int argc, char ** argv) {
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
        fooTarget(target, "");
        target.AddSource("main.cpp");
        target.Build();
    }

External Lib
-------------

For end users consuming third party libraries 

Custom Target
----------------

For super customized targets and toolchains 

Generic
--------

Select library type and target-toolchain type at runtime

PrecompileHeader
----------------

Precompile header usage with GCC and MSVC compilers 

Dependency Chaining
---------------------

Chain `Generators` and `Targets` using the `Register` module 

Target Info
-------------

* Target Info usage to store Target specific information
* Example usage for Header Only targets, however it can store information for all Target inputs
* Common information used between multiple targets can be stored into a `TargetInfo` instance
