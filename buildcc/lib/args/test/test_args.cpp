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

TEST(ArgsTestGroup, Args_CustomToolchain) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml",
                               "--config", "configs/gcc_toolchain.toml"};
  int argc = av.size();

  buildcc::Args args;
  buildcc::Args::ToolchainArg gcc_toolchain;
  args.AddCustomToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // Toolchain
  CHECK_TRUE(gcc_toolchain.state.build);
  CHECK_FALSE(gcc_toolchain.state.test);
  CHECK(gcc_toolchain.id == buildcc::base::Toolchain::Id::Gcc);
  STRCMP_EQUAL(gcc_toolchain.name.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.asm_compiler.c_str(), "as");
  STRCMP_EQUAL(gcc_toolchain.c_compiler.c_str(), "gcc");
  STRCMP_EQUAL(gcc_toolchain.cpp_compiler.c_str(), "g++");
  STRCMP_EQUAL(gcc_toolchain.archiver.c_str(), "ar");
  STRCMP_EQUAL(gcc_toolchain.linker.c_str(), "ld");
}

TEST(ArgsTestGroup, Args_MultipleCustomToolchain) {}

TEST(ArgsTestGroup, Args_DuplicateCustomToolchain) {}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
