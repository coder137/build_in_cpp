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

* Friend classes are used when 2 classes have strong coupling with each other, but still need to maintain flexibility for other purposes. For example: Unit testing.
* In ``target.h`` we have 3 friend classes (non CRTP based)
   * ``CompilePch``
   * ``CompileObject``
   * ``LinkTarget``
* These 3 classes are made friend classes for 2 main purposes
   * Unit Testing
   * Flexibility / Maintaibility
* Unit Testing
   * If these were not friend classes, the functions would've been private in scope within the ``base::Target`` class
   * Unit testing these individual private functions would not be possible would public interfaces
   * By making them friend classes, We can now unit test the public functions and embed this class in a private context with the ``base::Target`` class
* Flexibility / Maintaibility
   * Each one of the classes mentioned above have their own information / states / tasks to hold.
   * Without this segregation all of the member variables, states and tasks would need to be present inside the ``base::Target`` class
* Strong Coupling
   * The 3 friend classes have strong coupling with the ``base::Target`` class since it uses its internal member variables for setting / getting information.
   * The friend class can interact with the parent class and vice versa.
