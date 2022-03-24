#include <cstdlib>

#include "toolchain/toolchain.h"

#include "env/host_os.h"

#include "expect_command.h"

#include "mock_command_copier.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(ToolchainFindTestGroup)
{
  void teardown() {
    mock().checkExpectations();
    mock().clear();
  }
};
// clang-format on

TEST(ToolchainFindTestGroup, FindToolchain_ThroughEnvVar) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  std::string putenv_str = fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc",
                                       fs::current_path().string());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  std::vector<fs::path> found_toolchains = gcc.Find(config);
  CHECK_TRUE(!found_toolchains.empty());
}

TEST(ToolchainFindTestGroup, FindToolchain_ThroughAbsolutePath) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainFindConfig config;
  config.absolute_search_paths.insert(fs::current_path() / "toolchains" /
                                      "gcc");
  config.env_vars.clear();

  std::vector<fs::path> found_toolchains = gcc.Find(config);
  CHECK_TRUE(!found_toolchains.empty());
}

TEST(ToolchainFindTestGroup, FindToolchain_DirectoryDoesntExist) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainFindConfig config;
  config.absolute_search_paths.insert(fs::current_path() / "toolchains" /
                                      "directory_doesnt_exist");
  config.env_vars.clear();

  std::vector<fs::path> found_toolchains = gcc.Find(config);
  CHECK_TRUE(found_toolchains.empty());
}

TEST(ToolchainFindTestGroup, FindToolchain_NoDirectoryFound) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainFindConfig config;
  config.absolute_search_paths.insert(fs::current_path() / "toolchains" /
                                      "gcc" / "ar");
  config.env_vars.clear();

  std::vector<fs::path> found_toolchains = gcc.Find(config);
  CHECK_TRUE(found_toolchains.empty());
}

TEST(ToolchainFindTestGroup, FindToolchain_NoToolchainFound) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainFindConfig config;
  config.absolute_search_paths.insert(fs::current_path() / "toolchains");
  config.env_vars.clear();

  std::vector<fs::path> found_toolchains = gcc.Find(config);
  CHECK_TRUE(found_toolchains.empty());
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
