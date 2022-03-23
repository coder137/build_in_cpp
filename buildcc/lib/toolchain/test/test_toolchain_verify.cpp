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
TEST_GROUP(ToolchainVerifyTestGroup)
{
  void teardown() {
    mock().checkExpectations();
    mock().clear();
  }
};
// clang-format on

// NOTE, We are mocking the environment instead of actually querying it
TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout_data);

  std::string putenv_str =
      fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  buildcc::ToolchainCompilerInfo compiler_info = gcc.Verify(config);

  STRCMP_EQUAL(compiler_info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(compiler_info.target_arch.c_str(), "arch");
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Clang) {
  buildcc::Toolchain clang(buildcc::ToolchainId::Clang, "clang", "llvm-as",
                           "clang", "clang++", "llvm-ar", "lld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::env::m::CommandExpect_Execute(1, true, &arch_stdout_data);

  std::string putenv_str = fmt::format(
      "CUSTOM_BUILDCC_PATH={}/toolchains/clang", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  buildcc::ToolchainCompilerInfo compiler_info = clang.Verify(config);

  STRCMP_EQUAL(compiler_info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(compiler_info.target_arch.c_str(), "arch");
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Msvc) {
  buildcc::Toolchain msvc(buildcc::ToolchainId::Msvc, "msvc", "cl", "cl", "cl",
                          "lib", "link");
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
  std::string putenv_str =
      fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/msvc", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  buildcc::ToolchainCompilerInfo compiler_info = msvc.Verify(config);

  STRCMP_EQUAL(compiler_info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(compiler_info.target_arch.c_str(), "host_arch_tgt_arch");
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_BadCompilerId) {
  buildcc::Toolchain gcc((buildcc::ToolchainId)65535, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  std::string putenv_str =
      fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_BadAbsolutePath) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.insert((fs::current_path() / "does_not_exist"));

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_PathContainsDir) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.insert((fs::current_path() / "toolchains"));

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__MINGW64__)

TEST(ToolchainVerifyTestGroup, VerifyToolchain_LockedFolder) {
  std::error_code err;
  fs::permissions(fs::current_path() / "toolchains" / "gcc", fs::perms::none,
                  err);
  if (err) {
    FAIL_TEST("Could not set file permissions");
  }

  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.insert(
      (fs::current_path() / "toolchains" / "gcc"));

  CHECK_THROWS(std::exception, gcc.Verify(config));

  fs::permissions(fs::current_path() / "toolchains" / "gcc", fs::perms::all,
                  err);
  if (err) {
    FAIL_TEST("Could not set file permissions");
  }
}

#endif

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
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
