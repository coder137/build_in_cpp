#include "toolchain/toolchain.h"

#include "target/target_info.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(ToolchainFlagApiTestGroup)
{
};
// clang-format on

TEST(ToolchainFlagApiTestGroup, BasicToolchainTest) {
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc",
                               "g++", "ar", "ld");
  toolchain.AddPreprocessorFlag("-preprocessor");
  toolchain.AddAsmCompileFlag("-asm");
  toolchain.AddPchCompileFlag("-pchcompile");
  toolchain.AddPchObjectFlag("-pchobject");
  toolchain.AddCommonCompileFlag("-common");
  toolchain.AddCCompileFlag("-c");
  toolchain.AddCppCompileFlag("-cpp");
  toolchain.AddLinkFlag("-link");
  CHECK_FALSE(toolchain.GetLockInfo().IsLocked());
  toolchain.Lock();
  CHECK_TRUE(toolchain.GetLockInfo().IsLocked());

  CHECK_EQUAL(toolchain.GetPreprocessorFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetAsmCompileFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetPchCompileFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetPchObjectFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetCommonCompileFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetCCompileFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetCppCompileFlags().size(), 1);
  CHECK_EQUAL(toolchain.GetLinkFlags().size(), 1);
}

TEST(ToolchainFlagApiTestGroup, BasicTargetTest) {
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc",
                               "g++", "ar", "ld");

  toolchain.AddPreprocessorFlag("-preprocessor");
  toolchain.AddAsmCompileFlag("-asm");
  toolchain.AddPchCompileFlag("-pchcompile");
  toolchain.AddPchObjectFlag("-pchobject");
  toolchain.AddCommonCompileFlag("-common");
  toolchain.AddCCompileFlag("-c");
  toolchain.AddCppCompileFlag("-cpp");
  toolchain.AddLinkFlag("-link");

  CHECK_FALSE(toolchain.GetLockInfo().IsLocked());
  { CHECK_THROWS(std::exception, (buildcc::TargetInfo(toolchain, ""))); }

  toolchain.Lock();
  CHECK_TRUE(toolchain.GetLockInfo().IsLocked());
  {
    buildcc::TargetInfo targetinfo(toolchain, "");
    CHECK_EQUAL(targetinfo.GetPreprocessorFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetAsmCompileFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetPchCompileFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetPchObjectFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetCommonCompileFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetCCompileFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetCppCompileFlags().size(), 1);
    CHECK_EQUAL(targetinfo.GetLinkFlags().size(), 1);
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
