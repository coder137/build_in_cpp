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
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout_data);

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
  buildcc::base::Toolchain clang(buildcc::base::Toolchain::Id::Clang, "clang",
                                 "llvm-as", "clang", "clang++", "llvm-ar",
                                 "lld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout_data);

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
      clang.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(!verified_toolchains.empty());
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(), "arch");
}

TEST(ToolchainTestGroup, VerifyToolchain_Msvc) {
  buildcc::base::Toolchain msvc(buildcc::base::Toolchain::Id::Msvc, "msvc",
                                "cl", "cl", "cl", "lib", "link");
  // Setup ENV
  // VSCMD_VER
  std::string vscmd_ver = std::string("VSCMD_VER=version");
  // VSCMD_ARG_HOST_ARCH
  std::string host_arch = std::string("VSCMD_ARG_HOST_ARCH=host_arch");
  // VSCMD_ARG_TGT_ARCH
  std::string tgt_arch = std::string("VSCMD_ARG_TGT_ARCH=tgt_arch");

  CHECK_TRUE(putenv(vscmd_ver.data()) == 0);
  CHECK_TRUE(putenv(host_arch.data()) == 0);
  CHECK_TRUE(putenv(tgt_arch.data()) == 0);

  // MSVC Compiler
  std::string putenv_str = fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/msvc",
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
      msvc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(!verified_toolchains.empty());
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(),
               "host_arch_tgt_arch");
}

TEST(ToolchainTestGroup, VerifyToolchain_BadCompilerId) {
  buildcc::base::Toolchain gcc((buildcc::base::Toolchain::Id)65535, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

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
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(), "");
}

TEST(ToolchainTestGroup, VerifyToolchain_BadAbsolutePath) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "does_not_exist").string());

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(verified_toolchains.empty());
}

TEST(ToolchainTestGroup, VerifyToolchain_PathContainsDir) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains").string());

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(verified_toolchains.empty());
}

TEST(ToolchainTestGroup, VerifyToolchain_LockedFolder) {
  std::error_code err;
  fs::permissions(fs::current_path() / "toolchains" / "gcc", fs::perms::none,
                  err);
  if (err) {
    FAIL_TEST("Could not set file permissions");
  }

  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(verified_toolchains.empty());

  fs::permissions(fs::current_path() / "toolchains" / "gcc", fs::perms::all,
                  err);
  if (err) {
    FAIL_TEST("Could not set file permissions");
  }
}

TEST(ToolchainTestGroup, VerifyToolchain_ConditionalAdd_CompilerVersion) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());
  config.compiler_version = "10.2.1";

  std::vector<std::string> compiler_version{"10.2.1"};
  std::vector<std::string> arch{"none"};
  buildcc::env::m::CommandExpect_Execute(1, true, &compiler_version, nullptr);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch, nullptr);

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_EQUAL(verified_toolchains.size(), 1);
}

TEST(ToolchainTestGroup,
     VerifyToolchain_ConditionalAdd_CompilerVersionFailure) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());
  config.compiler_version = "11.0.0";

  std::vector<std::string> compiler_version{"10.2.1"};
  std::vector<std::string> arch{"none"};
  buildcc::env::m::CommandExpect_Execute(1, true, &compiler_version, nullptr);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch, nullptr);

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_EQUAL(verified_toolchains.size(), 0);
}

TEST(ToolchainTestGroup, VerifyToolchain_ConditionalAdd_TargetArch) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());
  config.target_arch = "arm-none-eabi";

  std::vector<std::string> compiler_version{"10.2.1"};
  std::vector<std::string> arch{"arm-none-eabi"};
  buildcc::env::m::CommandExpect_Execute(1, true, &compiler_version, nullptr);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch, nullptr);

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_EQUAL(verified_toolchains.size(), 1);
}

TEST(ToolchainTestGroup, VerifyToolchain_ConditionalAdd_TargetArchFailure) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());
  config.target_arch = "none";

  std::vector<std::string> compiler_version{"10.2.1"};
  std::vector<std::string> arch{"arm-none-eabi"};
  buildcc::env::m::CommandExpect_Execute(1, true, &compiler_version, nullptr);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch, nullptr);

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_EQUAL(verified_toolchains.size(), 0);
}

TEST(ToolchainTestGroup, VerifyToolchain_ConditionalAdd_BothFailure) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  buildcc::base::VerifyToolchainConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.push_back(
      (fs::current_path() / "toolchains" / "gcc").string());
  config.compiler_version = "none";
  config.target_arch = "none";

  std::vector<std::string> compiler_version{"10.2.1"};
  std::vector<std::string> arch{"arm-none-eabi"};
  buildcc::env::m::CommandExpect_Execute(1, true, &compiler_version, nullptr);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch, nullptr);

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify(config);
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_EQUAL(verified_toolchains.size(), 0);
}

int main(int ac, char **av) {
  buildcc::env::m::VectorStringCopier copier;
  mock().installCopier(TEST_VECTOR_STRING_TYPE, copier);

  // NOTE, Check the GCC, MSVC and Clang compilers
  // Create directory and populate it with gcc and cl executables
  // Linux
  // toolchains/gcc
  // toolchains/clang

  // Windows
  // toolchains/msvc
  // toolchains/mingw
  // TODO, Check executables used in clang

  return CommandLineTestRunner::RunAllTests(ac, av);
}
