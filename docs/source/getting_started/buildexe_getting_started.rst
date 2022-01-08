Getting started with BuildExe
=============================

Basic Procedure
----------------

Since we are writing our scripts in C++ we first need to **compile** our "script" to an executable, and then **execute** it to build targets.

.. uml::

    usecase "build.helloworld.cpp" as build_cpp
    usecase "compile.toml" as compile_toml
    usecase "host_toolchain.toml" as host_toolchain_toml
    usecase "./build.helloworld" as build_project_exe
    usecase "build.toml" as build_toml

    rectangle "./buildexe" as buildexe_exe

    artifact "./hello_world" as hello_world_exe

    compile_toml -up-> build_cpp
    host_toolchain_toml -up-> build_cpp
    build_cpp -right-> buildexe_exe
    buildexe_exe -right-> build_project_exe
    build_toml -up-> build_project_exe
    build_project_exe -right-> hello_world_exe

Helloworld example
------------------

* Write your C++ "script"
* Write your ``compile.toml`` file
* Write your ``build.toml`` file
* Invoke ``buildexe`` from the command line from the **[workspace]** folder
   * Pay attention to the ``root_dir`` and ``build_dir`` parameters set in your ``compile.toml`` and ``build.toml`` file. 
   * These directories are relative to the directory from which you **invoke** buildexe

.. code-block:: bash

    ./buildexe --config compile.toml --config $BUILDCC_HOME/host/host_toolchain.toml

* Your target will now be present in ``[build_dir]/[toolchain_name]/[target_name]`` (taken from ``build.toml`` and ``build.helloworld.cpp``)

Directory structure
++++++++++++++++++++

.. uml::
    
    @startmindmap
    * [workspace]
    ** [src]
    *** main.cpp
    ** build.helloworld.cpp
    ** compile.toml
    ** build.toml 
    @endmindmap

Write your C++ "script"
++++++++++++++++++++++++

From the "script" below we can see that we have a few lines of **boilerplate**

* Setup args
* Setup register (pre and post callback requirements)

We then setup our main **toolchain**-**target** pairs. Highlighted below

* Specify your toolchain
   * Verify the toolchain existance on your machine by using the ``.Verify`` API
   * If multiple similar toolchains are detected (due to multiple installations), the first found toolchain is picked
   * You can pass in the ``VerifyToolchainConfig`` to narrow down your search and verification.
* Specify your compatible target
   * Every specific target is meant to use a specific target.
   * For example: ``ExecutableTarget_gcc`` specialized target can use the ``Toolchain_gcc`` specialized toolchain but not ``Toolchain_msvc``.

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 25,26,27,29,30
    :caption: build.helloworld.cpp

    #include "buildcc.h"

    using namespace buildcc;

    void clean_cb();
    // All specialized targets derive from BaseTarget
    void hello_world_build_cb(BaseTarget & target);

    int main(int argc, char ** argv) {
        // Step 1. Setup your args
        Args args;
        ArgToolchain arg_gcc;
        args.AddToolchain("gcc", "GCC toolchain", arg_gcc);
        args.Parse(argc, argv);

        // Step 2. Register
        Register reg(args);

        // Step 3. Pre build steps
        // for example. clean your environment
        reg.Clean(clean_cb);

        // Step 4. Build steps
        // Main setup
        Toolchain_gcc gcc;
        auto verified_gcc_toolchains = gcc.Verify();
        env::assert_fatal(!verified_gcc_toolchains.empty(), "GCC toolchain not found");

        ExecutableTarget_gcc hello_world("hello_world", gcc, "");
        reg.Build(arg_gcc.state, hello_world_build_cb, hello_world);

        // Step 5. Build your targets
        reg.RunBuild();

        // Step 6. Post build steps
        // for example. clang compile commands database
        plugin::ClangCompileCommands({&hello_world}).Generate();

        return 0;
    }

    void clean_cb() {
        fs::remove_all(env::get_project_build_dir());
    }

    void hello_world_build_cb(BaseTarget & target) {
        // Add your source
        target.AddSource("src/main.cpp");

        // Initializes the target build tasks
        target.Build();
    }

Write your ``compile.toml`` file
++++++++++++++++++++++++++++++++

.. code-block:: toml
    :linenos:
    :caption: compile.toml

    # Settings
    root_dir = ""
    build_dir = "_build_internal"
    loglevel = "info"
    clean = false

    # BuildExe run mode
    mode = "script"

    # Target information
    name = "build.helloworld"
    type = "executable"
    relative_to_root = ""
    srcs = ["build.helloworld.cpp"]

    [script]
    configs = ["build.toml"]

Write your ``build.toml`` file
+++++++++++++++++++++++++++++++

.. code-block:: toml
    :linenos:
    :caption: build.toml

    # Root
    root_dir = ""
    build_dir = "_build"
    loglevel = "debug"

    # Project
    clean = false

    # Toolchain
    [toolchain.gcc]
    build = true
    test = false
