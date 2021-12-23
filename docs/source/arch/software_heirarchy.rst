Software Heirarchy
==================

BuildCC single lib
-------------------

**BuildCC** sources are compiled into a single library

* The easiest way to use ``BuildCC``
* After building the project all we need to do is ``-lbuildcc -ltiny-process-library`` or equivalent

.. uml::
   
   rectangle Flatbuffers as flatbuffers
   rectangle fmt as fmt
   rectangle spdlog as spdlog
   rectangle Taskflow as taskflow
   rectangle CLI11 as cli11
   rectangle "tiny-process-library" as tpl

   rectangle BuildCC as buildcc

   flatbuffers -up-> buildcc
   fmt -up-> buildcc
   spdlog -up-> buildcc
   taskflow -up-> buildcc
   cli11 -up-> buildcc
   tpl -up-> buildcc


BuildCC interface lib
---------------------

**BuildCC** is broken up into multiple smaller libraries

* This has been done mainly for unit-testing and mocking segregation
* It helps to easily architect the ``BuildCC`` library by visualizing internal dependencies
* Please see :doc:`testing` for more information of how the ``mock_*`` equivalent of these libraries are used

.. uml::

   rectangle Flatbuffers as flatbuffers #palegreen
   rectangle fmt as fmt #palegreen
   rectangle spdlog as spdlog #palegreen
   rectangle Taskflow as taskflow #palegreen
   rectangle CLI11 as cli11 #palegreen
   rectangle "tiny-process-library" as tpl #palegreen

   rectangle Environment as env #aliceblue
   rectangle Toolchain as toolchain #aliceblue
   rectangle Target as target #aliceblue
   rectangle "Toolchain specialized" as toolchain_specialized #aliceblue
   rectangle "Target specialized" as target_specialized #aliceblue
   rectangle Args as args #aliceblue
   rectangle Register as register #aliceblue
   rectangle "Supported Plugins" as plugins #aliceblue
   rectangle BuildCC as buildcc


   fmt -up-> env
   spdlog .up.> env
   tpl .up.> env

   flatbuffers .up.> target
   taskflow -up-> target

   cli11 -up-> args
   taskflow -up-> register

   env -up-> toolchain

   toolchain -up-> target
   toolchain -up-> toolchain_specialized

   target -up-> target_specialized

   target -up-> args
   target -up-> register
   target -up-> plugins

   toolchain_specialized -up-> buildcc
   target_specialized -up-> buildcc
   args -up-> buildcc
   register -up-> buildcc
   plugins -up-> buildcc
