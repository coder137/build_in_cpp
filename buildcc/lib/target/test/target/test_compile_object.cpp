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

static buildcc::Toolchain gcc(buildcc::Toolchain::Id::Gcc, "gcc", "as", "gcc",
                              "g++", "ar", "ld");

TEST(TargetCompileObjectTestGroup, CacheCompileCommand_Invalid) {
  buildcc::BaseTarget target("CacheCompileCommand_Invalid",
                             buildcc::TargetType::Executable, gcc, "data");
  buildcc::base::CompileObject object(target);

  object.AddObjectData("random.invalid");

  // Target not Built, which causes an exception
  CHECK_THROWS(std::exception, object.CacheCompileCommands());
}

int main(int ac, char **av) {
  buildcc::env::init(fs::current_path(), fs::current_path() / "intermediate" /
                                             "target_compile_object");
  fs::remove_all(buildcc::env::get_project_build_dir());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
