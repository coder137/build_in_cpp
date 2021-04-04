#include "constants.h"

#include "base/target.h"

#include "internal/util.h"

#include "env.h"

//
#include "dynamic_target.h"
#include "executable_target.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetTestDynamicLibGroup)
{
};
// clang-format on

static fs::path intermediate_path =
    fs::path(BUILD_TARGET_DYNAMIC_LIB_INTERMEDIATE_DIR);

TEST(TargetTestDynamicLibGroup, DynamicLibrary_SimpleBuildTest) {
  constexpr const char *const DYNAMIC_TARGET = "libDynamicTest.so";

  fs::remove_all(intermediate_path / DYNAMIC_TARGET);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  buildcc::DynamicTarget dynamic_target(DYNAMIC_TARGET, gcc, "");
  dynamic_target.AddSource("data/include_header.cpp");
  dynamic_target.AddIncludeDir("data/include");
  dynamic_target.Build();

  // TODO, Add Verification
}

TEST(TargetTestDynamicLibGroup,
     DynamicLibrary_ExecutableTargetDep_SimpleBuildRebuildTest) {
  constexpr const char *const DYNAMIC_NAME = "libRandomTest.so";
  constexpr const char *const EXE_NAME = "dynamicTestExe.exe";

  fs::remove_all(intermediate_path / DYNAMIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");

  {
    buildcc::DynamicTarget dynamic_target(DYNAMIC_NAME, gcc, "data");
    dynamic_target.AddSource("lib/random_lib.cpp");
    dynamic_target.AddIncludeDir("lib");
    dynamic_target.Build();

    // Executable for dynamic
    // NOTE, This runs on linux
    // On Windows we need to copy the linux .so library to the directory of the
    // executable
    // Make sure the overrides in DynamicTarget is updated accordingly
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("lib_tester.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(dynamic_target);
    exe_target.Build();
  }

  // TODO, Rebuild test
}

// TODO, Update tests are per `test_target_static_lib.cpp`
// TODO, Add, Remove and Update shared library

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_DYNAMIC_LIB_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
