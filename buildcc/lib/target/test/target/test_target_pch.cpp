#include <filesystem>

#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"
#include "test_target_util.h"

#include "target/target.h"

#include "env/env.h"
#include "env/util.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTest/UtestMacros.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetPchTestGroup)
{
    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

TEST(TargetPchTestGroup, Target_AddPch) {
  constexpr const char *const NAME = "AddPch.exe";
  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");
}

TEST(TargetPchTestGroup, Target_AddPch_Build) {
  constexpr const char *const NAME = "AddPch_Build.exe";
  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  target.Build();
  buildcc::m::TargetRunner(target);
  bool exists = fs::exists(target.GetPchHeaderPath());
  CHECK_TRUE(exists);

  // Save file
  buildcc::env::save_file(target.GetPchCompilePath().string().c_str(), "",
                          false);
  exists = fs::exists(target.GetPchHeaderPath());
  CHECK_TRUE(exists);

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPch_Rebuild) {
  constexpr const char *const NAME = "AddPch_Rebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: No change
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Removed
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_PathRemoved(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Added
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::m::TargetExpect_PathAdded(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Updated
  {
    buildcc::m::blocking_sleep(1);
    fs::path filename =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / "pch/pch_header_1.h";
    bool save = buildcc::env::save_file(filename.string().c_str(), "", false);
    CHECK_TRUE(save);

    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::m::TargetExpect_PathUpdated(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPch_CppRebuild) {
  constexpr const char *const NAME = "AddPch_CppRebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: No change
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Removed
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddSource("dummy_main.cpp");

    buildcc::m::TargetExpect_PathRemoved(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Added
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::m::TargetExpect_PathAdded(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Updated
  {
    buildcc::m::blocking_sleep(1);
    fs::path filename =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / "pch/pch_header_1.h";
    bool save = buildcc::env::save_file(filename.string().c_str(), "", false);
    CHECK_TRUE(save);

    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::m::TargetExpect_PathUpdated(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPch_IncludeDirsRebuild) {
  constexpr const char *const NAME = "AddPch_IncludeDirsRebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddIncludeDir("pch");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // No Change
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddIncludeDir("pch");

    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Remove
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    // target.AddIncludeDir("pch");

    buildcc::m::TargetExpect_DirChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddIncludeDir("pch");

    buildcc::m::TargetExpect_DirChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }
}

TEST(TargetPchTestGroup, Target_AddPch_HeadersRebuild) {
  constexpr const char *const NAME = "AddPch_HeadersRebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddHeader("pch/pch_header_1.h");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // No Change
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddHeader("pch/pch_header_1.h");

    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Remove
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    // target.AddHeader("pch/pch_header_1.h");

    buildcc::m::TargetExpect_PathChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddHeader("pch/pch_header_1.h");

    buildcc::m::TargetExpect_PathChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }
}

TEST(TargetPchTestGroup, Target_AddPchs_FlagsRebuild) {
  constexpr const char *const NAME = "Target_AddPchs_FlagsRebuild.exe";

  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPch("pch/pch_header_1.h");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added Preprocessor flag
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPreprocessorFlag("-H");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_FlagChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added CommonCompileFlag
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPreprocessorFlag("-H");
    target.AddCommonCompileFlag("-H");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_FlagChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added PchCompileFlag
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPreprocessorFlag("-H");
    target.AddCommonCompileFlag("-H");
    target.AddPchCompileFlag("-H");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_FlagChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Added CCompileFlag
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPreprocessorFlag("-H");
    target.AddCommonCompileFlag("-H");
    target.AddPchCompileFlag("-H");
    target.AddCCompileFlag("-H");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_FlagChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Keep CCompileFlag, Added CppCompileFlag
  {
    buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    target.AddPreprocessorFlag("-H");
    target.AddCommonCompileFlag("-H");
    target.AddPchCompileFlag("-H");
    target.AddCCompileFlag("-H");
    target.AddCppCompileFlag("-H");
    target.AddPch("pch/pch_header_1.h");

    buildcc::m::TargetExpect_FlagChanged(1, &target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    target.Build();
    buildcc::m::TargetRunner(target);
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }
}

TEST(TargetPchTestGroup, Target_AddPchObjectFlag_Build) {
  constexpr const char *const NAME = "AddPchObjectFlag_Build.exe";

  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  target.AddPchObjectFlag("-H");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  target.Build();
  buildcc::m::TargetRunner(target);
  bool exists = fs::exists(target.GetPchHeaderPath());
  CHECK_TRUE(exists);
  CHECK_EQUAL(target.GetPchObjectFlags().size(), 1);

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_BadPch) {
  constexpr const char *const NAME = "Target_BadPch.exe";
  buildcc::BaseTarget target(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");

  buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);

  target.Build();
  buildcc::m::TargetRunner(target);
  bool exists = fs::exists(target.GetPchHeaderPath());
  CHECK_FALSE(exists);

  buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);

  // Save file
  exists = fs::exists(target.GetPchHeaderPath());
  CHECK_FALSE(exists);

  mock().checkExpectations();
}

int main(int ac, char **av) {
  const fs::path target_source_intermediate_path =
      fs::path(BUILD_TARGET_PCH_INTERMEDIATE_DIR) / gcc.GetName();
  fs::remove_all(target_source_intermediate_path);

  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_PCH_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
