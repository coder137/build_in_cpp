Environment
===========


env.h
-----

.. doxygenfunction:: is_init

.. doxygenfunction:: get_project_root_dir

.. doxygenfunction:: get_project_build_dir

logging.h
---------

.. doxygenfunction:: log_trace

.. doxygenfunction:: log_debug

.. doxygenfunction:: log_info

.. doxygenfunction:: log_warning

.. doxygenfunction:: log_critical

assert_fatal.h
--------------

.. doxygenfunction:: assert_fatal([[maybe_unused]] const char *)

.. doxygenfunction:: assert_fatal(const std::string &)

.. doxygenfunction:: assert_fatal(bool, const char *)

.. doxygenfunction:: assert_fatal(bool, const std::string &)

.. doxygendefine:: ASSERT_FATAL

assert_throw.h
--------------

.. doxygenfunction:: assert_throw([[maybe_unused]] const char *)

.. doxygenfunction:: assert_throw(const std::string &)

.. doxygenfunction:: assert_throw(bool, const char *)

.. doxygenfunction:: assert_throw(bool, const std::string &)

.. doxygendefine:: ASSERT_THROW

command.h
---------
