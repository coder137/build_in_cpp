Template Generator
===================

> TODO,

File Generator
===============

> TODO

Custom Generator
================

custom_generator.h
-------------------

.. doxygenclass:: buildcc::CustomGenerator

Example
--------

> TODO, Update example

.. code-block:: cpp
    :linenos:

    // Example Setup
    TargetEnv gen_env("gen_root", "gen_build");
    bool parallel{true};
    BaseGenerator generator("unique_name", gen_env, parallel);
    // Adds absolute {gen_root_dir} and {gen_build_dir} paths to internal fmt::format

    // We can now do this
    // {gen_root_dir}/my_generator.py -> {my_generator}
    generator.AddInput("{gen_root_dir}/my_generator.py", "my_generator");

    // {gen_build_dir}/my_generator.h -> {generated_header}
    // {gen_build_dir}/my_generator.cpp -> {generated_source}
    generator.AddOutput("{gen_build_dir}/generated.h", "generated_header");
    generator.AddOutput("{gen_build_dir}/generated.cpp", "generated_source");

    // Example Commands
    // NOTE: If `parallel==true`, both of these commands runs in parallel
    // NOTE: If `parallel==false`, commands will run in the order that they are added.
    generator.AddCommand("{my_generator} --generate header {generated_header}");
    generator.AddCommand("{my_generator} --generate source {generated_source}");

    // Setup the tasks
    generator.Build();

    // Run the generator
    tf::Executor executor;
    executor.run(generator.GetTaskflow());
    executor.wait_for_all();
