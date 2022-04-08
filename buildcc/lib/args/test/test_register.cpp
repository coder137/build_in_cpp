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
    buildcc::Reg::Deinit();
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

  buildcc::Reg::Init();
  buildcc::Reg::Init(); // Second init does nothing
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

    buildcc::Reg::Init();
    mock().expectOneCall("CleanCb");
    buildcc::Reg::Call(buildcc::Args::Clean()).Func([]() {
      mock().actualCall("CleanCb");
    });
    buildcc::Reg::Deinit();
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

    buildcc::Reg::Init();
    buildcc::Reg::Call(buildcc::Args::Clean()).Func([]() {
      mock().actualCall("CleanCb");
    });
    buildcc::Reg::Deinit();
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

    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(state).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    CHECK_TRUE(!buildcc::Reg::GetTaskflow().empty());
    buildcc::Reg::Deinit();
    CHECK_THROWS(std::exception, buildcc::Reg::GetTaskflow());
  }

  {
    buildcc::ArgToolchainState state{true, true};

    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(state).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // T0D1
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_depT");
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency);

    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // T1D0
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);

    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // T1D1
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency);

    buildcc::Reg::Toolchain(trueState).Dep(target, dependency);
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(falseState)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, dependency)
        .Dep(target, dependency);
    buildcc::Reg::Deinit();
  }

  // T0D1
  {
    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(falseState)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    mock().expectNCalls(1, "BuildTask_depT");
    // In this case, target is not built so Dep throws
    // Bad usage
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState)
                     .Build([](buildcc::BaseTarget &target) { (void)target; },
                            dependency)
                     .Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // T1D0
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(falseState)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, dependency);

    // In this case dependency is not built
    // Bad usage
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // T1D1
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    buildcc::Reg::Toolchain(trueState)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, dependency)
        .Dep(target, dependency);
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    buildcc::Reg::Toolchain(trueState)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, dependency)
        .Dep(target, dependency);
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    buildcc::Reg::Deinit();
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency2);

    buildcc::Reg::Toolchain(trueState).Dep(dependency, dependency2);
    buildcc::Reg::Toolchain(trueState).Dep(target, dependency);
    buildcc::Reg::Toolchain(trueState).Dep(target, dependency2);

    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency));
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(target, dependency2));
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency);

    buildcc::Reg::Toolchain(trueState).Dep(target, dependency);
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(dependency, target));
    buildcc::Reg::Deinit();
  }

  // Duplicate dependency with 3 Targets
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    mock().expectNCalls(1, "BuildTask_depT");
    mock().expectNCalls(1, "BuildTask_dep2T");

    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency);
    buildcc::Reg::Toolchain(trueState).Build(
        [](buildcc::BaseTarget &target) { (void)target; }, dependency2);

    buildcc::Reg::Toolchain(trueState).Dep(dependency, dependency2);
    buildcc::Reg::Toolchain(trueState).Dep(target, dependency);

    // dependency2 -> dependency -> target -> dependency2
    CHECK_THROWS(std::exception,
                 buildcc::Reg::Toolchain(trueState).Dep(dependency2, target));
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(stateFail).Test("{executable}", target);
    buildcc::Reg::Deinit();
  }

  // TF
  {
    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(state1).Test("{executable}", target);
    buildcc::Reg::Deinit();
  }

  // FT
  {
    buildcc::Reg::Init();
    buildcc::Reg::Toolchain(state2).Test("{executable}", target);
    buildcc::Reg::Deinit();
  }

  // TT
  // Reg::Instance::Build not called
  {
    buildcc::Reg::Init();
    CHECK_THROWS(
        std::exception,
        buildcc::Reg::Toolchain(stateSuccess).Test("{executable}", target));
    buildcc::Reg::Deinit();
  }

  // Correct Usage
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess).Test("{executable}", target);

    std::vector<std::string> stdout_data;
    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, &stderr_data);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
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
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test("{executable}", target,
              buildcc::TestConfig(
                  {}, {},
                  buildcc::TestOutput(
                      buildcc::TestOutput::Type::DefaultBehaviour)));

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
  }

  // TestOutput::Type::TestPrintOnStderr
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test("{executable}", target,
              buildcc::TestConfig(
                  {}, {},
                  buildcc::TestOutput(
                      buildcc::TestOutput::Type::TestPrintOnStderr)));

    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, nullptr, &stderr_data);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
  }

  // TestOutput::Type::TestPrintOnStdout
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test("{executable}", target,
              buildcc::TestConfig(
                  {}, {},
                  buildcc::TestOutput(
                      buildcc::TestOutput::Type::TestPrintOnStdout)));

    std::vector<std::string> stdout_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, nullptr);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
  }

  // TestOutput::Type::TestPrintOnStderrAndStdout
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test("{executable}", target,
              buildcc::TestConfig(
                  {}, {},
                  buildcc::TestOutput(
                      buildcc::TestOutput::Type::TestPrintOnStderrAndStdout)));

    std::vector<std::string> stdout_data;
    std::vector<std::string> stderr_data;
    buildcc::env::m::CommandExpect_Execute(1, true, &stdout_data, &stderr_data);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
  }

  // TestOutput::Type::UserRedirect
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test("{executable}", target,
              buildcc::TestConfig(
                  {}, {},
                  buildcc::TestOutput(buildcc::TestOutput::Type::UserRedirect,
                                      nullptr, nullptr)));

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::Reg::Run();
    buildcc::Reg::Deinit();
  }

  // TestOutput::Type::UserRedirect
  {
    buildcc::Reg::Init();
    mock().expectNCalls(1, "BuildTask_dummyT");
    buildcc::Reg::Toolchain(stateSuccess)
        .Build([](buildcc::BaseTarget &target) { (void)target; }, target);
    buildcc::Reg::Toolchain(stateSuccess)
        .Test(
            "{executable}", target,
            buildcc::TestConfig(
                {}, {}, buildcc::TestOutput(buildcc::TestOutput::Type(65535))));
    CHECK_THROWS(std::exception, buildcc::Reg::Run());
    buildcc::Reg::Deinit();
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
