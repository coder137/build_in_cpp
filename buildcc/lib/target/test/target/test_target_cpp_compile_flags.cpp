#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

#include "env/env.h"

//
#include "target/target_loader.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestCppCompileFlagsGroup)
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
    fs::path(BUILD_TARGET_CPP_COMPILE_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestCppCompileFlagsGroup, Target_AddCompileFlag) {
  constexpr const char *const NAME = "AddCppCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddCppCompileFlag("-std=c++17");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  simple.Build();

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::FbsLoader loader(NAME, simple.GetTargetIntermediateDir());
  bool loaded = loader.Load();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(loader.GetLoadedCppCompileFlags().size(), 1);
}

TEST(TargetTestCppCompileFlagsGroup, Target_ChangedCompileFlag) {
  constexpr const char *const NAME = "ChangedCppCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c++17");

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    simple.Build();
  }
  {
    // * Remove flag
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    simple.Build();
  }

  {
    // * Add flag
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c++17");
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    simple.Build();
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_CPP_COMPILE_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
