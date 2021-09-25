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
  }

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
  }

  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_Build) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::Args args;
  buildcc::Args::ToolchainArg gcc_toolchain;
  buildcc::Args::ToolchainArg msvc_toolchain;
  args.AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain);
  args.AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain);
  args.Parse(argc, av.data());

  STRCMP_EQUAL(args.GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(args.GetProjectBuildDir().string().c_str(), "build");
  CHECK(args.GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(args.Clean());

  // Make dummy toolchain and target
  buildcc::env::init(fs::current_path(), fs::current_path());
  buildcc::base::Toolchain toolchain(buildcc::base::Toolchain::Id::Gcc, "", "",
                                     "", "", "", "");
  buildcc::base::Target target("dummyT", buildcc::base::TargetType::Executable,
                               toolchain, "");

  {
    buildcc::Args::ToolchainState state{false, false};

    buildcc::Register reg(args);
    reg.Build(state, target,
              [](buildcc::base::Target &target) { (void)target; });
  }

  {
    buildcc::Args::ToolchainState state{true, true};

    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(state, target,
              [](buildcc::base::Target &target) { (void)target; });
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
