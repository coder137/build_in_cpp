#include "constants.h"

#include "base/target.h"

#include "env.h"

//
#include "dynamic_target.h"

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

TEST(TargetTestDynamicLibGroup, DynamicLibrarySimple) {
  constexpr const char *const DYNAMIC_TARGET = "libDynamicTest.so";

  fs::remove_all(intermediate_path / DYNAMIC_TARGET);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  buildcc::DynamicTarget dynamic_target(DYNAMIC_TARGET, gcc, "");
  dynamic_target.AddSource("data/include_header.cpp");
  dynamic_target.AddIncludeDir("data/include");
  dynamic_target.Build();

  // TODO, Add Verification
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_DYNAMIC_LIB_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
