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

  const std::vector<fs::path> &matches = findcmake.GetPathMatches();
  CHECK_TRUE(!matches.empty());
}

TEST(PluginsTestGroup, BuildccFind_BadEnv) {
  buildcc::plugin::BuildccFind findrandomenv(
      "cmake", buildcc::plugin::BuildccFind::Type::HostExecutable,
      {"FIND_RANDOM_ENV"});
  bool found = findrandomenv.Search();
  CHECK_FALSE(found);

  const std::vector<fs::path> &matches = findrandomenv.GetPathMatches();
  CHECK_TRUE(matches.empty());
}

TEST(PluginsTestGroup, BuildccFind_WrongExecutable) {
  buildcc::plugin::BuildccFind findrandomexecutable(
      "random_cmake_executable",
      buildcc::plugin::BuildccFind::Type::HostExecutable, {"FIND_RANDOM_ENV"});
  bool found = findrandomexecutable.Search();
  CHECK_FALSE(found);

  const std::vector<fs::path> &matches = findrandomexecutable.GetPathMatches();
  CHECK_TRUE(matches.empty());
}

TEST(PluginsTestGroup, BuildccFind_SearchUnimplemented) {
  {
    buildcc::plugin::BuildccFind findunimplemented(
        "random_library", buildcc::plugin::BuildccFind::Type::BuildccLibrary);
    bool found = findunimplemented.Search();
    CHECK_FALSE(found);

    const std::vector<fs::path> &matches = findunimplemented.GetPathMatches();
    CHECK_TRUE(matches.empty());
  }

  {
    buildcc::plugin::BuildccFind findunimplemented(
        "random_library", buildcc::plugin::BuildccFind::Type::BuildccPlugin);
    bool found = findunimplemented.Search();
    CHECK_FALSE(found);

    const std::vector<fs::path> &matches = findunimplemented.GetPathMatches();
    CHECK_TRUE(matches.empty());
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
