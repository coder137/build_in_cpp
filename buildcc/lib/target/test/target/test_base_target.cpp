#include "constants.h"

#include "expect_command.h"

#include "target/target.h"

#include "env/env.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetBaseTestGroup)
{
  void teardown() {
    mock().clear();
  }
};
// clang-format on
static const buildcc::Toolchain gcc(buildcc::Toolchain::Id::Gcc, "gcc", "as",
                                    "gcc", "g++", "ar", "ld");

TEST(TargetBaseTestGroup, InvalidTargetType) {
  constexpr const char *const INVALID_NAME = "Invalid.random";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  auto intermediate_path =
      fs::path(BUILD_TARGET_BASE_INTERMEDIATE_DIR) / gcc.GetName();

  fs::remove_all(intermediate_path / INVALID_NAME);

  CHECK_THROWS(
      std::exception,
      buildcc::BaseTarget(INVALID_NAME, (buildcc::TargetType)3, gcc, ""));

  buildcc::env::deinit();
}

TEST(TargetBaseTestGroup, NoEnvInit) {
  constexpr const char *const NAME = "Init.exe";

  CHECK_THROWS(
      std::exception,
      buildcc::BaseTarget(NAME, buildcc::TargetType::Executable, gcc, "data"));
}

TEST(TargetBaseTestGroup, TargetConfig_BadCompileCommand) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  fs::path target_source_intermediate_path =
      buildcc::env::get_project_build_dir() / gcc.GetName();

  constexpr const char *const NAME = "BadCompileCommand.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  {
    buildcc::TargetConfig config;
    config.compile_command = "{invalid_compile_string}";
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data", config);
    CHECK(simple.GetType() == buildcc::TargetType::Executable);
    simple.AddSource("dummy_main.c");
    CHECK_THROWS(std::exception, simple.Build());
  }

  buildcc::env::deinit();
}

TEST(TargetBaseTestGroup, TargetConfig_BadLinkCommand) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  fs::path target_source_intermediate_path =
      buildcc::env::get_project_build_dir() / gcc.GetName();

  constexpr const char *const NAME = "BadCompileCommand.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  {
    buildcc::TargetConfig config;
    config.link_command = "{invalid_link_string}";
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data", config);
    simple.AddSource("dummy_main.c");
    CHECK_THROWS(std::exception, simple.Build());
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

// TODO, Check toolchain change
// There are few parameters that must NOT be changed after the initial buildcc
// project is generated
// NOTE, Throw an error if these options are changed by the user

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
