Testing
=======

Test libs 
----------

* ``mock_env``
* ``mock_toolchain``
* ``mock_target``

mock_env
^^^^^^^^

* ``assert_handle_fatal`` uses ``throw std::exception`` instead of ``std::terminate``
* We can now use the CppUTest ``CHECK_THROWS`` API when a failure occurs

.. doxygenfunction:: assert_handle_fatal


* ``Command::Execute`` is mocked out to CppUMock ``.actualCall("execute")`` API
* We can redirect stdout and stderr through the actual call, with our own information to match expectations using the CppUMock ``.withOutputParameterOfType`` API
* Provided with the ``CommandExpect_Execute`` mocked API
* The ``command`` and ``working_directory`` params are not used

.. doxygenfunction:: CommandExpect_Execute

* All of the logging APIs have been **stubbed** out since they are unnecessary for unit tests and very noisy when getting output
* A better alternative would be debugging unit tests or adding the CppUTest ``UT_PRINT`` statement instead

.. doxygenclass:: buildcc::env::m::VectorStringCopier

mock_toolchain
^^^^^^^^^^^^^^

* Does not override any classes / functions

mock_target
^^^^^^^^^^^^

.. doxygenfunction:: GeneratorRunner

.. doxygenfunction:: GeneratorExpect_InputRemoved

.. doxygenfunction:: GeneratorExpect_InputAdded

.. doxygenfunction:: GeneratorExpect_InputUpdated

.. doxygenfunction:: GeneratorExpect_OutputChanged

.. doxygenfunction:: GeneratorExpect_CommandChanged

From the :doc:`serialization_schema` **generator** we can see that our generator has 3 rebuild conditions

* Inputs
   * Removed state (input file is removed)
   * Added state (input file is added)
   * Updated state (input file is updated)
* Outputs
   * Changed (output file is added or removed)
* Commands
   * Changed (command is added or removed)

.. doxygenfunction:: TargetRunner

.. doxygenfunction:: TargetExpect_SourceRemoved

.. doxygenfunction:: TargetExpect_SourceAdded

.. doxygenfunction:: TargetExpect_SourceUpdated

.. doxygenfunction:: TargetExpect_PathRemoved

.. doxygenfunction:: TargetExpect_PathAdded

.. doxygenfunction:: TargetExpect_PathUpdated

.. doxygenfunction:: TargetExpect_DirChanged

.. doxygenfunction:: TargetExpect_FlagChanged

.. doxygenfunction:: TargetExpect_ExternalLibChanged

From the :doc:`serialization_schema` **target** we can see that our generator has multiple rebuild conditions

Anything associated with ``Path`` has 3 states i.e Added, Removed or Updated

Everything else has only 2 states i.e Added or Removed

Tests
------

* ``env``
* ``toolchain``
* ``target``
* ``args``
* ``register``
* ``plugins``

env
^^^^

* test_env_util
* test_task_state
* test_command

toolchain
^^^^^^^^^^

* test_toolchain_verify

target
^^^^^^^

* test_path
* test_builder_interface
* test_target_config
* test_target_state
* test_generator
* test_compile_object
* test_base_target
* test_target_pch
* test_target_source
* test_target_source_out_of_root
* test_target_include_dir
* test_target_lib_dep
* test_target_external_lib
* test_target_flags
* test_target_user_deps
* test_target_lock
* test_target_sync
* test_target_failure_states

args
^^^^^

* test_args
* test_register
* test_persistent_storage

plugins
^^^^^^^^

.. note:: Incomplete implementation and tests
