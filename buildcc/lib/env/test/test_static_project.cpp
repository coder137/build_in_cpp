#include "env/env.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(StaticProjectTestGroup)
{
  void setup() {
  }
};
// clang-format on

TEST(StaticProjectTestGroup, ProjectInitialized) {
  CHECK_FALSE(buildcc::Project::IsInit());
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  CHECK_TRUE(buildcc::Project::IsInit());
  buildcc::Project::Deinit();
  CHECK_FALSE(buildcc::Project::IsInit());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
