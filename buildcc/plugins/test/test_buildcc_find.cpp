#include "plugins/buildcc_find.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(PluginsTestGroup)
{
};
// clang-format on

TEST(PluginsTestGroup, BuildccFind_Search) {
  buildcc::plugin::BuildccFind findcmake(
      "cmake", buildcc::plugin::BuildccFind::Type::HostExecutable, {"PATH"});
  bool found = findcmake.Search();
  CHECK_TRUE(found);

  std::vector<fs::path> matches = findcmake.GetPathMatches();
  CHECK_TRUE(!matches.empty());
}

TEST(PluginsTestGroup, BuildccFind_BadEnv) {}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
