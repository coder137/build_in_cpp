GCC
====

Lowlevel GCC Tests

Simple
--------

Compile a single source

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 8

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "Simple" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc target("Simple", gcc, "");
    target.AddSource("main.cpp");
    target.Build();

    // Build
    tf::Executor executor;
    executor.run(target.GetTaskflow());
    executor.wait_for_all();


IncludeDir
----------

Compile multiple sources with header files

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 12,15

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "IncludeDir" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc target("IncludeDir", gcc, "files");
    target.AddSource("main.cpp", "src");
    target.AddSource("src/random.cpp");

    // Track header for rebuilds
    target.AddHeader("include/random.h");

    // Add include dir to search paths
    target.AddIncludeDir("include");
    target.Build();

    // Build
    tf::Executor executor;
    executor.run(target.GetTaskflow());
    executor.wait_for_all();

StaticLib
----------

Compile a static library which is used by an executable

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 7

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "librandom.a" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    StaticTarget_gcc statictarget("librandom", gcc, "files");
    statictarget.AddSource("src/random.cpp");
    statictarget.AddHeader("include/random.h");
    statictarget.AddIncludeDir("include");
    statictarget.Build();

    // GCC specialized targets
    // Create "statictest" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc exetarget("statictest", gcc, "files");
    exetarget.AddSource("main.cpp", "src");
    exetarget.AddIncludeDir("include");
    exetarget.AddLibDep(statictarget);
    exetarget.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // Explicitly setup your dependencies
    tf::Task statictargetTask = taskflow.composed_of(statictarget.GetTaskflow());
    tf::Task exetargetTask = taskflow.composed_of(exetarget.GetTaskflow());
    exetargetTask.succeed(statictargetTask);

    // Run
    executor.run(taskflow);
    executor.wait_for_all();

DynamicLib
-----------

Compile a dynamic library which is used by an executable

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 7

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "librandom.so" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    DynamicTarget_gcc dynamictarget("librandom", gcc, "files");
    dynamictarget.AddSource("src/random.cpp");
    dynamictarget.AddHeader("include/random.h");
    dynamictarget.AddIncludeDir("include");
    dynamictarget.Build();

    // GCC specialized targets
    // Create "dynamictest" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc target("dynamictest", gcc, "files");
    target.AddSource("main.cpp", "src");
    target.AddIncludeDir("include");
    target.AddLibDep(dynamictarget);
    target.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // Explicitly setup your dependencies
    auto dynamictargetTask = taskflow.composed_of(dynamictarget.GetTaskflow());
    auto targetTask = taskflow.composed_of(target.GetTaskflow());
    targetTask.succeed(dynamictargetTask);

    executor.run(taskflow);
    executor.wait_for_all();

    // Post Build step
    if (target.IsBuilt()) {
        fs::path copy_to_path =
            target.GetTargetBuildDir() / dynamictarget.GetTargetPath().filename();
        fs::copy(dynamictarget.GetTargetPath(), copy_to_path);
    }

.. note:: Our ``ExecutableTarget_gcc`` depends on ``DynamicTarget_gcc`` and requires the ``librandom.so`` file to be present in the same folder location as the executable when running.

Flags
------

Using **PreprocessorFlags**, **C Compile flags**, **Cpp Compile flags** and **Link flags**

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 12,13,14,15,23,24,25,26

    // GCC specialized toolchain
    Toolchain_gcc toolchain;

    // GCC specialized targets
    // Create "CppFlags" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc cpptarget("CppFlags", gcc, "files");
    cpptarget.AddSource("main.cpp", "src");
    cpptarget.AddSource("src/random.cpp");
    cpptarget.AddHeader("include/random.h");
    cpptarget.AddIncludeDir("include");
    cpptarget.AddPreprocessorFlag("-DRANDOM=1");
    cpptarget.AddCppCompileFlag("-Wall");
    cpptarget.AddCppCompileFlag("-Werror");
    cpptarget.AddLinkFlag("-lm");
    cpptarget.Build();

    // Gcc specialized targets
    // Create "CFlags" target (meant to use the GCC compiler)
    // On Windows the equivalent is the MinGW compiler
    ExecutableTarget_gcc ctarget("CFlags", gcc, "files");
    ctarget.AddSource("main.c", "src");
    ctarget.AddPreprocessorFlag("-DRANDOM=1");
    ctarget.AddCCompileFlag("-Wall");
    ctarget.AddCCompileFlag("-Werror");
    ctarget.AddLinkFlag("-lm");
    ctarget.Build();

    // Build
    tf::Executor executor;
    tf::Taskflow taskflow;

    // There isn't any dependency between the 2 targets
    taskflow.composed_of(cpptarget.GetTaskflow());
    taskflow.composed_of(ctarget.GetTaskflow());

    executor.run(taskflow);
    executor.wait_for_all();

AfterInstall
-------------

Use BuildCC with CMake


* Install ``BuildCC`` via CMake to your system and add it to **PATH**
* Use the script below to **compile** your build script to an executable
* Copy the **Flags** build example
* Run the executable from your project root directory

.. code-block:: cmake

    # Package dependencies
    # fmt is imported by spdlog by default
    find_package(fmt_package NAMES "fmt" REQUIRED)
    find_package(spdlog_package NAMES "spdlog" REQUIRED)
    find_package(nlohmann_json_package NAMES "nlohmann_json" REQUIRED)
    find_package(taskflow_package NAMES "Taskflow" "taskflow" REQUIRED)
    find_package(CLI11_package NAMES "CLI11" REQUIRED)
    find_package(tiny_process_library_package NAMES "tiny-process-library" REQUIRED)

    find_package(buildcc_package NAMES "buildcc" REQUIRED)

    message("Find package: ${fmt_package_DIR}")
    message("Find package: ${spdlog_package_DIR}")
    message("Find package: ${nlohmann_json_package_DIR}")
    message("Find package: ${taskflow_package_DIR}")
    message("Find package: ${CLI11_package_DIR}")
    message("Find package: ${tiny_process_library_package_DIR}") #

    message("Find package: ${buildcc_package_DIR}")

    # build executable
    add_executable(build build.cpp)
    target_include_directories(build PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
    target_link_libraries(build PRIVATE 
        buildcc
    )
    if (${MINGW})
        message(WARNING "-Wl,--allow-multiple-definition for MINGW")
        target_link_options(build PRIVATE -Wl,--allow-multiple-definition)
    endif()

    # Add your constants file for the environment
    configure_file(constants.h.in ${CMAKE_BINARY_DIR}/generated/constants.h @ONLY)


Plugins
--------

Demonstrating BuildCC supported plugin usage

* From the **Flags** example above
* Add the targets for which you would like to generate the `Clang CompileCommands Database <https://clang.llvm.org/docs/JSONCompilationDatabase.html>`_

.. code-block:: cpp
    :linenos:

    plugin::ClangCompileCommands({&cppflags, &cflags}).Generate();

PrecompileHeader
----------------

TODO
