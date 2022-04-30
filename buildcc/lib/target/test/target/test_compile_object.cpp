#include "env/env.h"
#include "target/target.h"

#include "target/friend/compile_object.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetCompileObjectTestGroup)
{
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

TEST(TargetCompileObjectTestGroup, CacheCompileCommand_Invalid) {
  buildcc::BaseTarget target("CacheCompileCommand_Invalid",
                             buildcc::TargetType::Executable, gcc, "data");
  buildcc::internal::CompileObject object(target);

  object.AddObjectData("random.invalid");

  // Target not Built, which causes an exception
  CHECK_THROWS(std::exception, object.CacheCompileCommands());
}

int main(int ac, char **av) {
  buildcc::Project::Init(fs::current_path(), fs::current_path() /
                                                 "intermediate" /
                                                 "target_compile_object");
  fs::remove_all(buildcc::Project::GetBuildDir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
