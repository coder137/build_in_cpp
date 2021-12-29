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

.. doxygenclass:: buildcc::env::Command
   :members: Execute

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

toolchain
^^^^^^^^^^

target
^^^^^^^

args
^^^^^

register
^^^^^^^^^^

plugins
^^^^^^^^
