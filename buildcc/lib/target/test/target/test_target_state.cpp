#include "target/common/target_file_ext.h"
#include "target/common/target_state.h"

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
  buildcc::base::TargetState target_state;

  CHECK_FALSE(target_state.contains_c);
  target_state.SetSourceState(buildcc::TargetFileExt::C);
  CHECK_TRUE(target_state.contains_c);

  CHECK_FALSE(target_state.contains_cpp);
  target_state.SetSourceState(buildcc::TargetFileExt::Cpp);
  CHECK_TRUE(target_state.contains_cpp);

  CHECK_FALSE(target_state.contains_asm);
  target_state.SetSourceState(buildcc::TargetFileExt::Asm);
  CHECK_TRUE(target_state.contains_asm);

  // Ignored
  target_state.SetSourceState(buildcc::TargetFileExt::Header);
  target_state.SetSourceState(buildcc::TargetFileExt::Invalid);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
