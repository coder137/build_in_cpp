CMake Boilerplate
=================

.. code-block:: cmake

    if (${TESTING})
    # setup mocking
    # setup testing executables

    # TODO, Add example
    endif()

    if(${BUILDCC_BUILD_AS_SINGLE_LIB})
    # buildcc files as an aggregate to one CMake library
    # third party libraries still remain seperate so do NOT add it here
    # Add third party library dependency to `buildcc` library in `buildcc/CMakeLists.txt` 

    # TODO, Add example
    endif()

    if(${BUILDCC_BUILD_AS_INTERFACE})
    # one buildcc library broken up into smaller library chunks instead of aggregated to one CMake library like in BUILDCC_BUILD_AS_SINGLE_LIB
    # NOTE: Do not forget to add this small library chunk to `buildcc_i` library in `buildcc/CMakeLists.txt`

    # TODO, Add example
    endif()

    if (${BUILDCC_INSTALL})
    # Install behaviour when option selected

    # TODO, Add example
    endif()
