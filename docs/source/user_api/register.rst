Register
=========

register.h
-----------

.. doxygenclass:: buildcc::Reg

test_info.h
-------------

.. doxygenstruct:: buildcc::TestConfig

.. doxygenstruct:: buildcc::TestOutput

Example
--------

.. code-block:: cpp
    :linenos:

    class BigObj {};

    static void callback_usage_func(const BigObj & cobj, BigObj & obj);

    int main(int argc, char ** argv) {
        Args::Init()
            .Parse(argc, argv);

        Reg::Init();
        Reg::Call(Args::Clean()).Func([](){
            fs::remove_all(Project::GetBuildDir());
        })

        
        BigObj obj;
        Reg::Call().Func(callback_usage_func, BigObj(), obj)

        bool expression = true; // false
        Reg::Call(expression).Func(callback_usage_func, BigObj(), obj)

        // Example snippets of these given in Target API
        // Build
        // Dep
        // Test
        // RunBuild
        // RunTest
        return 0;
    }
