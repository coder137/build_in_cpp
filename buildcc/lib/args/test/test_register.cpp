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

TEST(RegisterTestGroup, Register_Clean) {
  {
    std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
    int argc = av.size();

    buildcc::Args args;
    args.Parse(argc, av.data());

    STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
    STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
    CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
    CHECK_TRUE(args.Clean());

    buildcc::Register reg(args);
    mock().expectOneCall("CleanCb");
    reg.Clean([]() { mock().actualCall("CleanCb"); });
    mock().checkExpectations();
  }

  {
    {
      std::vector<const char *> av{
          "",
          "--config",
          "configs/basic_parse.toml",
          "--config",
          "configs/no_clean.toml",
      };
      int argc = av.size();

      buildcc::Args args;
      args.Parse(argc, av.data());

      STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
      STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
      CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
      CHECK_FALSE(args.Clean());

      buildcc::Register reg(args);
      reg.Clean([]() { mock().actualCall("CleanCb"); });
      mock().checkExpectations();
    }
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
