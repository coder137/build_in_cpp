// Internal
#include "internal/fbs_loader.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(FbsLoaderTestGroup)
{
};
// clang-format on

TEST(FbsLoaderTestGroup, LoadSchemaData) {
  {
    buildcc::internal::FbsLoader loader("Simple.exe", "");
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
  }

  {
    buildcc::internal::FbsLoader loader("NotExist.exe", "");
    bool is_loaded = loader.Load();
    CHECK_FALSE(is_loaded);
  }
}

TEST(FbsLoaderTestGroup, GetLoadedSources) {
  buildcc::internal::FbsLoader loader("Simple.exe", "");
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
