#include "constants.h"

#include "env/logging.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

#include "target/target_loader.h"

#include <iostream>
#include <unistd.h>

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

static const buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc,
                                          "gcc", "as", "gcc", "g++", "ar",
                                          "ld");

static const fs::path intermediate_path =
    fs::path(BUILD_TARGET_EXTERNAL_LIB_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestExternalLib, TestAddLibDir) {
  constexpr const char *const EXENAME = "libDir.exe";

  fs::remove_all(intermediate_path / EXENAME);

  buildcc::base::Target exe(EXENAME, buildcc::base::Target::Type::StaticLibrary,
                            gcc, "data");
  exe.AddSource("foo_main.cpp");
  exe.AddLibDir(exe.GetTargetPath().parent_path());

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  exe.Build();
  CHECK_TRUE(exe.GetBuildState());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetLoader loader(EXENAME, exe.GetTargetBuildDir());
  bool loaded = loader.Load();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(loader.GetLoadedLibDirs().size(), 1);
  CHECK_EQUAL(loader.GetLoadedExternalLibDeps().size(), 0);
}

TEST(TargetTestExternalLib, TestAddExternalLibDep_Simple) {
  constexpr const char *const EXENAME = "externalLibDep.exe";

  fs::remove_all(intermediate_path / EXENAME);

  buildcc::base::Target exe(EXENAME, buildcc::base::Target::Type::StaticLibrary,
                            gcc, "data");
  exe.AddSource("foo_main.cpp");
  exe.AddLibDir(exe.GetTargetPath().parent_path());
  exe.AddLibDep("-lfoo");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  exe.Build();
  CHECK_TRUE(exe.GetBuildState());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetLoader loader(EXENAME, exe.GetTargetBuildDir());
  bool loaded = loader.Load();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(loader.GetLoadedLibDirs().size(), 1);
  CHECK_EQUAL(loader.GetLoadedExternalLibDeps().size(), 1);
}

TEST(TargetTestExternalLib, TestAddExternalLibDep_RebuildChanged) {
  constexpr const char *const EXENAME = "externalRebuildLibDep.exe";

  fs::remove_all(intermediate_path / EXENAME);

  // First build
  {
    buildcc::base::Target exe(
        EXENAME, buildcc::base::Target::Type::StaticLibrary, gcc, "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    exe.Build();
    CHECK_TRUE(exe.GetBuildState());
  }

  // Add
  {
    buildcc::base::Target exe(
        EXENAME, buildcc::base::Target::Type::StaticLibrary, gcc, "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());
    exe.AddLibDep("-lfoo");

    buildcc::base::m::TargetExpect_ExternalLibChanged(1, &exe);
    buildcc::m::CommandExpect_Execute(1, true);
    exe.Build();
    CHECK_TRUE(exe.GetBuildState());
  }

  // Remove
  {
    buildcc::base::Target exe(
        EXENAME, buildcc::base::Target::Type::StaticLibrary, gcc, "data");
    exe.AddSource("foo_main.cpp");
    exe.AddLibDir(exe.GetTargetPath().parent_path());

    buildcc::base::m::TargetExpect_ExternalLibChanged(1, &exe);
    buildcc::m::CommandExpect_Execute(1, true);
    exe.Build();
    CHECK_TRUE(exe.GetBuildState());
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_EXTERNAL_LIB_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
