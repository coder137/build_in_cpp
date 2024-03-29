#include "target/file_generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"
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
TEST_GROUP(FileGeneratorTestGroup)
{
    void teardown() {
      buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
      mock().clear();
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "file_generator";

TEST(FileGeneratorTestGroup, Generator_Build) {
  constexpr const char *const NAME = "Build";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"compiler", "gcc"},
  });

  generator.AddInput("{current_root_dir}/dummy_main.c");
  generator.AddOutput("{current_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                       "{current_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_Identifier) {
  constexpr const char *const NAME = "Identifier";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"compiler", "gcc"},
  });
  generator.AddPattern("dummy_main_c", "{current_root_dir}/dummy_main.c");
  generator.AddPattern("dummy_main_exe", "{current_build_dir}/dummy_main.exe");

  generator.AddInput("{dummy_main_c}");
  generator.AddOutput("{dummy_main_exe}");
  generator.AddCommand("{compiler} -o {dummy_main_exe} {dummy_main_c}");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_Rebuild) {
  constexpr const char *const NAME = "Rebuild";
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_Rebuild_Inputs) {
  constexpr const char *const NAME = "Rebuild_Inputs";

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/new_source.cpp");
    generator.AddOutput("{current_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {current_build_dir}/new_source.exe "
                         "{current_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }

  // Removed
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddOutput("{current_build_dir}/new_source.exe");
    generator.AddCommand("gcc -o {current_build_dir}/new_source.exe "
                         "{current_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }

  // Added
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/new_source.cpp");
    generator.AddOutput("{current_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {current_build_dir}/new_source.cpp.exe "
                         "{current_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  buildcc::m::blocking_sleep(1);
  bool saved = buildcc::env::save_file(
      (buildcc::Project::GetRootDir() / "new_source.cpp").string().c_str(), "",
      false);
  CHECK_TRUE(saved);

  // Updated
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/new_source.cpp");
    generator.AddOutput("{current_build_dir}/new_source.cpp.exe");
    generator.AddCommand("gcc -o {current_build_dir}/new_source.cpp.exe "
                         "{current_root_dir}/new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_Rebuild_Outputs) {
  constexpr const char *const NAME = "Rebuild_Outputs";
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_Rebuild_Commands) {
  constexpr const char *const NAME = "Rebuild_Commands";
  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} {current_root_dir}/dummy_main.c",
                         {
                             {"compiler", "gcc"},
                         });

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  {
    buildcc::FileGenerator generator(NAME, "");
    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("gcc -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c");

    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, Generator_AddDefaultArguments) {
  constexpr const char *const NAME = "AddDefaultArgument";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"key", "value"},
  });
  const std::string &value = generator.Get("key");
  STRCMP_EQUAL(value.c_str(), "value");
  STRCMP_EQUAL(generator.GetName().c_str(), "AddDefaultArgument");
}

// FAILURE STATES

TEST(FileGeneratorTestGroup, Generator_FailedEnvTaskState) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "FailedEnvTaskState";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"compiler", "gcc"},
  });

  generator.AddInput("{current_root_dir}/dummy_main.c");
  generator.AddOutput("{current_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                       "{current_root_dir}/dummy_main.c");

  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(FileGeneratorTestGroup, Generator_FailedGenerateConvert) {
  constexpr const char *const NAME = "FailedGenerateConvert";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"compiler", "gcc"},
  });

  generator.AddInput("{current_root_dir}/this_file_does_not_exist.c");
  generator.AddOutput("{current_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                       "{current_root_dir}/dummy_main.c");

  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(FileGeneratorTestGroup, Generator_FailedGenerateCommand) {
  constexpr const char *const NAME = "FailedGenerateCommand";
  buildcc::FileGenerator generator(NAME, "");

  generator.AddPatterns({
      {"compiler", "gcc"},
  });

  generator.AddInput("{current_root_dir}/dummy_main.c");
  generator.AddOutput("{current_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                       "{current_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, false);
  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(FileGeneratorTestGroup, Generator_FailedStore) {
  constexpr const char *const NAME = "FailedStore";
  const fs::path test_build_dir = buildcc::Project::GetBuildDir() / NAME;

  buildcc::FileGenerator generator(NAME, "");
  fs::remove_all(test_build_dir);

  generator.AddPatterns({
      {"compiler", "gcc"},
  });

  generator.AddInput("{current_root_dir}/dummy_main.c");
  generator.AddOutput("{current_build_dir}/dummy_main.exe");
  generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                       "{current_root_dir}/dummy_main.c");

  buildcc::env::m::CommandExpect_Execute(1, true);
  generator.Build();
  buildcc::m::CustomGeneratorRunner(generator);

  // CHECK(generator.GetTaskState() == buildcc::env::TaskState::FAILURE);
  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);

  mock().checkExpectations();

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
}

TEST(FileGeneratorTestGroup, FailedEnvTaskState_Rebuild) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "FailedEnvTaskState_Rebuild";
  {
    buildcc::FileGenerator generator(NAME, "");

    generator.AddPatterns({
        {"compiler", "gcc"},
    });

    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c");

    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  // reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // rebuild
  {
    buildcc::FileGenerator generator(NAME, "");

    generator.AddPatterns({
        {"compiler", "gcc"},
    });

    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c");

    generator.Build();
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

TEST(FileGeneratorTestGroup, FailedGenerateCommand_Rebuild) {
  constexpr const char *const NAME = "FailedGenerateCommand_Rebuild";

  {
    buildcc::FileGenerator generator(NAME, "");

    generator.AddPatterns({
        {"compiler", "gcc"},
    });

    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c");

    buildcc::env::m::CommandExpect_Execute(1, false);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  // reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // rebuild
  {
    buildcc::FileGenerator generator(NAME, "");

    generator.AddPatterns({
        {"compiler", "gcc"},
    });

    generator.AddInput("{current_root_dir}/dummy_main.c");
    generator.AddOutput("{current_build_dir}/dummy_main.exe");
    generator.AddCommand("{compiler} -o {current_build_dir}/dummy_main.exe "
                         "{current_root_dir}/dummy_main.c");

    buildcc::m::CustomGeneratorExpect_IdAdded(1, &generator);
    buildcc::env::m::CommandExpect_Execute(1, true);
    generator.Build();
    buildcc::m::CustomGeneratorRunner(generator);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
