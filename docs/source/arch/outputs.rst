Outputs
=======

BuildCC Library
-----------------

The ``build_in_cpp`` project aims to remove DSLs by writing build files in C++.

However our C++ "script" first needs to be **built (compiled and linked)** then **executed (build targets)**.

When building (compiling and linking) our C++ "script" we need to link the ``buildcc`` library as well to provide the "script" with ``buildcc`` APIs.

BuildExe Executable
---------------------

``BuildExe`` is a standalone executable similar to ``make.exe``.

As part of the current release ``BuildExe`` requires the following folder structure. 

.. note:: Proper details of ``BuildExe`` usage to be added. For now this is in its experimental stage with only support for GCC, MinGW and MSVC compilers.

.. uml::

   @startmindmap
   * ENV[BUILDCC_HOME]
   ** buildcc
   ** extensions
   ** libs
   ** host
   @endmindmap

ENV[BUILDCC_HOME]
^^^^^^^^^^^^^^^^^^

Create your BUILDCC_HOME directory. For example: ``C:/BUILDCCHOME`` or ``local/mnt/BUILDCCHOME`` and add this path to the **environment variable BUILDCC_HOME**.

buildcc
^^^^^^^^

This directory contains the git cloned ``build_in_cpp`` repository

extensions
^^^^^^^^^^^

This directory contains several git cloned second and third party plugins and extensions

.. note:: BuildExe will have the ability to directly use these extensions after specifying them in the .toml file

libs
^^^^^

This directory contains several git cloned libraries that need to be used in your projects or code base once they are specified in your .toml file.

In this way, the ``build_in_cpp`` project automatically behaves like a package manager.

host
^^^^^

Host toolchain toml files that contain information pertaining to the various host toolchains installed on your operating system.

.. note:: The goal is to have a standalone executable to detect the host toolchains and automatically generate .toml files for each one of them. Users can then use these host toolchain files for building their "script"

.. note:: Consider adding a **cross** folder for cross compiled toolchains are well which cannot be used to build the "script" but instead can be used by the script to build cross compiled targets.
