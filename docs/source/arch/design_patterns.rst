Design Patterns
===============

CRTP / Mixins
--------------

`Article by fluentcpp.com on CRTP <https://www.fluentcpp.com/2017/05/16/what-the-crtp-brings-to-code/>`_

* Mixins are a design pattern used to add additional functionality to an existing class
* In this case, the ``base::TargetInfo`` and the ``base::Target`` class are meant to have a lot of setter APIs for user inputs.
* Adding more APIs to the class makes its difficult to read and maintain.
* For reference: See :doc:`serialization_schema`
* For example: In Target ``source_files`` have currently have several APIs
   * ``AddSource``
   * ``AddSourceAbsolute``
   * ``GlobSources``
   * ``GlobSourcesAbsolute``
* In the future we might have additional APIs such as
   * ``AddSources`` that takes an ``std::initializer_list``
   * ``AddSources`` that takes a ``std::vector<>`` or ``std::unordered_set<>``
   * ``AddSource(s)ByPattern`` that takes a fmt string like ``{dir}/source.cpp``
* From our serialization schema we can see that each of these fields could have many APIs associated with them. Having 50+ APIs for different fields in a single header i.e ``target_info.h`` / ``target.h`` would not be readible and hard to maintain.
* The Mixin / CRTP pattern is used to easily add functionality for a particular field in its own header / source file.

Friend classes
---------------
