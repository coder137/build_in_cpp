#include <cstdlib>

#include "toolchain/common/toolchain_id.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(ToolchainIdTestGroup)
{
  void teardown() {
    mock().checkExpectations();
    mock().clear();
  }
};
// clang-format on

TEST(ToolchainIdTestGroup, ToolchainIdAsString) {
  std::string compiler;

  compiler = fmt::format("{}", buildcc::ToolchainId::Gcc);
  STRCMP_EQUAL(compiler.c_str(), "Gcc");

  compiler = fmt::format("{}", buildcc::ToolchainId::Msvc);
  STRCMP_EQUAL(compiler.c_str(), "Msvc");

  compiler = fmt::format("{}", buildcc::ToolchainId::Clang);
  STRCMP_EQUAL(compiler.c_str(), "Clang");

  compiler = fmt::format("{}", buildcc::ToolchainId::MinGW);
  STRCMP_EQUAL(compiler.c_str(), "MinGW");

  compiler = fmt::format("{}", buildcc::ToolchainId::Custom);
  STRCMP_EQUAL(compiler.c_str(), "Custom");

  compiler = fmt::format("{}", buildcc::ToolchainId::Undefined);
  STRCMP_EQUAL(compiler.c_str(), "Undefined");

  compiler = fmt::format("{}", (buildcc::ToolchainId)65535);
  STRCMP_EQUAL(compiler.c_str(), "Undefined");
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
