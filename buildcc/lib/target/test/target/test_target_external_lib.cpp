#include "constants.h"

#include "env/logging.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

#include "schema/target_serialization.h"

#include <iostream>

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestExternalLib)
{
    void teardown() {
        mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

static const fs::path intermediate_path =
    fs::path(BUILD_TARGET_EXTERNAL_LIB_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestExternalLib, TestAddLibDir) {
  constexpr const char *const EXENAME = "libDir.exe";

  fs::remove_all(intermediate_path / EXENAME);

  buildcc::BaseTarget exe(EXENAME, buildcc::TargetType::StaticLibrary, gcc,
                          "data");
  exe.AddSource("foo_main.cpp");
  exe.AddLibDir(exe.GetTargetPath().parent_path());

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);
  CHECK_TRUE(exe.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      exe.GetTargetBuildDir() / (std::string(EXENAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);
  CHECK_EQUAL(serialization.GetLoad().lib_dirs.size(), 1);
  CHECK_EQUAL(serialization.GetLoad().external_libs.size(), 0);
}

TEST(TargetTestExternalLib, TestAddExternalLibDep_Simple) {
  constexpr const char *const EXENAME = "externalLibDep.exe";

  fs::remove_all(intermediate_path / EXENAME);

  buildcc::BaseTarget exe(EXENAME, buildcc::TargetType::StaticLibrary, gcc,
                          "data");
  exe.AddSource("foo_main.cpp");
  exe.AddLibDir(exe.GetTargetPath().parent_path());
  exe.AddLibDep("-lfoo");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);
  CHECK_TRUE(exe.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      exe.GetTargetBuildDir() / (std::string(EXENAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().lib_dirs.size(), 1);
  CHECK_EQUAL(serialization.GetLoad().external_libs.size(), 1);
}

TEST(TargetTestExternalLib, TestAddExternalLibDep_RebuildChanged) {
  constexpr const char *const EXENAME = "externalRebuildLibDep.exe";

  fs::remove_all(intermediate_path / EXENAME);

  // First build
  {
    buildcc::BaseTarget exe(EXENAME, buildcc::TargetType::StaticLibrary, gcc,
                            "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe.Build();
    buildcc::m::TargetRunner(exe);
    CHECK_TRUE(exe.IsBuilt());
  }

  // Add
  {
    buildcc::BaseTarget exe(EXENAME, buildcc::TargetType::StaticLibrary, gcc,
                            "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());
    exe.AddLibDep("-lfoo");

    buildcc::m::TargetExpect_ExternalLibChanged(1, &exe);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe.Build();
    buildcc::m::TargetRunner(exe);
    CHECK_TRUE(exe.IsBuilt());
  }

  // Remove
  {
    buildcc::BaseTarget exe(EXENAME, buildcc::TargetType::StaticLibrary, gcc,
                            "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());

    buildcc::m::TargetExpect_ExternalLibChanged(1, &exe);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe.Build();
    buildcc::m::TargetRunner(exe);
    CHECK_TRUE(exe.IsBuilt());
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_EXTERNAL_LIB_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
