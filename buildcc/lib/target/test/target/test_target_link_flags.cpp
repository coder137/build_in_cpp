#include "constants.h"

#include "expect_target.h"
#include "target.h"

#include "env.h"

// Third Party
#include "flatbuffers/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestLinkFlagsGroup)
{
};
// clang-format on

static fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_LINK_INTERMEDIATE_DIR);

TEST(TargetTestLinkFlagsGroup, Target_AddLinkFlag) {
  constexpr const char *const NAME = "AddLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(
      NAME, buildcc::base::TargetType::Executable,
      buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"), "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddLinkFlag("-lm");
}

TEST(TargetTestLinkFlagsGroup, Target_ChangedLinkFlag) {
  constexpr const char *const NAME = "ChangedLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::base::Target simple(
        NAME, buildcc::base::TargetType::Executable,
        buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }
  {
    // * Remove flag
    buildcc::base::Target simple(
        NAME, buildcc::base::TargetType::Executable,
        buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }

  {
    // * Add flag
    buildcc::base::Target simple(
        NAME, buildcc::base::TargetType::Executable,
        buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_LINK_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
