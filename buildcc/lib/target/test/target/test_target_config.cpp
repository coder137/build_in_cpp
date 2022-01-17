#include "target/common/target_config.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetConfigTestGroup)
{
};
// clang-format on

TEST(TargetConfigTestGroup, GetFileExt) {
  buildcc::TargetConfig target_config;

  buildcc::TargetFileExt ext;

  ext = target_config.GetFileExt("file.asm");
  CHECK(ext == buildcc::TargetFileExt::Asm);

  ext = target_config.GetFileExt("file.c");
  CHECK(ext == buildcc::TargetFileExt::C);

  ext = target_config.GetFileExt("file.cpp");
  CHECK(ext == buildcc::TargetFileExt::Cpp);

  ext = target_config.GetFileExt("file.h");
  CHECK(ext == buildcc::TargetFileExt::Header);

  ext = target_config.GetFileExt("file.invalid");
  CHECK(ext == buildcc::TargetFileExt::Invalid);

  ext = target_config.GetFileExt("random/directory");
  CHECK(ext == buildcc::TargetFileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
