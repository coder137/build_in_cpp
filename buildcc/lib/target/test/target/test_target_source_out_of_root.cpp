#include "constants.h"

#include "expect_target.h"
#include "target/target.h"

#include "env/env.h"
#include "env/logging.h"

// Third Party

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

static const buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc,
                                          "gcc", "as", "gcc", "g++", "ar",
                                          "ld");

static const fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_SOURCE_OUT_OF_ROOT_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestSourceOutOfRootGroup, Add_OutOfRootSource) {
  constexpr const char *const OUTOFROOT = "OutOfRootSource.random";

  fs::remove_all(target_source_intermediate_path / OUTOFROOT);

  buildcc::base::Target simple(OUTOFROOT, buildcc::base::TargetType::Executable,
                               gcc, "");
  simple.AddSource("../dummy_main.cpp");

  buildcc::internal::m::CommandExpect_Execute(1, true);
  buildcc::internal::m::CommandExpect_Execute(1, true);
  simple.Build();
}

TEST(TargetTestSourceOutOfRootGroup, Glob_OutOfRootSource) {
  constexpr const char *const OUTOFROOT = "GlobOutOfRootSources.exe";

  fs::remove_all(target_source_intermediate_path / OUTOFROOT);

  buildcc::base::Target simple(OUTOFROOT, buildcc::base::TargetType::Executable,
                               gcc, "");
  simple.GlobSources(".."); // 6 files detected
  simple.GlobSourcesAbsolute(fs::path(BUILD_SCRIPT_SOURCE) / "data",
                             target_source_intermediate_path /
                                 simple.GetName() /
                                 "OUT_OF_SOURCE"); // 6 files detected

  CHECK_EQUAL(12, simple.GetCurrentSourceFiles().size());

  buildcc::internal::m::CommandExpect_Execute(12, true);
  buildcc::internal::m::CommandExpect_Execute(1, true);
  simple.Build();

  mock().checkExpectations();
}

int main(int ac, char **av) {
  std::filesystem::create_directories(fs::path(BUILD_SCRIPT_SOURCE) / "data" /
                                      "random dir");
  buildcc::env::init(fs::path(BUILD_SCRIPT_SOURCE) / "data" / "random dir",
                     BUILD_TARGET_SOURCE_OUT_OF_ROOT_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
