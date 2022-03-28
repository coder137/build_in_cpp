#include "args/register.h"

#include "expect_command.h"

#include "mock_command_copier.h"

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
    buildcc::Args::Deinit();
    mock().clear();
  }
};
// clang-format on

TEST(RegisterTestGroup, Register_Initialize) {
  std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
  int argc = av.size();

  buildcc::Args::Init().Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  buildcc::Register reg;
}

TEST(RegisterTestGroup, Register_Clean) {
  {
    std::vector<const char *> av{"", "--config", "configs/basic_parse.toml"};
    int argc = av.size();

    buildcc::Args::Init().Parse(argc, av.data());

    STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
    STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
    CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
    CHECK_TRUE(buildcc::Args::Clean());

    buildcc::Register reg;
    mock().expectOneCall("CleanCb");
    reg.Clean([]() { mock().actualCall("CleanCb"); });
    buildcc::Args::Deinit();
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

    buildcc::Args::Init().Parse(argc, av.data());

    STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
    STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
    CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
    CHECK_FALSE(buildcc::Args::Clean());

    buildcc::Register reg;
    reg.Clean([]() { mock().actualCall("CleanCb"); });
    buildcc::Args::Deinit();
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

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");

  {
    buildcc::ArgToolchainState state{false, false};

    buildcc::Register reg;
    reg.Build(
        state, [](buildcc::BaseTarget &target) { (void)target; }, target);
  }

  {
    buildcc::ArgToolchainState state{true, true};

    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        state, [](buildcc::BaseTarget &target) { (void)target; }, target);
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_NoBuildAndDep) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");

  // 4 options
  // T -> Target
  // D -> Dep
  // T0D0 -> Throw
  // T0D1 -> Throw
  // T1D0 -> Throw
  // T1D1 -> This is the only condition for success
  // buildcc::ArgToolchainState falseState{false, false};
  buildcc::ArgToolchainState trueState{true, true};

  // T0D0
  {
    buildcc::Register reg;
    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T0D1
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T1D0
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // T1D1
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_BuildAndDep) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");

  // 4 options
  // T -> Target
  // D -> Dep
  // T0D0 -> Ignore
  // T0D1 -> Ignore
  // T1D0 -> Ignore
  // T1D1 -> This is the only condition for success
  buildcc::ArgToolchainState falseState{false, false};
  buildcc::ArgToolchainState trueState{true, true};

  // T0D0
  {
    buildcc::Register reg;
    reg.Build(
        falseState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        falseState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T0D1
  {
    buildcc::Register reg;
    reg.Build(
        falseState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T1D0
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        falseState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  // T1D1
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_DepDuplicate) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");
  buildcc::BaseTarget dependency2("dep2T", buildcc::TargetType::Executable,
                                  toolchain, "");

  buildcc::ArgToolchainState trueState{true, true};

  // Duplicate dependency with 2 Targets
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency2);

    reg.Dep(dependency, dependency2);
    reg.Dep(target, dependency);
    reg.Dep(target, dependency2);

    CHECK_THROWS(std::exception, reg.Dep(target, dependency));
    CHECK_THROWS(std::exception, reg.Dep(target, dependency2));
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_DepCyclic) {
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");
  buildcc::BaseTarget dependency2("dep2T", buildcc::TargetType::Executable,
                                  toolchain, "");

  buildcc::ArgToolchainState trueState{true, true};

  // Immediate cyclic depdendency
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);

    reg.Dep(target, dependency);
    CHECK_THROWS(std::exception, reg.Dep(dependency, target));
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; }, target);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency);
    reg.Build(
        trueState, [](buildcc::BaseTarget &target) { (void)target; },
        dependency2);

    reg.Dep(dependency, dependency2);
    reg.Dep(target, dependency);

    // dependency2 -> dependency -> target -> dependency2
    CHECK_THROWS(std::exception, reg.Dep(dependency2, target));
  }

  buildcc::Project::Deinit();
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

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");

  // 4 states between build and test
  // FF
  // TF
  // FT
  // TT -> only success case
  buildcc::ArgToolchainState stateFail{false, false};
  buildcc::ArgToolchainState state1{true, false};
  buildcc::ArgToolchainState state2{false, true};
  buildcc::ArgToolchainState stateSuccess{true, true};

  // FF
  {
    buildcc::Register reg;
    reg.Test(stateFail, "{executable}", target);
  }

  // TF
  {
    buildcc::Register reg;
    reg.Test(state1, "{executable}", target);
  }

  // FT
  {
    buildcc::Register reg;
    reg.Test(state2, "{executable}", target);
  }

  // TT
  // Register::Build not called
  {
    buildcc::Register reg;
    CHECK_THROWS(std::exception,
                 reg.Test(stateSuccess, "{executable}", target));
  }

  // Correct Usage
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(stateSuccess, "{executable}", target);

    std::vector<std::string> stdout_data;
    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, &stderr_data);
    reg.RunTest();
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

TEST(RegisterTestGroup, Register_TestWithOutput) {
  // Arguments
  std::vector<const char *> av{
      "",
      "--config",
      "configs/basic_parse.toml",
  };
  int argc = av.size();

  buildcc::ArgToolchain gcc_toolchain;
  buildcc::ArgToolchain msvc_toolchain;
  buildcc::Args::Init()
      .AddToolchain("gcc", "Generic gcc toolchain", gcc_toolchain)
      .AddToolchain("msvc", "Generic msvc toolchain", msvc_toolchain)
      .Parse(argc, av.data());

  STRCMP_EQUAL(buildcc::Args::GetProjectRootDir().string().c_str(), "root");
  STRCMP_EQUAL(buildcc::Args::GetProjectBuildDir().string().c_str(), "build");
  CHECK(buildcc::Args::GetLogLevel() == buildcc::env::LogLevel::Trace);
  CHECK_TRUE(buildcc::Args::Clean());

  // Make dummy toolchain and target
  buildcc::Project::Init(fs::current_path(), fs::current_path());
  buildcc::Toolchain toolchain(buildcc::ToolchainId::Gcc, "", "", "", "", "",
                               "");
  buildcc::BaseTarget target("dummyT", buildcc::TargetType::Executable,
                             toolchain, "");
  buildcc::BaseTarget dependency("depT", buildcc::TargetType::Executable,
                                 toolchain, "");

  buildcc::ArgToolchainState stateSuccess{true, true};

  // TestOutput::Type::DefaultBehaviour
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(
        stateSuccess, "{executable}", target,
        buildcc::TestConfig(
            {}, {},
            buildcc::TestOutput(buildcc::TestOutput::Type::DefaultBehaviour)));

    buildcc::env::m::CommandExpect_Execute(1, true);
    reg.RunTest();
  }

  // TestOutput::Type::TestPrintOnStderr
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(
        stateSuccess, "{executable}", target,
        buildcc::TestConfig(
            {}, {},
            buildcc::TestOutput(buildcc::TestOutput::Type::TestPrintOnStderr)));

    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, nullptr, &stderr_data);
    reg.RunTest();
  }

  // TestOutput::Type::TestPrintOnStdout
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(
        stateSuccess, "{executable}", target,
        buildcc::TestConfig(
            {}, {},
            buildcc::TestOutput(buildcc::TestOutput::Type::TestPrintOnStdout)));

    std::vector<std::string> stdout_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, nullptr);
    reg.RunTest();
  }

  // TestOutput::Type::TestPrintOnStderrAndStdout
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(stateSuccess, "{executable}", target,
             buildcc::TestConfig(
                 {}, {},
                 buildcc::TestOutput(
                     buildcc::TestOutput::Type::TestPrintOnStderrAndStdout)));

    std::vector<std::string> stdout_data;
    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, &stderr_data);
    reg.RunTest();
  }

  // TestOutput::Type::UserRedirect
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(stateSuccess, "{executable}", target,
             buildcc::TestConfig(
                 {}, {},
                 buildcc::TestOutput(buildcc::TestOutput::Type::UserRedirect,
                                     nullptr, nullptr)));

    buildcc::env::m::CommandExpect_Execute(1, true);
    reg.RunTest();
  }

  // TestOutput::Type::UserRedirect
  {
    buildcc::Register reg;
    mock().expectNCalls(1, "BuildTask_dummyT");
    reg.Build(
        stateSuccess, [](buildcc::BaseTarget &target) { (void)target; },
        target);
    reg.Test(
        stateSuccess, "{executable}", target,
        buildcc::TestConfig(
            {}, {}, buildcc::TestOutput(buildcc::TestOutput::Type(65535))));
    CHECK_THROWS(std::exception, reg.RunTest());
  }

  buildcc::Project::Deinit();
  mock().checkExpectations();
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::destroyGlobalDetector();
  buildcc::env::m::VectorStringCopier copier;
  mock().installCopier(TEST_VECTOR_STRING_TYPE, copier);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
