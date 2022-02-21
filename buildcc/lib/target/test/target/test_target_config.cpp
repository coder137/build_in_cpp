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

  buildcc::FileExt ext;

  ext = target_config.GetFileExt("file.asm");
  CHECK(ext == buildcc::FileExt::Asm);

  ext = target_config.GetFileExt("file.c");
  CHECK(ext == buildcc::FileExt::C);

  ext = target_config.GetFileExt("file.cpp");
  CHECK(ext == buildcc::FileExt::Cpp);

  ext = target_config.GetFileExt("file.h");
  CHECK(ext == buildcc::FileExt::Header);

  ext = target_config.GetFileExt("file.invalid");
  CHECK(ext == buildcc::FileExt::Invalid);

  ext = target_config.GetFileExt("random/directory");
  CHECK(ext == buildcc::FileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
