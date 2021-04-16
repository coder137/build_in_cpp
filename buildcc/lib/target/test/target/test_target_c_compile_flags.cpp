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

// clang-format off
TEST_GROUP(TargetTestCCompileFlagsGroup)
{
};
// clang-format on

static fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_C_COMPILE_INTERMEDIATE_DIR);

TEST(TargetTestCCompileFlagsGroup, Target_AddCompileFlag) {
  constexpr const char *const NAME = "AddCCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.c";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(
      NAME, buildcc::base::TargetType::Executable,
      buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"), "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddCCompileFlag("-std=c11");
}

TEST(TargetTestCCompileFlagsGroup, Target_ChangedCompileFlag) {
  constexpr const char *const NAME = "ChangedCCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.c";

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
    simple.AddCCompileFlag("-std=c11");

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
    simple.AddCCompileFlag("-std=c11");
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_C_COMPILE_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
