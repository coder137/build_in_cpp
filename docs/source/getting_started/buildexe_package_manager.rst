BuildExe as a Package Manager
=============================

When we are **compiling** our "script" to an executable we can also add additional library build files which define how the library is built.

The procedure is similar to git cloning the library to the **ENV{BUILDCC_HOME}/libs** folder using the ``libs`` options in buildexe.

Please see :doc:`buildexe_setup` to setup your libs folder appropriately.

Basic Procedure
----------------

.. uml::

    usecase "build.user_project.cpp" as build_cpp
    usecase "libs/library/build.library.h" as build_lib_header
    usecase "libs/library/build.library.cpp" as build_lib_source

    usecase "compile.toml" as compile_toml
    usecase "host_toolchain.toml" as host_toolchain_toml
    rectangle "./build.user_project" as build_project_exe
    usecase "build.toml" as build_toml

    rectangle "./buildexe" as buildexe_exe

    artifact "library" as library
    artifact "./hello_world" as hello_world_exe

    build_cpp -right-> buildexe_exe
    build_lib_header -right-> buildexe_exe
    build_lib_source -right-> buildexe_exe

    compile_toml -up-> buildexe_exe
    host_toolchain_toml -up-> buildexe_exe

    buildexe_exe -right-> build_project_exe

    build_toml -up-> build_project_exe
    build_project_exe -right-> hello_world_exe

    library -up-> hello_world_exe
