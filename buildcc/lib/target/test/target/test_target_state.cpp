#include "target/common/target_state.h"
#include "toolchain/common/file_ext.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetStateTestGroup)
{
};
// clang-format on

TEST(TargetStateTestGroup, SetSourceState) {
  buildcc::TargetState target_state;

  CHECK_FALSE(target_state.ContainsC());
  target_state.SetSourceState(buildcc::FileExt::C);
  CHECK_TRUE(target_state.ContainsC());

  CHECK_FALSE(target_state.ContainsCpp());
  target_state.SetSourceState(buildcc::FileExt::Cpp);
  CHECK_TRUE(target_state.ContainsCpp());

  CHECK_FALSE(target_state.ContainsAsm());
  target_state.SetSourceState(buildcc::FileExt::Asm);
  CHECK_TRUE(target_state.ContainsAsm());

  // Ignored
  target_state.SetSourceState(buildcc::FileExt::Header);
  target_state.SetSourceState(buildcc::FileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
