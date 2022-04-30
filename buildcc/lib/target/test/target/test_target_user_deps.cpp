#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"
#include "test_target_util.h"

#include "target/target.h"

#include "env/env.h"
#include "env/util.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TargetTestUserDepsGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

static const fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_USER_DEPS_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestUserDepsGroup, Target_Build_CompileDeps_NoChange) {
  constexpr const char *NAME = "compileDep_NoChange.exe";
  buildcc::BaseTarget compileDep(NAME, buildcc::TargetType::Executable, gcc,
                                 "data");
  compileDep.AddSource("dummy_main.cpp");
  compileDep.AddCompileDependency("new_source.cpp");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  compileDep.Build();
  buildcc::m::TargetRunner(compileDep);

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_LinkDeps_NoChange) {
  constexpr const char *NAME = "linkDep_NoChange.exe";
  buildcc::BaseTarget linkDep(NAME, buildcc::TargetType::Executable, gcc,
                              "data");
  linkDep.AddSource("dummy_main.cpp");
  linkDep.AddLinkDependency("new_source.cpp");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  linkDep.Build();
  buildcc::m::TargetRunner(linkDep);

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_CompileDeps_Rebuild) {
  constexpr const char *NAME = "compileDep_Rebuild.exe";
  {
    buildcc::BaseTarget compileDep(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    compileDep.AddSource("dummy_main.cpp");
    compileDep.AddCompileDependency("new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    compileDep.Build();
    buildcc::m::TargetRunner(compileDep);
  }

  {
    // * To make sure that save_file is newer
    buildcc::m::blocking_sleep(1);
    const fs::path new_source =
        buildcc::Project::GetRootDir() / "data" / "new_source.cpp";
    std::string buf{""};
    buildcc::env::save_file(new_source.string().c_str(), buf, false);
  }

  {
    buildcc::BaseTarget compileDep(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    compileDep.AddSource("dummy_main.cpp");
    compileDep.AddCompileDependency("new_source.cpp");

    buildcc::m::TargetExpect_PathUpdated(1, &compileDep);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    compileDep.Build();
    buildcc::m::TargetRunner(compileDep);
  }

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_LinkDeps_Rebuild) {
  constexpr const char *NAME = "linkDep_Rebuild.exe";
  {
    buildcc::BaseTarget linkDep(NAME, buildcc::TargetType::Executable, gcc,
                                "data");
    linkDep.AddSource("dummy_main.cpp");
    linkDep.AddLinkDependency("new_source.cpp");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    linkDep.Build();
    buildcc::m::TargetRunner(linkDep);
  }

  {
    // * To make sure that save_file is newer
    buildcc::m::blocking_sleep(1);
    const fs::path new_source =
        buildcc::Project::GetRootDir() / "data" / "new_source.cpp";
    std::string buf{""};
    buildcc::env::save_file(new_source.string().c_str(), buf, false);
  }

  {
    buildcc::BaseTarget linkDep(NAME, buildcc::TargetType::Executable, gcc,
                                "data");
    linkDep.AddSource("dummy_main.cpp");
    linkDep.AddLinkDependency("new_source.cpp");

    buildcc::m::TargetExpect_PathUpdated(1, &linkDep); // Only link
    buildcc::env::m::CommandExpect_Execute(1, true);
    linkDep.Build();
    buildcc::m::TargetRunner(linkDep);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(target_source_intermediate_path);
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_USER_DEPS_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
