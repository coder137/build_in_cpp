#include "args/args.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(ArgsTestGroup)
{
};
// clang-format on

TEST(ArgsTestGroup, Args_BasicParse) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
  int argc = av.size();

  buildcc::Args args;
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());
}

TEST(ArgsTestGroup, Args_BasicExit) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--help"};
  int argc = av.size();

  buildcc::Args args;
  CHECK_THROWS(std::exception, args.Parse(argc, av.data()));
}

TEST(ArgsTestGroup, Args_MultiToml) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/no_clean.toml"};
  int argc = av.size();

  buildcc::Args args;
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_FALSE(args.Clean());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
