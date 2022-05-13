#include "target/generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"
#include "expect_generator.h"
#include "test_target_util.h"

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
      buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
      mock().clear();
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "generator";

TEST(GeneratorTestGroup, Generator_Build) {
  constexpr const char *const NAME = "Build";
  buildcc::Generator generator(NAME, "");

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  mock().checkExpectations();
}

// TODO, Remove Parallel build
TEST(GeneratorTestGroup, Generator_BuildParallel) {
  constexpr const char *const NAME = "BuildParallel";
  buildcc::Generator generator(NAME, "", true);

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Identifier) {
  constexpr const char *const NAME = "Identifier";
  buildcc::Generator generator(NAME, "");

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c", "dummy_main_c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe", "dummy_main_exe");
  generator.AddCommand("{compiler} -o {dummy_main_exe} {dummy_main_c}");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild) {
  constexpr const char *const NAME = "Rebuild";
  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Inputs) {
  constexpr const char *const NAME = "Rebuild_Inputs";

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }

  // Removed
  {
    buildcc::Generator generator(NAME, "");
    generator.AddOutput("{gen_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }

  // Added
  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.cpp.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  buildcc::m::blocking_sleep(1);
  bool saved = buildcc::env::save_file(
      (buildcc::Project::GetRootDir() / "new_source.cpp").string().c_str(), "",
      false);
  CHECK_TRUE(saved);

  // Updated
  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/new_source.cpp");
    generator.AddOutput("{gen_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/new_source.cpp.exe "
                         "{gen_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Outputs) {
  constexpr const char *const NAME = "Rebuild_Outputs";
  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

// TODO, Add implementation for this
IGNORE_TEST(GeneratorTestGroup, Generator_Rebuild_Commands) {
  constexpr const char *const NAME = "Rebuild_Commands";
  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} {gen_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    // buildcc::m::GeneratorExpect_CommandChanged(1, &generator);
    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  {
    buildcc::Generator generator(NAME, "");
    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("gcc -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    // buildcc::m::GeneratorExpect_CommandChanged(1, &generator);
    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_AddDefaultArguments) {
  constexpr const char *const NAME = "AddDefaultArgument";
  buildcc::Generator generator(NAME, "");

  generator.AddDefaultArguments({
      {"key", "value"},
  });
  const std::string &value = generator.GetValueByIdentifier("key");
  STRCMP_EQUAL(value.c_str(), "value");
  STRCMP_EQUAL(generator.GetName().c_str(), "AddDefaultArgument");
}

// FAILURE STATES

TEST(GeneratorTestGroup, Generator_FailedEnvTaskState) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "FailedEnvTaskState";
  buildcc::Generator generator(NAME, "", true);

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(GeneratorTestGroup, Generator_FailedGenerateConvert) {
  constexpr const char *const NAME = "FailedGenerateConvert";
  buildcc::Generator generator(NAME, "", false);

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/this_file_does_not_exist.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(GeneratorTestGroup, Generator_FailedGenerateCommand) {
  constexpr const char *const NAME = "FailedGenerateCommand";
  buildcc::Generator generator(NAME, "", false);

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, false);
  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(GeneratorTestGroup, Generator_FailedStore) {
  constexpr const char *const NAME = "FailedStore";
  const fs::path test_build_dir = buildcc::Project::GetBuildDir() / NAME;

  buildcc::Generator generator(NAME, "", false);
  fs::remove_all(test_build_dir);

  generator.AddDefaultArguments({
      {"compiler", "gcc"},
  });

  generator.AddInput("{gen_root_dir}/dummy_main.c");
  generator.AddOutput("{gen_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                       "{gen_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::GeneratorRunner(generator);

  // CHECK(generator.GetTaskState() == buildcc::env::TaskState::FAILURE);
  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(GeneratorTestGroup, FailedEnvTaskState_Rebuild) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "FailedEnvTaskState_Rebuild";
  {
    buildcc::Generator generator(NAME, "", true);

    generator.AddDefaultArguments({
        {"compiler", "gcc"},
    });

    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  // reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // rebuild
  {
    buildcc::Generator generator(NAME, "", true);

    generator.AddDefaultArguments({
        {"compiler", "gcc"},
    });

    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    generator.Build();
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, FailedGenerateCommand_Rebuild) {
  constexpr const char *const NAME = "FailedGenerateCommand_Rebuild";

  {
    buildcc::Generator generator(NAME, "", false);

    generator.AddDefaultArguments({
        {"compiler", "gcc"},
    });

    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    buildcc::env::m::CommandExpect_Execute(1, false);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  // reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // rebuild
  {
    buildcc::Generator generator(NAME, "", false);

    generator.AddDefaultArguments({
        {"compiler", "gcc"},
    });

    generator.AddInput("{gen_root_dir}/dummy_main.c");
    generator.AddOutput("{gen_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {gen_build_dir}/dummy_main.exe "
                         "{gen_root_dir}/dummy_main.c");

    buildcc::m::CustomGeneratorExpect_IdAdded(1, &generator);
    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::GeneratorRunner(generator);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
