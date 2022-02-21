#include "toolchain/common/toolchain_config.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(ToolchainConfigTestGroup)
{
};
// clang-format on

TEST(ToolchainConfigTestGroup, GetFileExt) {
  buildcc::ToolchainConfig toolchain_config;

  buildcc::FileExt ext;

  ext = toolchain_config.GetFileExt("file.asm");
  CHECK(ext == buildcc::FileExt::Asm);

  ext = toolchain_config.GetFileExt("file.c");
  CHECK(ext == buildcc::FileExt::C);

  ext = toolchain_config.GetFileExt("file.cpp");
  CHECK(ext == buildcc::FileExt::Cpp);

  ext = toolchain_config.GetFileExt("file.h");
  CHECK(ext == buildcc::FileExt::Header);

  ext = toolchain_config.GetFileExt("file.invalid");
  CHECK(ext == buildcc::FileExt::Invalid);

  ext = toolchain_config.GetFileExt("random/directory");
  CHECK(ext == buildcc::FileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
