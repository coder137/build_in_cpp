#include "args/register.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(RegisterTestGroup)
{
    void teardown() {
        mock().clear();
    }
};
// clang-format on

TEST(RegisterTestGroup, Register_Initialize) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
  int argc = av.size();

  buildcc::Args args;
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  buildcc::Register reg(args);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
