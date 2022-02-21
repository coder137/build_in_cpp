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

  CHECK_FALSE(target_state.contains_c);
  target_state.SetSourceState(buildcc::FileExt::C);
  CHECK_TRUE(target_state.contains_c);

  CHECK_FALSE(target_state.contains_cpp);
  target_state.SetSourceState(buildcc::FileExt::Cpp);
  CHECK_TRUE(target_state.contains_cpp);

  CHECK_FALSE(target_state.contains_asm);
  target_state.SetSourceState(buildcc::FileExt::Asm);
  CHECK_TRUE(target_state.contains_asm);

  // Ignored
  target_state.SetSourceState(buildcc::FileExt::Header);
  target_state.SetSourceState(buildcc::FileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
