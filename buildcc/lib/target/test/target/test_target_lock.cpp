#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

#include "env/env.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestLock)
{
    void teardown() {
        mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

TEST(TargetTestLock, LockState) {
  constexpr const char *const NAME = "LockState.exe";
  buildcc::BaseTarget exe(NAME, buildcc::TargetType::Executable, gcc, "data");

  CHECK_FALSE(exe.IsLocked());

  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);

  CHECK_TRUE(exe.IsLocked());

  mock().checkExpectations();
}

TEST(TargetTestLock, Lock_Build) {
  constexpr const char *const NAME = "Lock_Build.exe";
  buildcc::BaseTarget exe(NAME, buildcc::TargetType::Executable, gcc, "data");

  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);

  CHECK_THROWS(std::exception, exe.Build());

  mock().checkExpectations();
}

TEST(TargetTestLock, Lock_APIs) {
  constexpr const char *const NAME = "Lock_APIs.exe";
  buildcc::BaseTarget exe(NAME, buildcc::TargetType::Executable, gcc, "data");

  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);

  mock().checkExpectations();

  // Sources
  CHECK_THROWS(std::exception, exe.AddSource("dummy_main.c"));
  CHECK_THROWS(std::exception, exe.GlobSources(""));

  // Headers & Dirs
  CHECK_THROWS(std::exception, exe.AddHeader("include/include_header.h"));
  CHECK_THROWS(std::exception, exe.GlobHeaders("include/"));
  CHECK_THROWS(std::exception, exe.AddIncludeDir("include/"));
  CHECK_THROWS(std::exception, exe.AddLibDir("include/"));

  // LibDep
  CHECK_THROWS(std::exception, exe.AddLibDep("-lpthread"));
  CHECK_THROWS(std::exception, exe.AddLibDep(exe));

  // Flags
  CHECK_THROWS(std::exception, exe.AddPreprocessorFlag("-DTESTING=TRUE"));
  CHECK_THROWS(std::exception, exe.AddCommonCompileFlag("-Os"));
  CHECK_THROWS(std::exception, exe.AddAsmCompileFlag("-march=arm"));
  CHECK_THROWS(std::exception, exe.AddCCompileFlag("-std=c11"));
  CHECK_THROWS(std::exception, exe.AddCppCompileFlag("-std=c++17"));
  CHECK_THROWS(std::exception, exe.AddLinkFlag("-nostd"));

  // Rebuild
  CHECK_THROWS(std::exception, exe.AddCompileDependency("dummy_main.c"));
  CHECK_THROWS(std::exception, exe.AddLinkDependency("dummy_main.c"));
}

TEST(TargetTestLock, Unlock_APIs) {
  constexpr const char *const NAME = "Unlock_APIs.exe";
  buildcc::BaseTarget exe(NAME, buildcc::TargetType::Executable, gcc, "data");

  CHECK_THROWS(std::exception,
               exe.GetCompileCommand(exe.GetTargetRootDir() / "dummy_main.c"));
  CHECK_THROWS(std::exception, exe.GetLinkCommand());
  CHECK_THROWS(std::exception, exe.GetTaskflow());

  exe.AddSource("dummy_main.c");
  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  exe.Build();
  buildcc::m::TargetRunner(exe);
  mock().checkExpectations();

  exe.GetCompileCommand(exe.GetTargetRootDir() / "dummy_main.c");
  exe.GetLinkCommand();
  exe.GetTaskflow();
}

int main(int ac, char **av) {
  const fs::path target_source_intermediate_path =
      fs::path(BUILD_TARGET_LOCK_INTERMEDIATE_DIR) / gcc.GetName();
  fs::remove_all(target_source_intermediate_path);

  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_LOCK_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
