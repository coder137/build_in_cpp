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
static const buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar",
                                          "ld");

TEST(TargetBaseTestGroup, InvalidTargetType) {
  constexpr const char *const INVALID_NAME = "Invalid.random";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  auto intermediate_path =
      fs::path(BUILD_TARGET_BASE_INTERMEDIATE_DIR) / gcc.GetName();

  fs::remove_all(intermediate_path / INVALID_NAME);

  CHECK_THROWS(std::exception,
               buildcc::base::Target(INVALID_NAME, (buildcc::base::TargetType)3,
                                     gcc, ""));

  buildcc::env::deinit();
}

TEST(TargetBaseTestGroup, NoEnvInit) {
  constexpr const char *const NAME = "Init.exe";

  CHECK_THROWS(std::exception,
               buildcc::base::Target(
                   NAME, buildcc::base::TargetType::Executable, gcc, "data"));
}

TEST(TargetBaseTestGroup, OutofRootSource) {
  constexpr const char *const OUTOFROOT = "OutOfRootSource.random";

  buildcc::env::init(fs::path(BUILD_SCRIPT_SOURCE) / "data" / "foo",
                     BUILD_TARGET_BASE_INTERMEDIATE_DIR);
  auto intermediate_path =
      fs::path(BUILD_TARGET_BASE_INTERMEDIATE_DIR) / gcc.GetName();

  fs::remove_all(intermediate_path / OUTOFROOT);

  buildcc::base::Target simple(OUTOFROOT, buildcc::base::TargetType::Executable,
                               gcc, "");
  //  Out of source paths not allowed
  // NOTE, Everything is evaluated relative to the
  // buildcc::env::get_project_root
  CHECK_THROWS(std::exception, simple.AddSource("../dummy_main.cpp"));

  buildcc::env::deinit();
}

// TODO, Check toolchain change
// There are few parameters that must NOT be changed after the initial buildcc
// project is generated
// NOTE, Throw an error if these options are changed by the user

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
