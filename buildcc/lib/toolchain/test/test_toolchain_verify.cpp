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
                const std::string &assembler, const std::string &c_compiler,
                const std::string &cpp_compiler, const std::string &archiver,
                const std::string &linker)
      : buildcc::Toolchain(id, name, assembler, c_compiler, cpp_compiler,
                           archiver, linker) {}

private:
  // Example implementation
  std::optional<buildcc::ToolchainCompilerInfo> GetToolchainInfo(
      const buildcc::ToolchainExecutables &executables) const override {
    (void)executables;
    std::vector<std::string> version_stdout;
    std::vector<std::string> arch_stdout;
    bool version_done = buildcc::env::Command::Execute("", {}, &version_stdout);
    bool arch_done = buildcc::env::Command::Execute("", {}, &arch_stdout);
    if (!version_done || !arch_done || version_stdout.empty() ||
        arch_stdout.empty()) {
      return {};
    }
    buildcc::ToolchainCompilerInfo info;
    info.compiler_version = version_stdout[0];
    info.target_arch = arch_stdout[0];
    return info;
  }
};

// NOTE, We are mocking the environment instead of actually querying it
TEST(ToolchainVerifyTestGroup, VerifyToolchain_BaseToolchain_Failure) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
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

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

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

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc_CompilerVersionFailure) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, false, &version_stdout_data);
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

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc_CompilerVersionEmpty) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

  std::vector<std::string> version_stdout_data;
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

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc_TargetArchFailure) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::env::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::env::m::CommandExpect_Execute(1, false, &arch_stdout_data);

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

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Gcc_TargetArchEmpty) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data;
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

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_BadAbsolutePath) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

  buildcc::ToolchainVerifyConfig config;
  config.env_vars.clear();
  config.absolute_search_paths.insert((fs::current_path() / "does_not_exist"));

  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_PathContainsDir) {
  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

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

  MockToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                    "ld");

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
