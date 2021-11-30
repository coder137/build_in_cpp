#include "constants.h"

#include "env/logging.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestFailureStates)
{
    void teardown() {
        mock().checkExpectations();
        mock().clear();
        buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
    }
};
// clang-format on

buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                             "gcc", "g++", "ar", "ld");

TEST(TargetTestFailureStates, StartTaskEnvFailure) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "StartTaskEnvFailure.exe";
  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");

  simple.AddSource("dummy_main.cpp");
  simple.Build();
  buildcc::base::m::TargetRunner(simple);

  CHECK(simple.GetTaskState() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, CompilePchFailure) {
  constexpr const char *const NAME = "CompilePchFailure.exe";
  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");

  simple.AddSource("dummy_main.cpp");
  simple.AddPch("include/include_header.h");
  simple.Build();

  buildcc::m::CommandExpect_Execute(1, false); // PCH compile
  buildcc::base::m::TargetRunner(simple);

  CHECK(simple.GetTaskState() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, CompileObjectFailure) {
  constexpr const char *const NAME = "CompileObjectFailure.exe";

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");

  simple.AddSource("dummy_main.cpp");
  simple.AddSource("dummy_main.c");
  simple.Build();

  buildcc::m::CommandExpect_Execute(1, false); // compile
  buildcc::m::CommandExpect_Execute(1, true);  // compile
  buildcc::base::m::TargetRunner(simple);

  CHECK(simple.GetTaskState() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, LinkTargetFailure) {
  constexpr const char *const NAME = "LinkTargetFailure.exe";

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");

  simple.AddSource("dummy_main.cpp");
  simple.Build();

  buildcc::m::CommandExpect_Execute(1, true);  // compile
  buildcc::m::CommandExpect_Execute(1, false); // link
  buildcc::base::m::TargetRunner(simple);

  CHECK(simple.GetTaskState() == buildcc::env::TaskState::FAILURE);
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_FAILURE_STATES_BUILD_DIR);
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
