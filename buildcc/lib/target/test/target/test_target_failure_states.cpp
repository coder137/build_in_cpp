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

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

TEST(TargetTestFailureStates, StartTaskEnvFailure) {
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  constexpr const char *const NAME = "StartTaskEnvFailure.exe";
  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("dummy_main.cpp");
  target.Build();
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, CompilePchFailure) {
  constexpr const char *const NAME = "CompilePchFailure.exe";
  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("dummy_main.cpp");
  target.AddPch("include/include_header.h");
  target.Build();

  buildcc::env::m::CommandExpect_Execute(1, false); // PCH compile
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, CompileObjectFailure) {
  constexpr const char *const NAME = "CompileObjectFailure.exe";

  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("dummy_main.cpp");
  target.AddSource("dummy_main.c");
  target.Build();

  buildcc::env::m::CommandExpect_Execute(1, false); // compile
  buildcc::env::m::CommandExpect_Execute(1, true);  // compile
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, CompileObject_FileNotFoundFailure) {
  constexpr const char *const NAME = "CompileObject_FileNotFoundFailure.exe";

  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("file_not_present.cpp");
  target.Build();
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, LinkTargetFailure) {
  constexpr const char *const NAME = "LinkTargetFailure.exe";

  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("dummy_main.cpp");
  target.Build();

  buildcc::env::m::CommandExpect_Execute(1, true);  // compile
  buildcc::env::m::CommandExpect_Execute(1, false); // link
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

TEST(TargetTestFailureStates, EndTaskStoreFailure) {
  constexpr const char *const NAME = "EndTaskStoreFailure.exe";

  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  target.AddSource("dummy_main.cpp");
  target.Build();
  fs::remove_all(
      target.GetTargetBuildDir()); // removing this path causes store failure

  buildcc::env::m::CommandExpect_Execute(1, true); // compile
  buildcc::env::m::CommandExpect_Execute(1, true); // link
  buildcc::m::TargetRunner(target);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
}

// TODO, Test failure rebuilds!
// Every failure state during rebuild should re-run!

TEST(TargetTestFailureStates, StartTaskEnvFailure_Rebuild) {
  // env state is failure so target fails
  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);
  constexpr const char *const NAME = "StartTaskEnvFailure_Rebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.Build();
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
    CHECK_FALSE(target.IsBuilt());
  }

  // Reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // during rebuild, this target must run!
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.Build();

    buildcc::env::m::CommandExpect_Execute(1, true); // compile
    buildcc::env::m::CommandExpect_Execute(1, true); // link
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
    CHECK_TRUE(target.IsBuilt());
  }
}

TEST(TargetTestFailureStates, CompilePchFailure_Rebuild) {
  // Pch fails to compile during first run
  constexpr const char *const NAME = "CompilePchFailure_Rebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.AddPch("include/include_header.h");
    target.Build();

    buildcc::env::m::CommandExpect_Execute(1, false); // PCH compile
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
  }

  // Reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // during rebuild, this must run!
  // must move to compile object stage
  // must move to link target stage
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.AddPch("include/include_header.h");
    target.Build();

    buildcc::m::TargetExpect_PathAdded(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true); // PCH compile
    buildcc::env::m::CommandExpect_Execute(1, true); // Object compile
    buildcc::env::m::CommandExpect_Execute(1, true); // Link target

    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }
}

TEST(TargetTestFailureStates, CompileObjectFailure_Rebuild) {
  // Compile object fails during first run

  constexpr const char *const NAME = "CompileObjectFailure_Rebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.AddSource("dummy_main.c");
    target.Build();

    buildcc::env::m::CommandExpect_Execute(1, false); // compile
    buildcc::env::m::CommandExpect_Execute(1, true);  // compile
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
  }

  // Reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // during rebuild this must run the ones that were NOT build before
  // for example
  // a.cpp -> BUILT
  // b.cpp -> FAILS

  // rerun
  // a.cpp -> no need to compile again!
  // b.cpp -> REBUILD

  // must move to link target stage
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.AddSource("dummy_main.c");
    target.Build();

    // NOTE, The other one does not compile since it already compiled
    // successfully earlier!
    buildcc::env::m::CommandExpect_Execute(1, true); // compile
    buildcc::m::TargetExpect_SourceAdded(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true); // link
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }
}

TEST(TargetTestFailureStates, LinkTargetFailure_Rebuild) {
  // Link target fails during first run
  constexpr const char *const NAME = "LinkTargetFailure_Rebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.Build();

    buildcc::env::m::CommandExpect_Execute(1, true);  // compile
    buildcc::env::m::CommandExpect_Execute(1, false); // link
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
  }

  // Reset
  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // during rebuild this must try to relink!
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    target.AddSource("dummy_main.cpp");
    target.Build();

    // we do not recompile
    buildcc::env::m::CommandExpect_Execute(1, true); // link
    buildcc::m::TargetRunner(target);

    CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
  }
}

int main(int ac, char **av) {
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_FAILURE_STATES_BUILD_DIR);
  fs::remove_all(buildcc::Project::GetBuildDir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
