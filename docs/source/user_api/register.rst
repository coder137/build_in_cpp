Register
=========

register.h
-----------

.. doxygenclass:: buildcc::Register
    :members: Register, Clean, Callback, CallbackIf, Build, Dep, Test, RunBuild, RunTest


Example
--------

.. code-block:: cpp
    :linenos:

    class BigObj {};

    static void callback_usage_func(const BigObj & cobj, BigObj & obj);

    int main(int argc, char ** argv) {
        Args args;
        args.Parse(argc, argv);

        Register reg(args);
        reg.Clean([](){
            fs::remove_all(env::get_project_build_dir());
        });
        
        BigObj obj;
        reg.Callback(callback_usage_func, BigObj(), obj);

        bool expression = true; // false
        reg.CallbackIf(expression, callback_usage_func, BigObj(), obj);

        // Example snippets of these given in Target API
        // Build
        // Dep
        // Test
        // RunBuild
        // RunTest
        return 0;
    }
