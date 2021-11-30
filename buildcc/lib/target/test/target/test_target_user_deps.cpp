#include <unistd.h>

#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"

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

static const buildcc::base::Toolchain gcc(buildcc::base::Toolchain::Id::Gcc,
                                          "gcc", "as", "gcc", "g++", "ar",
                                          "ld");

static const fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_USER_DEPS_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestUserDepsGroup, Target_Build_CompileDeps_NoChange) {
  constexpr const char *NAME = "compileDep_NoChange.exe";
  buildcc::base::Target compileDep(NAME, buildcc::base::TargetType::Executable,
                                   gcc, "data");
  compileDep.AddSource("dummy_main.cpp");
  compileDep.AddCompileDependency("new_source.cpp");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  compileDep.Build();
  buildcc::base::m::TargetRunner(compileDep);

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_LinkDeps_NoChange) {
  constexpr const char *NAME = "linkDep_NoChange.exe";
  buildcc::base::Target linkDep(NAME, buildcc::base::TargetType::Executable,
                                gcc, "data");
  linkDep.AddSource("dummy_main.cpp");
  linkDep.AddLinkDependency("new_source.cpp");

  buildcc::m::CommandExpect_Execute(1, true);
  buildcc::m::CommandExpect_Execute(1, true);
  linkDep.Build();
  buildcc::base::m::TargetRunner(linkDep);

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_CompileDeps_Rebuild) {
  constexpr const char *NAME = "compileDep_Rebuild.exe";
  {
    buildcc::base::Target compileDep(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    compileDep.AddSource("dummy_main.cpp");
    compileDep.AddCompileDependency("new_source.cpp");

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    compileDep.Build();
    buildcc::base::m::TargetRunner(compileDep);
  }

  {
    // * To make sure that save_file is newer
    sleep(1);
    const fs::path new_source =
        buildcc::env::get_project_root_dir() / "data" / "new_source.cpp";
    std::string buf{""};
    buildcc::env::save_file(new_source.string().c_str(), buf, false);
  }

  {
    buildcc::base::Target compileDep(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    compileDep.AddSource("dummy_main.cpp");
    compileDep.AddCompileDependency("new_source.cpp");

    buildcc::base::m::TargetExpect_PathUpdated(1, &compileDep);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    compileDep.Build();
    buildcc::base::m::TargetRunner(compileDep);
  }

  mock().checkExpectations();
}

TEST(TargetTestUserDepsGroup, Target_Build_LinkDeps_Rebuild) {
  constexpr const char *NAME = "linkDep_Rebuild.exe";
  {
    buildcc::base::Target linkDep(NAME, buildcc::base::TargetType::Executable,
                                  gcc, "data");
    linkDep.AddSource("dummy_main.cpp");
    linkDep.AddLinkDependency("new_source.cpp");

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    linkDep.Build();
    buildcc::base::m::TargetRunner(linkDep);
  }

  {
    // * To make sure that save_file is newer
    sleep(1);
    const fs::path new_source =
        buildcc::env::get_project_root_dir() / "data" / "new_source.cpp";
    std::string buf{""};
    buildcc::env::save_file(new_source.string().c_str(), buf, false);
  }

  {
    buildcc::base::Target linkDep(NAME, buildcc::base::TargetType::Executable,
                                  gcc, "data");
    linkDep.AddSource("dummy_main.cpp");
    linkDep.AddLinkDependency("new_source.cpp");

    buildcc::base::m::TargetExpect_PathUpdated(1, &linkDep); // Only link
    buildcc::m::CommandExpect_Execute(1, true);
    linkDep.Build();
    buildcc::base::m::TargetRunner(linkDep);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(target_source_intermediate_path);
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_USER_DEPS_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
