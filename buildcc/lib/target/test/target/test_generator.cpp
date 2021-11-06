#include "target/generator.h"

#include <unistd.h>

#include "expect_command.h"
#include "expect_generator.h"

#include "taskflow/taskflow.hpp"

#include "env/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(GeneratorTestGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "generator";

TEST(GeneratorTestGroup, Generator_Build) {
  constexpr const char *const NAME = "Build";
  buildcc::base::Generator generator(NAME, "");

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c",
                       {
                           {"compiler", "gcc"},
                       });

  buildcc::m::CommandExpect_Execute(1, true);
  generator.Build();

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild) {
  constexpr const char *const NAME = "Rebuild";
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Inputs) {
  constexpr const char *const NAME = "Rebuild_Inputs";

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  // Removed
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddOutput("{gen_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::base::m::GeneratorExpect_InputRemoved(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  // Added
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.cpp.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::base::m::GeneratorExpect_InputAdded(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  sleep(1);
  bool saved = buildcc::env::SaveFile(
      (buildcc::env::get_project_root_dir() / "new_source.cpp")
          .string()
          .c_str(),
      "", false);
  CHECK_TRUE(saved);

  // Updated
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.cpp.exe "
                         "{gen_root_dir}/new_source.cpp");
    buildcc::base::m::GeneratorExpect_InputUpdated(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Outputs) {
  constexpr const char *const NAME = "Rebuild_Outputs";
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::base::m::GeneratorExpect_OutputChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::base::m::GeneratorExpect_OutputChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Commands) {
  constexpr const char *const NAME = "Rebuild_Commands";
  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} {gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::base::m::GeneratorExpect_CommandChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    buildcc::base::m::GeneratorExpect_CommandChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::env::init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
