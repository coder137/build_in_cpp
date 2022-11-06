Target Info APIs
=================

source_api.h
-------------

.. doxygenclass:: buildcc::internal::SourceApi

include_api.h
---------------

.. doxygenclass:: buildcc::internal::IncludeApi

lib_api.h
----------

.. doxygenclass:: buildcc::internal::LibApi

pch_api.h
-----------

.. doxygenclass:: buildcc::internal::PchApi

flag_api.h
------------

.. doxygenclass:: buildcc::internal::FlagApi

deps_api.h
-----------

.. doxygenclass:: buildcc::internal::DepsApi

sync_api.h
------------

.. doxygenclass:: buildcc::internal::SyncApi

TargetInfo
===========

target_info.h
--------------

.. doxygenclass:: buildcc::TargetInfo

.. doxygentypedef:: BaseTargetInfo

Target APIs
=============

.. important:: Target APIs can also use TargetInfo APIs

target_env.h
---------------------

.. doxygenclass:: buildcc::internal::TargetEnvApi

Target
=======

target.h
---------

.. doxygenclass:: buildcc::Target

.. doxygentypedef:: buildcc::BaseTarget

Specialized Target
==================

target_custom.h
---------------

.. doxygentypedef:: buildcc::Target_custom

target_gcc.h
-------------

.. doxygenclass:: buildcc::ExecutableTarget_gcc

.. doxygenclass:: buildcc::StaticTarget_gcc

.. doxygenclass:: buildcc::DynamicTarget_gcc

target_msvc.h
-------------

.. doxygenclass:: buildcc::ExecutableTarget_msvc

.. doxygenclass:: buildcc::StaticTarget_msvc

.. doxygenclass:: buildcc::DynamicTarget_msvc

target_generic.h
-----------------

.. doxygenclass:: buildcc::ExecutableTarget_generic

.. doxygenclass:: buildcc::StaticTarget_generic

.. doxygenclass:: buildcc::DynamicTarget_generic

.. doxygenclass:: buildcc::Target_generic

Example
--------

.. code-block:: cpp
    :linenos:

    // Generic toolchain GCC
    Toolchain_gcc gcc;

    // Target compatible GCC toolchain
    ExecutableTarget_gcc hello_world("name", gcc, "relative_to_global_env_root_dir");

    // NOTE: See APIs above, they are self explanatory
    hello_world.AddSource("");

    // Setup the tasks
    hello_world.Build();

    // Run the task
    tf::Executor executor;
    executor.run(hello_world.GetTaskflow());
    executor.wait_for_all();
