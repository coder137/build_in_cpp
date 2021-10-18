#include <filesystem>

#include <unistd.h>

#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"

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
        mock().clear();
    }
};
// clang-format on

static const buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc,
                                          "gcc", "as", "gcc", "g++", "ar",
                                          "ld");

TEST(TargetPchTestGroup, Target_AddPch) {
  constexpr const char *const NAME = "AddPch.exe";
  buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");
}

TEST(TargetPchTestGroup, Target_AddPch_Build) {
  constexpr const char *const NAME = "AddPch_Build.exe";
  buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  target.Build();
  bool exists = fs::exists(target.GetPchHeaderPath());
  CHECK_TRUE(exists);

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPch_Rebuild) {
  constexpr const char *const NAME = "AddPch_Rebuild.exe";

  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: No change
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Removed
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");

    buildcc::base::m::TargetExpect_PathRemoved(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Added
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::base::m::TargetExpect_PathAdded(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Updated
  {
    sleep(1);
    fs::path filename =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / "pch/pch_header_1.h";
    bool save = buildcc::env::SaveFile(filename.string().c_str(), "", false);
    CHECK_TRUE(save);

    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");

    buildcc::base::m::TargetExpect_PathUpdated(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPch_CppRebuild) {
  constexpr const char *const NAME = "AddPch_CppRebuild.exe";

  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: No change
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Removed
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddSource("dummy_main.cpp");

    buildcc::base::m::TargetExpect_PathRemoved(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Added
  {
    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::base::m::TargetExpect_PathAdded(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  // Rebuild: Updated
  {
    sleep(1);
    fs::path filename =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / "pch/pch_header_1.h";
    bool save = buildcc::env::SaveFile(filename.string().c_str(), "", false);
    CHECK_TRUE(save);

    buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    target.AddPch("pch/pch_header_1.h");
    target.AddPch("pch/pch_header_2.h");
    target.AddSource("dummy_main.cpp");

    buildcc::base::m::TargetExpect_PathUpdated(1, &target);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    target.Build();
    bool exists = fs::exists(target.GetPchHeaderPath());
    CHECK_TRUE(exists);
  }

  mock().checkExpectations();
}

TEST(TargetPchTestGroup, Target_AddPchFlag_Build) {
  constexpr const char *const NAME = "AddPchFlag_Build.exe";

  buildcc::base::Target target(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");
  target.AddPchFlag("-H");
  target.AddPch("pch/pch_header_1.h");
  target.AddPch("pch/pch_header_2.h");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  target.Build();
  bool exists = fs::exists(target.GetPchHeaderPath());
  CHECK_TRUE(exists);
  CHECK_EQUAL(target.GetCurrentPchFlags().size(), 1);

  mock().checkExpectations();
}

int main(int ac, char **av) {
  const fs::path target_source_intermediate_path =
      fs::path(BUILD_TARGET_PCH_INTERMEDIATE_DIR) / gcc.GetName();
  fs::remove_all(target_source_intermediate_path);

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_PCH_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
