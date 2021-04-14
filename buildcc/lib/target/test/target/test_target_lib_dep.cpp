#include "constants.h"

#include "env.h"

#include "expect_target.h"
#include "target.h"

#include "flatbuffers/util.h"

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

  buildcc::base::Target foolib(
      STATIC_NAME, buildcc::base::TargetType::StaticLibrary, gcc, "data");
  foolib.AddSource("foo.cpp", "foo");
  foolib.AddIncludeDir("foo");

  buildcc::internal::m::Expect_command(1, true);
  buildcc::internal::m::Expect_command(1, true);
  foolib.Build();

  // TODO, Verify .binary file
}

TEST(TargetTestStaticLibGroup, TargetDep_RebuildTest) {
  constexpr const char *const STATIC_FOO_LIB = "libfoo.a";
  constexpr const char *const EXE_NAME = "staticTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_FOO_LIB);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  {
    buildcc::base::Target foolib(
        STATIC_FOO_LIB, buildcc::base::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    foolib.Build();
    flatbuffers::SaveFile(foolib.GetTargetPath().string().c_str(),
                          std::string{""}, false);

    // Executable for static
    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    exe_target.Build();
  }

  {
    buildcc::base::Target foolib(
        STATIC_FOO_LIB, buildcc::base::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    foolib.Build();

    // Executable for static
    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup, TargetDep_AddRemoveTest) {
  constexpr const char *const STATIC_NAME = "libAddRemoveRandomTest.a";
  constexpr const char *const EXE_NAME = "staticAddRemoveTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  buildcc::base::Target foolib(
      STATIC_NAME, buildcc::base::TargetType::StaticLibrary, gcc, "data");
  foolib.AddSource("foo/foo.cpp");
  foolib.AddIncludeDir("foo");

  buildcc::internal::m::Expect_command(1, true);
  buildcc::internal::m::Expect_command(1, true);
  foolib.Build();

  flatbuffers::SaveFile(foolib.GetTargetPath().string().c_str(),
                        std::string{""}, false);

  // * Initial executable
  // Executable for static
  {
    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    exe_target.Build();
  }

  // * Add new library
  // Build
  {
    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::base::m::TargetExpect_PathAdded(1, &exe_target);
    buildcc::internal::m::Expect_command(1, true);
    exe_target.Build();
  }

  // * Remove library
  {
    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("empty_main.cpp");
    exe_target.AddIncludeDir("foo");

    buildcc::base::m::TargetExpect_PathRemoved(1, &exe_target);
    buildcc::internal::m::Expect_command(1, true);
    exe_target.Build();
  }
}

TEST(TargetTestStaticLibGroup, TargetDep_UpdateExistingLibraryTest) {
  constexpr const char *const STATIC_NAME = "libUpdateLibRandomTest.a";
  constexpr const char *const EXE_NAME = "staticUpdateLibTestExe.exe";

  fs::remove_all(intermediate_path / STATIC_NAME);
  fs::remove_all(intermediate_path / EXE_NAME);

  buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar", "ld");

  // Build initial
  {
    buildcc::base::Target foolib(
        STATIC_NAME, buildcc::base::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    foolib.Build();

    flatbuffers::SaveFile(foolib.GetTargetPath().string().c_str(),
                          std::string{""}, false);

    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    exe_target.Build();
  }

  // * Update static library
  {
    buildcc::base::Target foolib(
        STATIC_NAME, buildcc::base::TargetType::StaticLibrary, gcc, "data");
    foolib.AddSource("foo/foo.cpp");
    foolib.AddIncludeDir("foo");
    foolib.AddIncludeDir(".");

    buildcc::base::m::TargetExpect_PathAdded(1, &foolib);
    buildcc::internal::m::Expect_command(1, true);
    buildcc::internal::m::Expect_command(1, true);
    foolib.Build();

    flatbuffers::SaveFile(foolib.GetTargetPath().string().c_str(),
                          std::string{""}, false);

    buildcc::base::Target exe_target(
        EXE_NAME, buildcc::base::TargetType::Executable, gcc, "data");
    exe_target.AddSource("foo_main.cpp");
    exe_target.AddIncludeDir("foo");
    exe_target.AddLibDep(foolib);

    buildcc::base::m::TargetExpect_PathUpdated(1, &exe_target);
    buildcc::internal::m::Expect_command(1, true);
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
