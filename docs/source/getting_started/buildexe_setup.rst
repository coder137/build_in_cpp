BuildExe Setup
==============

ENV[BUILDCC_HOME]
-----------------

* Add the environment variable ``BUILDCC_HOME`` with the absolute path on the operating system. For example: ``BUILDCC_HOME=C:\buildcc`` or ``BUILDCC_HOME=/local/mnt/buildcc``
* Create directories **bin**, **extensions**, **libs** and **host** inside your ENV[BUILDCC_HOME] directory
   * Download **BuildExe_Win.zip** or **BuildExe_Linux.zip** and unzip the bin file contents into the **bin** folder
   * **extensions** and **libs** folder will be empty for the time being
* Update your ``PATH`` variable with the **bin** folder. 
   * For example: ``PATH += ENV[BUILDCC_HOME]/bin;ENV[PATH]``
   * Linux : ``export PATH="$BUILDCC_HOME/bin:$PATH"``
   * Windows : My Computer -> Properties -> Advanced System Settings -> Environment Variables -> [Update your system variables]

* Git clone the buildcc project in the ENV[BUILDCC_HOME] directory.

.. code-block:: bash

    git clone https://github.com/coder137/build_in_cpp.git buildcc
    git submodule init
    git submodule update

* To update just do the following

.. code-block:: bash

    git pull
    git submodule init
    git submodule update

* Your ENV[BUILDCC_HOME] directory should look like the following

.. uml::

    @startmindmap
    * ENV[BUILDCC_HOME]
    ** bin
    *** flatc
    *** buildexe
    ** buildcc
    *** [git cloned]
    ** extensions
    *** [empty]
    ** libs
    *** [empty]
    ** host
    *** [empty]
    @endmindmap

Host Toolchains
------------------

From the above map we can see that the **host** folder is empty

This folder will contain the .toml files of all the HOST toolchains present on your system.

.. code-block:: toml
    :linenos:

    [toolchain.host]
    # Toolchain family id
    # valid options are: gcc, msvc, mingw
    # clang is not yet supported
    id = "gcc" 
    
    # NOTE: Each name MUST be unique
    # Preferrably use the [id]_[target_arch]_[compiler_version] for uniqueness,
    # but make sure the name is unique if you have multiple similar host toolchains installed on your system
    name = "gcc_x86_64-linux-gnu_9.3.0"

    # Make sure all of these executables are present when you install your toolchain
    asm_compiler = "as"
    c_compiler = "gcc"
    cpp_compiler = "g++"
    archiver = "ar"
    linker = "ld"

    # Additional information
    # Necessary if multiple similar toolchains are installed
    # For example. Installed gcc version 9.3.0 and 10.2.0
    path = "/usr/bin"
    compiler_version = "9.3.0"
    target_arch = "x86_64-linux-gnu"

.. important:: For **Windows**, make sure to use ``vcvarsall.bat [flavour]`` to activate your toolchain

* Your ENV[BUILDCC_HOME] directory should look like the following

.. uml::

    @startmindmap
    * ENV[BUILDCC_HOME]
    ** bin
    *** flatc
    *** buildexe
    ** buildcc
    *** [git cloned]
    ** extensions
    *** [empty]
    ** libs
    *** [empty]
    ** host
    *** gcc_x86_64-linux-gnu_9.3.0.toml
    *** mingw_x86_64-w64-mingw32_10.2.0.toml
    *** msvc_am64_19.29.30137.toml
    @endmindmap

Libs
-------

* Any library that the user needs to use in their projects can be **git cloned** into the libs folder.
* BuildExe can then be used to compile the library build files along with the "script". 
* The library build files define how a particular library is built. 
* These functions can be used in the "script" to define how your target should be built.

In this way we achieve a package manager like functionality with BuildExe and git. (Local package manager)

For example we download the **fmt library** to our libs folder

.. code-block:: bash

    cd $BUILDCC_HOME/libs
    git clone https://github.com/fmtlib/fmt.git

.. important:: This might sound strange, but the git cloned library **folder name** must also be a valid C++ **variable name**.

* Since the **fmt** lib does not have support for BuildCC style build files we write our own
* Please see the :doc:`buildexe_package_manager` document for a simple example.

.. uml::

    @startmindmap
    * ENV[BUILDCC_HOME]
    ** bin
    *** flatc
    *** buildexe
    ** buildcc
    *** [git cloned]
    ** extensions
    *** [empty]
    ** libs
    *** fmt
    **** [git cloned]
    *** spdlog
    **** [git cloned]
    *** json
    **** [git cloned]
    ** host
    *** gcc_x86_64-linux-gnu_9.3.0.toml
    *** mingw_x86_64-w64-mingw32_10.2.0.toml
    *** msvc_am64_19.29.30137.toml
    @endmindmap

Extensions
-------------

.. note:: Extensions support in BuildExe is incomplete and there currently aren't any third party extensions for BuildCC.
