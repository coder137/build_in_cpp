Setup
========

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
