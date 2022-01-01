Hybrid
=======

Real world tests combining multiple compilers

Single
-------

Compile a single source with **Register** and **Args** module

Simple 
-------

Similar to Flags example with **Register** and **Args** module 

Foolib
-------

For library developers 

External Lib
-------------

For end users consuming third party libraries 

Custom Target
----------------

For super customized targets and toolchains 

Generic
--------

Select library type and target-toolchain type at runtime

PCH
-----

Precompile header usage with GCC and MSVC compilers 

Dependency Chaining
---------------------

Chain `Generators` and `Targets` using the `Register` module 

Target Info
-------------

* Target Info usage to store Target specific information
* Example usage for Header Only targets, however it can store information for all Target inputs
* Common information used between multiple targets can be stored into a `TargetInfo` instance
