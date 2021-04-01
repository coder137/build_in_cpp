#include "constants.h"

#include "base/target.h"

#include "env.h"

//
#include "static_target.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetTestStaticLibGroup)
{
};
// clang-format on

static fs::path intermediate_path =
    fs::path(BUILD_TARGET_STATIC_LIB_INTERMEDIATE_DIR);

TEST(TargetTestStaticLibGroup, StaticLibrarySimple) {
  constexpr const char *const STATIC_NAME = "libStaticTest.a";

  fs::remove_all(intermediate_path / STATIC_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  CHECK_TRUE(gcc.AddExecutable("ar", "ar"));
  // Re adding it should not be allowed
  CHECK_FALSE(gcc.AddExecutable("ar", "ar"));

  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "");
  static_target.AddSource("data/include_header.cpp");
  static_target.AddIncludeDir("data/include");
  static_target.Build();
}

TEST(TargetTestStaticLibGroup, StaticLibCheckAr) {
  constexpr const char *const STATIC_NAME = "libStaticTest.a";

  // Check if "ar" compiler is present
  fs::remove_all(intermediate_path / STATIC_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "");
  static_target.AddSource("data/include_header.cpp");
  static_target.AddIncludeDir("data/include");
  CHECK_THROWS(std::string, static_target.Build());
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_STATIC_LIB_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
