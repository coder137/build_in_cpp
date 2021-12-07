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
TEST_GROUP(ToolchainTestGroup)
{
  void teardown() {
    mock().checkExpectations();
    mock().clear();
  }
};
// clang-format on

// NOTE, We are mocking the environment instead of actually querying it
TEST(ToolchainTestGroup, VerifyToolchain_Gcc) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::m::CommandExpect_Execute(1, true, &arch_stdout_data);

  std::string putenv_str = fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc",
                                       fs::current_path().string());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.env_vars.push_back("CUSTOM_BUILDCC_PATH");

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(!verified_toolchains.empty());
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(), "arch");
}

TEST(ToolchainTestGroup, VerifyToolchain_Clang) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Clang, "clang",
                               "llvm-as", "clang", "clang++", "llvm-ar", "lld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::m::CommandExpect_Execute(1, true, &arch_stdout_data);

  std::string putenv_str = fmt::format(
      "CUSTOM_BUILDCC_PATH={}/toolchains/clang", fs::current_path().string());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.env_vars.push_back("CUSTOM_BUILDCC_PATH");

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(!verified_toolchains.empty());
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(), "arch");
}

int main(int ac, char **av) {
  buildcc::m::VectorStringCopier copier;
  mock().installCopier(TEST_VECTOR_STRING_TYPE, copier);

  // NOTE, Check the GCC, MSVC and Clang compilers
  // Create directory and populate it with gcc and cl executables
  // Linux
  // toolchains/gcc
  // toolchains/clang_linux

  // Windows
  // toolchains/msvc
  // toolchains/mingw
  // TODO, Check executables used in clang_win

  // Linux + Windows + Mac for Cross compilation
  // toolchains/arm-none-eabi-gcc
  return CommandLineTestRunner::RunAllTests(ac, av);
}
