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

target_getter.h
-----------------

.. doxygenclass:: buildcc::internal::TargetInfoGetter

TargetInfo
===========

target_info.h
--------------

.. doxygenclass:: buildcc::TargetInfo

.. doxygentypedef:: BaseTargetInfo

Target APIs
=============

.. important:: Target APIs can also use TargetInfo APIs

target_info_getter.h
---------------------

.. doxygenclass:: buildcc::internal::TargetGetter

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
