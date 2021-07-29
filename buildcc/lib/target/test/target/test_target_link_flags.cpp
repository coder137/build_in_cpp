#include "constants.h"

#include "expect_target.h"
#include "target/target.h"

#include "env/env.h"

//
#include "target/fbs_loader.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestLinkFlagsGroup)
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
    fs::path(BUILD_TARGET_LINK_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestLinkFlagsGroup, Target_AddLinkFlag) {
  constexpr const char *const NAME = "AddLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddLinkFlag("-lm");

  buildcc::internal::m::CommandExpect_Execute(1, true);
  buildcc::internal::m::CommandExpect_Execute(1, true);
  simple.Build();

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::FbsLoader loader(NAME, simple.GetTargetIntermediateDir());
  bool loaded = loader.Load();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(loader.GetLoadedLinkFlags().size(), 1);
}

TEST(TargetTestLinkFlagsGroup, Target_ChangedLinkFlag) {
  constexpr const char *const NAME = "ChangedLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");

    buildcc::internal::m::CommandExpect_Execute(1, true);
    buildcc::internal::m::CommandExpect_Execute(1, true);
    simple.Build();
  }
  {
    // * Remove flag
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::CommandExpect_Execute(1, true);
    simple.Build();
  }

  {
    // * Add flag
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");
    buildcc::base::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::internal::m::CommandExpect_Execute(1, true);
    simple.Build();
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_LINK_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
