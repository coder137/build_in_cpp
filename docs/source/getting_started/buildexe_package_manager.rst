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

        // Register your fmt lib tasks
        target.Build();
    }

    void build_fmt_ho_cb(TargetInfo& target_info) {
        target_info.AddIncludeDir("include", false);
        target_info.GlobHeaders("include/fmt");
        target_info.AddPreprocessorFlag("-DFMT_HEADER_ONLY=1");
    }


Write your C++ "script"
++++++++++++++++++++++++

* Boilerplate is similar to the BuildExe helloworld "script" example in :doc:`buildexe_script_example`

**Core build setup is highlighted below**

* On line 4 we include our ``build.fmt.h`` include file. See ``compile.toml`` libs submodule to correlate
* On line 8 we include the ``buildexe_lib_dirs.h`` include file. This is a generated include file which contains the absolute paths of the library folders.
   * Access is through ``BuildExeLibDir::[lib_folder_name]``
   * This is the reason why we need to make sure that our git cloned library folder name is also a valid C++ variable name.
* On line 40 we point to the absolute ``fmt`` libs folder path for the sources and **redirect** the output to our ``env::get_project_build_dir() / "fmt"`` folder.
   * In this way we can safely use out of root projects and redirect the output files to our build location
   * There are other input source -> output object redirect options through additional APIs.
* On line 43 and 44 we directly use our fmtlib build APIs to define how fmtlib should be built
* On line 47 and 48 we define our Hello World executable target
   * See ``main.cpp`` below for fmtlib hello world example
   * See ``hello_world_build_cb`` for build steps
* On line 79 ``hello_world_build_cb`` in additional to compiling our ``main.cpp`` file
   * We need to link our compiled ``fmt_lib`` using the ``AddLibDep`` API
   * We also insert the ``fmt_lib`` include dirs to the hello world target since we need to ``#include "fmt/format.h"`` in ``main.cpp``
* On line 52 we register a dependency of ``fmt_lib`` on ``hello_world``. 
   * This guarantees that the fmt library will be built before the hello world executable.
   * This is essential because we need to **link** fmtlib with our hello world executable.

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 4,8,38,39,40,43,44,47,48,52,79

    #include "buildcc.h"

    // Included through libs
    #include "build.fmt.h"

    // Generated by BuildCC
    // See the `_build_internal` directory
    #include "buildexe_lib_dirs.h"

    using namespace buildcc;

    // Function Prototypes
    static void clean_cb();
    static void hello_world_build_cb(BaseTarget &target, BaseTarget &fmt_lib);

    int main(int argc, char **argv) {
        // 1. Get arguments
        Args args;
        ArgToolchain arg_gcc;
        args.AddToolchain("gcc", "Generic gcc toolchain", arg_gcc);
        args.Parse(argc, argv);

        // 2. Initialize your environment
        Register reg(args);

        // 3. Pre-build steps
        reg.Clean(clean_cb);

        // 4. Build steps
        // Explicit toolchain - target pairs
        Toolchain_gcc gcc;
        auto verified_toolchains = gcc.Verify();
        env::assert_fatal(!verified_toolchains.empty(), "GCC Toolchain not found");

        // Setup your [Library]Target_[toolchain] fmtlib instance
        // Update your TargetEnv to point to `BuildExeLibDir::fmt` folder
        // The generated build files will go into your current `project_build_dir / fmt` folder
        StaticTarget_gcc fmt_lib(
            "libfmt", gcc,
            TargetEnv(BuildExeLibDir::fmt, env::get_project_build_dir() / "fmt"));
        
        // We use the build.fmt.h and build.fmt.cpp APIs to define how we build our fmtlib
        FmtConfig fmt_config;
        reg.Build(arg_gcc.state, build_fmt_cb, fmt_lib, fmt_config);

        // Define our hello world executable
        ExecutableTarget_gcc hello_world("hello_world", gcc, "");
        reg.Build(arg_gcc.state, hello_world_build_cb, hello_world, fmt_lib);
        
        // Fmt lib is a dependency to the Hello world executable
        // This means that fmt lib is guaranteed to be built before the hello world executable
        reg.Dep(hello_world, fmt_lib);

        // 5. Test steps i.e Hello world is automatically run
        reg.Test(arg_gcc.state, "{executable}", hello_world);

        // 6. Build Target
        // Builds libfmt.a and ./hello_world
        reg.RunBuild();

        // 7. Test Target
        // Executes ./hello_world
        // Output -> Hello World
        reg.RunTest();

        // 8. Post Build steps
        // - Clang Compile Commands
        plugin::ClangCompileCommands({&hello_world}).Generate();
        // - Graphviz dump
        std::cout << reg.GetTaskflow().dump() << std::endl;

        return 0;
    }

    static void clean_cb() {
        fs::remove_all(env::get_project_build_dir());
    }

    static void hello_world_build_cb(BaseTarget &target, BaseTarget &fmt_lib) {
        target.AddSource("main.cpp", "src");

        // Add fmt_lib as a library dependency
        target.AddLibDep(fmt_lib);
        // We need to insert the fmt lib include dirs and header files into our hello_world executable target (naturally)
        target.Insert(fmt_lib, {
                                    SyncOption::IncludeDirs,
                                    SyncOption::HeaderFiles,
                                });
        
        // Register your tasks
        target.Build();
    }


Write your ``compile.toml`` file
++++++++++++++++++++++++++++++++

* The only difference from the ``compile.toml`` in :doc:`buildexe_script_example` is the additional of the ``libs`` submodule
* We use the ``fmt`` option since we git cloned the library into the libs folder
* We add the various fmt build files that need to be compiled with our "script"
* See highlighed lines 19 and 20

.. code-block:: toml
    :linenos:
    :emphasize-lines: 19,20

    # Settings
    root_dir = ""
    build_dir = "_build_internal"
    loglevel = "debug"
    clean = false

    # BuildExe run mode
    mode = "script"

    # Target information
    name = "single"
    type = "executable"
    relative_to_root = ""
    srcs = ["build.main.cpp"]

    [script]
    configs = ["build.toml"]

    [libs]
    fmt = ["build.fmt.cpp", "build.fmt.h"]


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
