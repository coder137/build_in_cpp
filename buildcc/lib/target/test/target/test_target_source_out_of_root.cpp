#include "constants.h"

#include "expect_target.h"
#include "target.h"

#include "env.h"
#include "logging.h"

// Third Party
#include "flatbuffers/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestSourceOutOfRootGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar",
                                          "ld");
static const fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_SOURCE_OUT_OF_ROOT_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestSourceOutOfRootGroup, Add_OutOfRootSource) {
  constexpr const char *const OUTOFROOT = "OutOfRootSource.random";

  fs::remove_all(target_source_intermediate_path / OUTOFROOT);

  buildcc::base::Target simple(OUTOFROOT, buildcc::base::TargetType::Executable,
                               gcc, "");
  //  Out of source paths not allowed
  // NOTE, Everything is evaluated relative to the
  // buildcc::env::get_project_root
  CHECK_THROWS(std::exception, simple.AddSource("../dummy_main.cpp"));
}

TEST(TargetTestSourceOutOfRootGroup, Glob_OutOfRootSource) {
  constexpr const char *const OUTOFROOT = "GlobOutOfRootSources.exe";

  fs::remove_all(target_source_intermediate_path / OUTOFROOT);

  buildcc::base::Target simple(OUTOFROOT, buildcc::base::TargetType::Executable,
                               gcc, "");
  //  Out of source paths not allowed
  // NOTE, Everything is evaluated relative to the
  // buildcc::env::get_project_root
  CHECK_THROWS(std::exception, simple.GlobSources(".."));
  simple.GlobSourcesAbsolute(fs::path(BUILD_SCRIPT_SOURCE) / "data",
                             target_source_intermediate_path /
                                 simple.GetName() / "OUT_OF_SOURCE");

  CHECK_EQUAL(6, simple.GetCurrentSourceFiles().size());

  buildcc::internal::m::Expect_command(6, true);
  buildcc::internal::m::Expect_command(1, true);
  simple.Build();

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(fs::path(BUILD_SCRIPT_SOURCE) / "data" / "random dir",
                     BUILD_TARGET_SOURCE_OUT_OF_ROOT_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
