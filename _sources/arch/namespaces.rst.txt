Namespaces
==========

User
-----

* ``buildcc``
* ``buildcc::env``
* ``buildcc::plugin``

.. admonition:: User/Developer opinion
    
    Do we need to segregate the **Environment** into its own ``buildcc::env`` namespace or merge all those APIs into the ``buildcc`` namespace?

Developer
----------

* ``buildcc::base``
* ``buildcc::internal``

.. note:: Consider removing ``buildcc::base`` since a lot of the APIs are typedef`ed to the ``buildcc`` namespace
