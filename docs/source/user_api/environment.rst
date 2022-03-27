Environment
===========


env.h
-----

.. doxygenclass:: buildcc::Project

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

.. doxygenclass:: buildcc::env::Command

host_compiler.h
----------------

.. doxygenfunction:: is_gcc

.. doxygenfunction:: is_mingw

.. doxygenfunction:: is_clang

.. doxygenfunction:: is_msvc

host_os.h
----------

.. doxygenfunction:: is_linux

.. doxygenfunction:: is_win

.. doxygenfunction:: is_mac

.. doxygenfunction:: is_unix

.. doxygenenum:: OsId

.. doxygenfunction:: get_host_os

host_os_util.h
----------------

.. doxygenfunction:: get_os_envvar_delim

.. doxygenfunction:: get_os_executable_extension

task_state.h
--------------

.. doxygenenum:: TaskState

.. doxygenfunction:: get_task_state

util.h
---------

.. doxygenfunction:: save_file(const char *, const std::string &, bool)

.. doxygenfunction:: load_file
