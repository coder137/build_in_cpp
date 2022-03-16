#include "constants.h"

#include "env/logging.h"
#include "env/util.h"

#include "expect_command.h"
#include "expect_target.h"
#include "test_target_util.h"

#include "target/target.h"

//
#include "schema/target_serialization.h"

#include <iostream>

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestLibDep)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::Toolchain::Id::Gcc, "gcc", "as", "gcc",
                              "g++", "ar", "ld");

static const fs::path intermediate_path =
    fs::path(BUILD_TARGET_LIB_DEP_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestLibDep, StaticLibrary_SimpleBuildTest) {
  constexpr const char *const STATIC_NAME = "libStaticTest.a";

  fs::remove_all(intermediate_path / STATIC_NAME);

  buildcc::BaseTarget foolib(STATIC_NAME, buildcc::TargetType::StaticLibrary,
                             gcc, "data");
  foolib.AddSource("foo.cpp", "foo");
  foolib.AddIncludeDir("foo");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  foolib.Build();
  buildcc::m::TargetRunner(foolib);

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      foolib.GetTargetBuildDir() / (std::string(STATIC_NAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().internal_sources.size(), 1);
  CHECK_EQUAL(serialization.GetLoad().include_dirs.size(), 1);
}

TEST(TargetTestLibDep, TargetDep_RebuildTest) {
  constexpr const char *const STATIC_FOO_LIB = "libfoo.a";
  constexpr const char *const EXE_NAME = "staticTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_FOO_LIB);
  fs::remove_all(intermediate_path / EXE_NAME);

  {
    buildcc::BaseTarget foolib(STATIC_FOO_LIB,
                               buildcc::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    foolib.Build();
    buildcc::m::TargetRunner(foolib);
    buildcc::env::save_file(foolib.GetTargetPath().string().c_str(),
                            std::string{""}, false);

    // Executable for static
    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  {
    buildcc::BaseTarget foolib(STATIC_FOO_LIB,
                               buildcc::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");
    foolib.Build();
    buildcc::m::TargetRunner(foolib);
    CHECK_FALSE(foolib.IsBuilt());

    // Executable for static
    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
    CHECK_FALSE(exe_target.IsBuilt());
  }

  mock().checkExpectations();
}

TEST(TargetTestLibDep, TargetDep_AddRemoveTest) {
  constexpr const char *const STATIC_NAME = "libAddRemoveRandomTest.a";
  constexpr const char *const EXE_NAME = "staticAddRemoveTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::BaseTarget foolib(STATIC_NAME, buildcc::TargetType::StaticLibrary,
                             gcc, "data");
  foolib.AddSource("foo/foo.cpp");
  foolib.AddIncludeDir("foo");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  foolib.Build();
  buildcc::m::TargetRunner(foolib);

  buildcc::env::save_file(foolib.GetTargetPath().string().c_str(),
                          std::string{""}, false);

  // * Initial executable
  // Executable for static
  {
    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  // * Add new library
  // Build
  {
    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::m::TargetExpect_PathAdded(1, &exe_target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  // * Remove library
  {
    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");

    buildcc::m::TargetExpect_PathRemoved(1, &exe_target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  mock().checkExpectations();
}

TEST(TargetTestLibDep, TargetDep_UpdateExistingLibraryTest) {
  constexpr const char *const STATIC_NAME = "libUpdateLibRandomTest.a";
  constexpr const char *const EXE_NAME = "staticUpdateLibTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  // Build initial
  {
    buildcc::BaseTarget foolib(STATIC_NAME, buildcc::TargetType::StaticLibrary,
                               gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    foolib.Build();
    buildcc::m::TargetRunner(foolib);

    bool saved = buildcc::env::save_file(
        foolib.GetTargetPath().string().c_str(), std::string{""}, false);
    CHECK_TRUE(saved);

    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  // * Update static library
  {
    buildcc::BaseTarget foolib(STATIC_NAME, buildcc::TargetType::StaticLibrary,
                               gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");
    foolib.AddIncludeDir("");

    buildcc::m::TargetExpect_DirChanged(1, &foolib);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    foolib.Build();
    buildcc::m::TargetRunner(foolib);

    // * To make sure that save_file is newer
    buildcc::m::blocking_sleep(1);
    bool saved = buildcc::env::save_file(
        foolib.GetTargetPath().string().c_str(), std::string{""}, false);
    CHECK_TRUE(saved);

    buildcc::BaseTarget exe_target(EXE_NAME, buildcc::TargetType::Executable,
                                   gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::m::TargetExpect_PathUpdated(1, &exe_target);
    buildcc::env::m::CommandExpect_Execute(1, true);
    exe_target.Build();
    buildcc::m::TargetRunner(exe_target);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_LIB_DEP_INTERMEDIATE_DIR);

  gcc.Lock();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
