#include "constants.h"

#include "env.h"

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

TEST(TargetTestStaticLibGroup, StaticLibrary_SimpleBuildTest) {
  constexpr const char *const STATIC_NAME = "libStaticTest.a";

  fs::remove_all(intermediate_path / STATIC_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
  static_target.AddSource("foo.cpp", "foo");
  static_target.AddIncludeDir("foo");
  static_target.Build();

  // TODO, Verify .binary file
}

IGNORE_TEST(TargetTestStaticLibGroup,
            StaticLibrary_ExecutableTargetDep_RebuildTest) {
  constexpr const char *const STATIC_FOO_LIB = "libfoo.a";
  constexpr const char *const EXE_NAME = "staticTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_FOO_LIB);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  {
    buildcc::StaticTarget foolib(STATIC_FOO_LIB, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");
    foolib.Build();

    // Executable for static
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);
    exe_target.Build();
  }

  {
    buildcc::StaticTarget foolib(STATIC_FOO_LIB, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");
    foolib.Build();

    // Executable for static
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup,
     StaticLibrary_ExecutableTargetDep_AddRemoveTest) {
  constexpr const char *const STATIC_NAME = "libAddRemoveRandomTest.a";
  constexpr const char *const EXE_NAME = "staticAddRemoveTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
  static_target.AddSource("foo/foo.cpp");
  static_target.AddIncludeDir("foo");
  static_target.Build();

  // * Initial executable
  // Executable for static
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.Build();
  }

  // * Add new library
  // Build
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }

  // * Remove library
  // Build
  {
    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup,
     StaticLibrary_ExecutableTargetDep_UpdateExistingLibraryTest) {
  constexpr const char *const STATIC_NAME = "libUpdateLibRandomTest.a";
  constexpr const char *const EXE_NAME = "staticUpdateLibTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  // Build initial
  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("foo/foo.cpp");
    static_target.AddIncludeDir("foo");
    static_target.Build();

    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }

  // * Update static library
  {
    buildcc::StaticTarget static_target(STATIC_NAME, gcc, "data");
    static_target.AddSource("foo/foo.cpp");
    static_target.AddIncludeDir("foo");
    static_target.AddIncludeDir(".");
    static_target.Build();

    buildcc::ExecutableTarget exe_target(EXE_NAME, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(static_target);
    exe_target.Build();
  }
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_STATIC_LIB_INTERMEDIATE_DIR);
  buildcc::env::set_log_level(buildcc::env::LogLevel::Trace);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
