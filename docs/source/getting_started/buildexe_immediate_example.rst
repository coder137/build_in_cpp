BuildExe "Immediate" example
=============================

Basic Procedure
----------------

BuildExe has an "immediate" mode where it can directly provide us the Target (singular) without going through the intermediate steps that we followed in the "script" mode.

.. uml::

    usecase "main.cpp" as main_cpp
    usecase "compile.toml" as compile_toml
    usecase "host_or_cross_toolchain.toml" as host_or_cross_toolchain

    rectangle "./buildexe" as buildexe_exe
    artifact "./hello_world" as hello_world_exe

    main_cpp -right-> buildexe_exe
    compile_toml -up-> buildexe_exe
    host_or_cross_toolchain -up-> buildexe_exe
    buildexe_exe -right-> hello_world_exe


What is the point of the "script" mode then?
++++++++++++++++++++++++++++++++++++++++++++

The "immediate" mode has a lot of limitations but it is also useful in certain scenarios

**Limitations**

* Cannot build more than one target at a time
* No customizability allowed.
   * Which means that apart from just building the target you cannot do anything else. 
   * For example: Setting dependencies between targets, running custom generators, running static analysis tools and so on.

**Usecase**

* Simple way to build one target.
* Completely run time dependent. Change your ``build.toml`` file and you can build a new target.
* Very easy to know how a particular target is built. 
   * For example. In a large project it might be very hard to visualize how a single target is built due to various code generation and library / target dependencies.
   * Since .toml is easily readable, we can understand the sources, search directories and flags that the target requires at a glance.
* Can be shipped to customers for a pain free build process i.e removes technical debt.
   * Building your artifact is as simple as ``buildexe --config build.toml --config %BUILDCC_HOME/host/host_or_cross_toolchain.toml``
   * build.toml contains the target information.
   * host_or_cross_toolchain.toml contains the host/cross toolchain information
   * We can combine the two into one .toml file.


Helloworld "immediate" example
------------------------------

* Write your ``build.toml`` file
* Invoke ``buildexe`` from the command line from the **[workspace]** folder
   * Pay attention to the ``root_dir`` and ``build_dir`` parameters set in your ``build.toml`` file. 
   * These directories are relative to the directory from which you **invoke** buildexe

.. code-block:: bash

    ./buildexe --config build.toml --config $BUILDCC_HOME/host/host_toolchain.toml

* Your target will now be present in ``[build_dir]/[toolchain_name]/[target_name]`` (taken from ``build.toml``)

Directory structure
++++++++++++++++++++

.. uml::
    
    @startmindmap
    * [workspace]
    ** [src]
    *** main.cpp
    ** build.toml 
    @endmindmap

Write your ``build.toml`` file
+++++++++++++++++++++++++++++++

.. code-block:: toml

    # Settings
    root_dir = ""
    build_dir = "_build"
    loglevel = "info"
    clean = false

    # BuildExe run mode
    mode = "immediate"

    # Target information
    name = "hello_world"
    type = "executable"
    relative_to_root = "src"
    srcs = ["main.cpp"]
