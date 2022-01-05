Introduction
=============

Aim
----

BuildCC aims to be an alternative to Makefiles while using the feature rich C++ language instead of a custom DSL.

Features
----------

* Complete flexibility for custom workflows and toolchains
* C++ language feature benefits and debuggable build binaries
* Optimized rebuilds through serialization. See target.fbs schema
   * Can optimize for rebuilds by comparing the previous stored build with current build.
   * Also see the FAQ for more details on Serialization
* Customizable for community plugins.

Pre-requisites
--------------

* Technical Knowledge
   * C++03 classes and template usage
* C++17 Compiler with
   * C++17 filesystem library support
   * C++11 thread library support
* Third Party Libraries (See License below)
   * Flatbuffers v2.0.0
   * Taskflow v3.1.0
   * CLI11 v2.1.0
   * Tiny Process Library v2.0.4
   * fmt v8.0.1
   * spdlog v1.9.2
   * CppUTest v4.0

General Information
-------------------

* A one stage **input / output** procedure is called a **Generator** with a wide variety of use cases
   * Single input creates single output
   * Single input creates multiple outputs
   * Multiple inputs create single output
   * Multiple inputs creates multiple outputs
* A two stage **compile** and **link** procedure is called a **Target**
   * This means that Executables, StaticLibraries and DynamicLibraries are all categorized as Targets
   * In the future C++20 modules can also be its own target depending on compiler implementations
* Every Target requires a complementary (and compatible) **Toolchain**
   * This ensures that cross compiling is very easy and explicit in nature.
   * Multiple toolchains can be `mixed` in a single build file i.e we can generate targets using the GCC, Clang, MSVC and many other compilers **simultaneously**.
* The **compile_command** (pch and object commands) and **link_command** (target command) is fed to the **process** call to invoke the **Toolchain**.
* Each **Target** can depend on other targets efficiently through Parallel Programming using **Taskflow**.
   * Dependency between targets is explicitly mentioned through the Taskflow APIs
   * This has been made easier for the user through the ``buildcc::Register`` module.
* Build files can be customized through command line arguments
   * Command line arguments can be stored in configurable ``.toml`` files and passed using the ``--config`` flag.
   * Users can define their own custom arguments.
   * Argument passing has been made easy using the ``buildcc::Args`` module.

Licenses
---------

`BuildCC` is licensed under the Apache License, Version 2.0. See **LICENSE** for the full license text. `BuildCC` aims to use open-source libraries containing permissive licenses. 

.. note:: Developers who would like to suggest an alternative library, raise an issue with the **license** and **advantages** clearly outlined.

* `Fmtlib <https://github.com/fmtlib/fmt>`_ (Formatting) [MIT License] [Header Only]
* `Spdlog <https://github.com/gabime/spdlog>`_ (Logging) [MIT License] [Header Only]
* `Tiny Process Library <https://gitlab.com/eidheim/tiny-process-library>`_ (Process handling) [MIT License]
* `Taskflow <https://github.com/taskflow/taskflow>`_ (Parallel Programming) [MIT License] [Header Only]
   * See also `3rd-Party <https://github.com/taskflow/taskflow/tree/master/3rd-party>`_ used by Taskflow
* `Flatbuffers <https://github.com/google/flatbuffers>`_ (Serialization) [Apache-2.0 License] [Header Only]
* `CLI11 <https://github.com/CLIUtils/CLI11>`_ (Argument Parsing) [BSD-3-Clause License] [Header Only]
* `CppUTest <https://github.com/cpputest/cpputest>`_ (Unit Testing/Mocking) [BSD-3-Clause License]
