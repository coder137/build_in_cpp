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
        // See Boilerplate

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

Similar to Flags example for both the GCC and MSVC compiler

Foolib
-------

For library developers 

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
