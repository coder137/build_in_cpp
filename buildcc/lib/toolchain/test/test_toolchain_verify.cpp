#include <cstdlib>

#include "toolchain/toolchain.h"

#include "env/command.h"
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

class MockToolchain : public buildcc::Toolchain {
public:
  MockToolchain(buildcc::ToolchainId id, const std::string &name,
                const buildcc::ToolchainExecutables &executables =
                    buildcc::ToolchainExecutables("as", "gcc", "g++", "ar",
                                                  "ld"))
      : buildcc::Toolchain(id, name, executables) {}
};

// NOTE, We are mocking the environment instead of actually querying it
TEST(ToolchainVerifyTestGroup, VerifyToolchain_BaseToolchain_Failure) {
  MockToolchain gcc(
      buildcc::ToolchainId::Gcc, "gcc",
      buildcc::ToolchainExecutables("as", "gcc", "g++", "ar", "ld"));

  std::string putenv_str =
      fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_BadAbsolutePath) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc");

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.insert((fs::current_path() / "does_not_exist"));

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_PathContainsDir) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc");

  buildcc::ToolchainFindConfig config;
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

  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc");

  buildcc::ToolchainFindConfig config;
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

TEST(ToolchainVerifyTestGroup, CustomToolchainInfo) {
  buildcc::Toolchain toolchain(
      buildcc::ToolchainId::Gcc, "gcc",
      buildcc::ToolchainExecutables("as", "gcc", "g++", "ar", "ld"));
  toolchain.SetToolchainInfoCb(
      [](const buildcc::ToolchainExecutables &executables)
          -> std::optional<buildcc::ToolchainCompilerInfo> {
        (void)executables;
        mock().actualCall("SetToolchainInfoCb");
        buildcc::ToolchainCompilerInfo info;
        info.compiler_version = "version";
        info.target_arch = "arch";
        return info;
      });

  std::string putenv_str =
      fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc", fs::current_path());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");

  mock().expectOneCall("SetToolchainInfoCb");
  auto info = toolchain.Verify(config);
  STRCMP_EQUAL(info.compiler_version.c_str(), "version");
  STRCMP_EQUAL(info.target_arch.c_str(), "arch");
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
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
