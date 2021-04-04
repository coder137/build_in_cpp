#include "constants.h"

#include "base/target.h"

#include "env.h"

//
#include "executable_target.h"
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

TEST(TargetTestStaticLibGroup, StaticLibrary_SimpleBuildTest) {
  constexpr const char *const STATIC_NAME = "libStaticTest.a";

  fs::remove_all(intermediate_path / STATIC_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  CHECK_TRUE(gcc.AddExecutable("ar", "ar"));
  // Re adding it should not be allowed
  CHECK_FALSE(gcc.AddExecutable("ar", "ar"));

  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
  static_target.AddSource("lib/random_lib.cpp");
  static_target.AddIncludeDir("lib");
  static_target.Build();

  // TODO, Verify .binary file
}

TEST(TargetTestStaticLibGroup,
     StaticLibrary_ExecutableTargetDep_SimpleBuildRebuildTest) {
  constexpr const char *const STATIC_NAME = "libRandomTest.a";
  constexpr const char *const EXE_NAME = "staticTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  CHECK_TRUE(gcc.AddExecutable("ar", "ar"));
  // Re adding it should not be allowed
  CHECK_FALSE(gcc.AddExecutable("ar", "ar"));

  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("lib/random_lib.cpp");
    static_target.AddIncludeDir("lib");
    static_target.Build();

    // Executable for static
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("lib_tester.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }

  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("lib/random_lib.cpp");
    static_target.AddIncludeDir("lib");
    static_target.Build();

    // Executable for static
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("lib_tester.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup,
     StaticLibrary_ExecutableTargetDep_AddRemoveTest) {
  constexpr const char *const STATIC_NAME = "libAddRemoveRandomTest.a";
  constexpr const char *const EXE_NAME = "staticAddRemoveTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  CHECK_TRUE(gcc.AddExecutable("ar", "ar"));

  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
  static_target.AddSource("lib/random_lib.cpp");
  static_target.AddIncludeDir("lib");
  static_target.Build();

  // * Initial executable
  // Executable for static
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("dummy_main.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.Build();
  }

  // * Add new library
  // Build
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("dummy_main.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }

  // * Remove library
  // Build
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("dummy_main.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup,
     StaticLibrary_ExecutableTargetDep_UpdateExistingLibraryTest) {
  constexpr const char *const STATIC_NAME = "libUpdateLibRandomTest.a";
  constexpr const char *const EXE_NAME = "staticUpdateLibTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::Toolchain gcc("gcc", "gcc", "g++");
  CHECK_TRUE(gcc.AddExecutable("ar", "ar"));

  // Build initial
  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("lib/random_lib.cpp");
    static_target.AddIncludeDir("lib");
    static_target.Build();

    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("dummy_main.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }

  // * Update static library
  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("lib/random_lib.cpp");
    static_target.AddIncludeDir("lib");
    static_target.AddIncludeDir(".");
    static_target.Build();

    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("dummy_main.cpp");
    exe_target.AddIncludeDir("lib");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_STATIC_LIB_INTERMEDIATE_DIR);
  buildcc::env::set_log_level(buildcc::env::LogLevel::Trace);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
