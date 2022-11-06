Project Layout
==============

.. uml::

   @startmindmap
   * [root]
   ** .clang-format
   ** .gitmodules
   ** CMakeLists.txt
   ** CMakePresets.json
   ** codecov.yml
   ** LICENSE
   ** README.md
   ** TODO.md
   * buildcc
   ** lib
   *** env
   *** toolchain
   *** target
   *** args
   *** register
   ** schema
   ** toolchains
   ** targets
   * bootstrap
   * buildexe
   * cmake
   ** coverage
   ** flags
   ** target
   ** tool
   * docs
   * example
   ** gcc
   ** msvc
   ** hybrid
   * third_party
   ** CLI11
   ** cpputest
   ** json
   ** fmt
   ** spdlog
   ** taskflow
   ** tiny-process-library
   @endmindmap

[workspace root]
----------------

* .clang-format
   * LLVM style clang format
* .gitmodules
   * See ``third_party`` folder for git submodules
* CMakeLists.txt
   * Main project level CMakeLists
   * Contains all the configurable options for CMake
* CMakePresets.json
   * Default configurable options for different presets / compilers
* codecov.yml
   * Codecov parameters
* LICENSE
   * Apache-2.0 License
* README.md
   * Project outline and details
* TODO.md
   * TODO list for community interactions

bootstrap
---------

Bootstrap build files to compile the ``build_in_cpp`` project

* Contains build files for compiling ``third_party`` libraries
   * Most of them are header only
   * ``tiny-process-library`` is the only library that compiles to a static / dynamic library
* Contains build file for ``buildcc`` library compilation

buildcc
--------

Contains the core ``buildcc`` library

* schema
* lib
   * env
   * toolchain
   * target
   * args
* targets
* toolchains
* plugins

buildexe
---------

Standalone ``buildcc`` executable for compiling projects in immediate mode or script mode


cmake
-------

Global cmake variables and custom_targets

* coverage
   * gcovr.cmake
   * lcov.cmake
* flags
   * build_flags.cmake
   * test_flags.cmake
* target
   * json.cmake
   * fmt.cmake
   * spdlog.cmake
   * tpl.cmake
   * taskflow.cmake
   * cli11.cmake
   * cpputest.cmake
* tool
   * clangtidy.cmake
   * cppcheck.cmake
   * doxygen.cmake

docs
-----

Project documentation

* arch
   * Project / Software architecture documentation
* user_api
   * User usable APIs

example
---------

* gcc
   * Lowlevel tests for the GCC compiler
* msvc
   * Lowlevel tests for the MSVC compiler
* hybrid
   * Real world usages with both GCC and MSVC compiler support

third_party
-----------

* JSON
* Fmtlib
* Spdlog
* CLI11
* Taskflow
* Tiny Process Library
* CppUTest
