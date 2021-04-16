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
TEST_GROUP(TargetTestPreprocessorFlagGroup)
{
};
// clang-format on

static fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_PREPROCESSOR_INTERMEDIATE_DIR);

TEST(TargetTestPreprocessorFlagGroup, Target_AddPreprocessorFlag) {
  constexpr const char *const NAME = "AddSource.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(
      NAME, buildcc::base::TargetType::Executable,
      buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"), "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddPreprocessorFlag("-DCOMPILE=1");
}

TEST(TargetTestPreprocessorFlagGroup, Target_ChangedPreprocessorFlag) {
  constexpr const char *const NAME = "AddSource.exe";
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
    simple.AddPreprocessorFlag("-DCOMPILE=1");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }
  {
    // * Remove preprocessor flag
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
    // * Remove preprocessor flag
    buildcc::base::Target simple(
        NAME, buildcc::base::TargetType::Executable,
        buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"),
        "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddPreprocessorFlag("-DRANDOM=1");
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    simple.Build();
  }
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_PREPROCESSOR_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
