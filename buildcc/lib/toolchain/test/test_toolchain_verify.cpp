#include <cstdlib>

#include "toolchain/toolchain.h"

#include "env/host_os.h"
#include "env/host_os_util.h"

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

class TestToolchain : public buildcc::Toolchain {
public:
  TestToolchain()
      : Toolchain(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++", "ar",
                  "ld") {}

private:
  virtual std::optional<buildcc::ToolchainCompilerInfo>
  VerifySelectedToolchainPath(
      const fs::path &selected_path,
      const buildcc::ToolchainFindConfig &verify_config) const override {
    (void)selected_path;
    (void)verify_config;
    mock().actualCall("VerifySelectedToolchainPath").onObject(this);
    if (!mock().getData("success").getBoolValue()) {
      return {};
    }

    buildcc::ToolchainCompilerInfo compiler_info;
    compiler_info.path = mock().getData("path").getStringValue();
    compiler_info.target_arch = mock().getData("target_arch").getStringValue();
    compiler_info.compiler_version =
        mock().getData("compiler_version").getStringValue();
    return compiler_info;
  }
};

// NOTE, We are mocking the environment instead of actually querying it
TEST(ToolchainVerifyTestGroup, VerifyToolchain_Success) {
  TestToolchain gcc;
  std::string putenv_str = fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc",
                                       fs::current_path().string());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  // UT_PRINT(custom_buildcc_path);

  mock().expectOneCall("VerifySelectedToolchainPath").onObject(&gcc);
  mock().setData("success", true);
  mock().setData("path", "dummy/path");
  mock().setData("compiler_version", "version");
  mock().setData("target_arch", "arch");

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");
  buildcc::ToolchainCompilerInfo verified_toolchain = gcc.Verify(config);

  // CHECK_TRUE(!verified_toolchain.empty());
  STRCMP_EQUAL(verified_toolchain.compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchain.target_arch.c_str(), "arch");
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_Failure) {
  TestToolchain gcc;
  std::string putenv_str = fmt::format("CUSTOM_BUILDCC_PATH={}/toolchains/gcc",
                                       fs::current_path().string());
  int put = putenv(putenv_str.data());
  CHECK_TRUE(put == 0);
  const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
  CHECK_TRUE(custom_buildcc_path != nullptr);
  UT_PRINT(custom_buildcc_path);

  mock().expectOneCall("VerifySelectedToolchainPath").onObject(&gcc);
  mock().setData("success", false);
  mock().setData("path", "dummy/path");
  mock().setData("compiler_version", "version");
  mock().setData("target_arch", "arch");

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");
  CHECK_THROWS(std::exception, gcc.Verify(config));
}

TEST(ToolchainVerifyTestGroup, VerifyToolchain_FailureBaseToolchain) {
  buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                         "ar", "ld");
  {
    std::string putenv_str = fmt::format(
        "CUSTOM_BUILDCC_PATH={}/toolchains/gcc", fs::current_path().string());
    int put = putenv(putenv_str.data());
    CHECK_TRUE(put == 0);
    const char *custom_buildcc_path = getenv("CUSTOM_BUILDCC_PATH");
    CHECK_TRUE(custom_buildcc_path != nullptr);
    UT_PRINT(custom_buildcc_path);
  }

  // mock().expectOneCall("VerifySelectedToolchainPath").onObject(&gcc);
  // mock().setData("success", false);
  // mock().setData("path", "dummy/path");
  // mock().setData("compiler_version", "version");
  // mock().setData("target_arch", "arch");

  buildcc::ToolchainFindConfig config;
  config.env_vars.clear();
  config.env_vars.insert("CUSTOM_BUILDCC_PATH");
  CHECK_THROWS(std::exception, gcc.Verify(config));
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
