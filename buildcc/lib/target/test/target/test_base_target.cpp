#include "constants.h"

#include "target.h"

#include "env.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetBaseTestGroup)
{
};
// clang-format on

TEST(TargetBaseTestGroup, InvalidTargetType) {
  constexpr const char *const INVALID_NAME = "Invalid.random";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  auto intermediate_path = fs::path(BUILD_TARGET_BASE_INTERMEDIATE_DIR);

  fs::remove_all(intermediate_path / INVALID_NAME);

  CHECK_THROWS(
      std::exception,
      buildcc::base::Target(
          INVALID_NAME, (buildcc::base::TargetType)3,
          buildcc::base::Toolchain("gcc", "as", "gcc", "g++", "ar", "ld"), ""));

  buildcc::env::deinit();
}

TEST(TargetBaseTestGroup, NoEnvInit) {
  constexpr const char *const NAME = "Init.exe";

  CHECK_THROWS(std::exception, buildcc::base::Target(
                                   NAME, buildcc::base::TargetType::Executable,
                                   buildcc::base::Toolchain("gcc", "as", "gcc",
                                                            "g++", "ar", "ld"),
                                   "data"));
}

// TODO, Check toolchain change
// There are few parameters that must NOT be changed after the initial buildcc
// project is generated
// NOTE, Throw an error if these options are changed by the user

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
