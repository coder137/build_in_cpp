#include "args/register.h"

#include "expect_command.h"

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
    reg.Build(
        state, [](buildcc::base::Target &target) { (void)target; }, target);
  }

  {
    buildcc::Args::ToolchainState state{true, true};

    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        state, [](buildcc::base::Target &target) { (void)target; }, target);
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_NoBuildAndDep) {
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
  buildcc::base::Target dependency(
      "depT", buildcc::base::TargetType::Executable, toolchain, "");

  // 4 options
  // T -> Target
  // D -> Dep
  // T0D0 -> Throw
  // T0D1 -> Throw
  // T1D0 -> Throw
  // T1D1 -> This is the only condition for success
  // buildcc::Args::ToolchainState falseState{false, false};
  buildcc::Args::ToolchainState trueState{true, true};

  // T0D0
  {
    buildcc::Register reg(args);
    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T0D1
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T1D0
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T1D1
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_BuildAndDep) {
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
  buildcc::base::Target dependency(
      "depT", buildcc::base::TargetType::Executable, toolchain, "");

  // 4 options
  // T -> Target
  // D -> Dep
  // T0D0 -> Ignore
  // T0D1 -> Ignore
  // T1D0 -> Ignore
  // T1D1 -> This is the only condition for success
  buildcc::Args::ToolchainState falseState{false, false};
  buildcc::Args::ToolchainState trueState{true, true};

  // T0D0
  {
    buildcc::Register reg(args);
    reg.Build(
        falseState, [](buildcc::base::Target &target) { (void)target; },
        target);
    reg.Build(
        falseState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T0D1
  {
    buildcc::Register reg(args);
    reg.Build(
        falseState, [](buildcc::base::Target &target) { (void)target; },
        target);
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T1D0
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        falseState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T1D1
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_DepDuplicate) {
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
  buildcc::base::Target dependency(
      "depT", buildcc::base::TargetType::Executable, toolchain, "");
  buildcc::base::Target dependency2(
      "dep2T", buildcc::base::TargetType::Executable, toolchain, "");

  buildcc::Args::ToolchainState trueState{true, true};

  // Duplicate dependency with 2 Targets
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency2);

    reg.Dep(dependency, dependency2);
    reg.Dep(target, dependency);
    reg.Dep(target, dependency2);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
    CHECK_THROWS(std::exception, reg.Dep(target, dependency2));
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_DepCyclic) {
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
  buildcc::base::Target dependency(
      "depT", buildcc::base::TargetType::Executable, toolchain, "");
  buildcc::base::Target dependency2(
      "dep2T", buildcc::base::TargetType::Executable, toolchain, "");

  buildcc::Args::ToolchainState trueState{true, true};

  // Immediate cyclic depdendency
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
    CHECK_THROWS(std::exception, reg.Dep(dependency, target));
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency);
    reg.Build(
        trueState, [](buildcc::base::Target &target) { (void)target; },
        dependency2);

    reg.Dep(dependency, dependency2);
    reg.Dep(target, dependency);

    // dependency2 -> dependency -> target -> dependency2
    CHECK_THROWS(std::exception, reg.Dep(dependency2, target));
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_Test) {
  // Arguments
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
  buildcc::base::Target dependency(
      "depT", buildcc::base::TargetType::Executable, toolchain, "");

  // 4 states between build and test
  // FF
  // TF
  // FT
  // TT -> only success case
  buildcc::Args::ToolchainState stateFail{false, false};
  buildcc::Args::ToolchainState state1{true, false};
  buildcc::Args::ToolchainState state2{false, true};
  buildcc::Args::ToolchainState stateSuccess{true, true};

  // FF
  {
    buildcc::Register reg(args);
    reg.Test(stateFail, "{executable}", target);
  }

  // TF
  {
    buildcc::Register reg(args);
    reg.Test(state1, "{executable}", target);
  }

  // FT
  {
    buildcc::Register reg(args);
    reg.Test(state2, "{executable}", target);
  }

  // TT
  // Register::Build not called
  {
    buildcc::Register reg(args);
    CHECK_THROWS(std::exception,
                 reg.Test(stateSuccess, "{executable}", target));
  }

  // Correct Usage
  {
    buildcc::Register reg(args);
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::base::Target &target) { (void)target; },
        target);
    reg.Test(stateSuccess, "{executable}", target);

    buildcc::m::CommandExpect_Execute(1, true);
    reg.RunTest();
  }

  buildcc::env::deinit();
  mock().checkExpectations();
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
