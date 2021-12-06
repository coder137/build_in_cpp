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

TEST(ToolchainTestGroup, VerifyToolchain) {
  buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc, "gcc", "as",
                               "gcc", "g++", "ar", "ld");

  std::vector<std::string> version_stdout_data{"version"};
  std::vector<std::string> arch_stdout_data{"arch"};
  buildcc::m::CommandExpect_Execute(1, true, &version_stdout_data);
  buildcc::m::CommandExpect_Execute(1, true, &arch_stdout_data);

  // On linux we have the path symlinked to /usr/bin and /usr dirs
  if constexpr (buildcc::env::is_linux()) {
    buildcc::m::CommandExpect_Execute(1, true, &version_stdout_data);
    buildcc::m::CommandExpect_Execute(1, true, &arch_stdout_data);
  }

  std::vector<buildcc::base::VerifiedToolchain> verified_toolchains =
      gcc.Verify();
  UT_PRINT(std::to_string(verified_toolchains.size()).c_str());
  CHECK_TRUE(!verified_toolchains.empty());
  STRCMP_EQUAL(verified_toolchains[0].compiler_version.c_str(), "version");
  STRCMP_EQUAL(verified_toolchains[0].target_arch.c_str(), "arch");
}

int main(int ac, char **av) {
  buildcc::m::VectorStringCopier copier;
  mock().installCopier(TEST_VECTOR_STRING_TYPE, copier);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
