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

Helloworld "fmtlib" example
----------------------------

* Git clone the ``fmt`` library into your ``ENV{BUILDCC_HOME}/libs`` folder
* Run ``buildexe libs --help-all``.
   * You should automatically see the library folder name pop up under the ``libs`` submodule.
   * In this case it will be the ``fmt`` option.

.. code-block:: shell
    :linenos:
    :emphasize-lines: 8

    script
        Options:
            --configs TEXT ...          Config files for script mode

    libs
        Libraries
        Options:
            --fmt TEXT ...              fmt library

* Since we want to use the ``fmt`` library in our project we can now write your ``compile.toml`` file as given below.

Directory structure
+++++++++++++++++++++

.. uml::
    
    @startmindmap
    * [workspace]
    ** [src]
    *** main.cpp
    ** build.helloworld.cpp
    ** compile.toml
    ** build.toml 
    @endmindmap

Write your fmtlib build files
++++++++++++++++++++++++++++++

.. note:: This process might seem like a hassle. But please note that fmtlib does not currently have support for BuildCC like build files and it must be provided by the user.

.. code-block:: cpp
    :linenos:
    :caption: build.fmt.h

    #pragma once

    #include "buildcc.h"

    using namespace buildcc;
    
    /**
    * @brief User configurable options
    * default_flags: Adds default preprocessor, compile and link flags to the fmt
    * library if true. If false these would need to be provided by the user.
    */
    struct FmtConfig {
        bool default_flags{true};
        // NOTE, Add more options here as required to customize your fmtlib build
    };

    /**
    * @brief Build the libfmt static or dynamic library
    *
    * @param target Initialized specialized library target
    * @param config See FmtConfig above
    */
    void build_fmt_cb(BaseTarget& target, const FmtConfig& config = FmtConfig());

    /**
    * @brief Information for fmt header only library
    *
    * @param target_info Holds the include dirs, headers and preprocessor flag
    * information
    */
    void build_fmt_ho_cb(TargetInfo& target_info);

.. code-block:: cpp
    :linenos:
    :caption: build.fmt.cpp

    #include "build.fmt.h"

    void build_fmt_cb(BaseTarget& target, const FmtConfig& config) {
        target.AddSource("src/os.cc");
        target.AddSource("src/format.cc");
        target.AddIncludeDir("include", false);
        target.GlobHeaders("include/fmt");
    
        // Toolchain specific flags added 
        // if default_flags == true
        if (config.default_flags) {
            switch (target.GetToolchain().GetId()) {
            case ToolchainId::Gcc:
                target.AddCppCompileFlag("-std=c++11");
                break;
            case ToolchainId::MinGW:
                target.AddCppCompileFlag("-std=c++11");
                break;
            case ToolchainId::Msvc:
                target.AddCppCompileFlag("/std:c++11");
                break;
            default:
                break;
            }
        }

        target.Build();
    }

    void build_fmt_ho_cb(TargetInfo& target_info) {
        target_info.AddIncludeDir("include", false);
        target_info.GlobHeaders("include/fmt");
        target_info.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
    }


Write your C++ "script"
++++++++++++++++++++++++

Write your ``compile.toml`` file
++++++++++++++++++++++++++++++++

Write your ``build.toml`` file
+++++++++++++++++++++++++++++++

Write your ``main.cpp`` helloworld example in fmtlib
++++++++++++++++++++++++++++++++++++++++++++++++++++

.. code-block:: cpp
    
    #include "fmt/format.h"
    int main() {
        fmt::print("{} {}", "Hello", "World");
        return 0;
    }
