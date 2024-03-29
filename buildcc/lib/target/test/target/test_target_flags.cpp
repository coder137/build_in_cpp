#include "constants.h"

#include "expect_command.h"
#include "expect_target.h"

#include "target/target.h"

#include "env/env.h"

//
#include "schema/target_serialization.h"

// Third Party

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// Constants

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

// ------------- PREPROCESSOR FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestPreprocessorFlagGroup)
{
    void teardown() {
      mock().checkExpectations();
      mock().clear();
    }
};
// clang-format on

static const fs::path target_preprocessorflag_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestPreprocessorFlagGroup, Target_AddPreprocessorFlag) {
  constexpr const char *const NAME = "AddPreprocessorFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_preprocessorflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddPreprocessorFlag("-DCOMPILE=1");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  simple.Build();
  buildcc::m::TargetRunner(simple);
  CHECK_TRUE(simple.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().preprocessor_flags.size(), 1);
}

TEST(TargetTestPreprocessorFlagGroup, Target_ChangedPreprocessorFlag) {
  constexpr const char *const NAME = "ChangedPreprocessorFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_preprocessorflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddPreprocessorFlag("-DCOMPILE=1");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddPreprocessorFlag("-DRANDOM=1");
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

// ------------- COMMON COMPILE FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestCommonCompileFlagsGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const fs::path target_commonflag_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestCommonCompileFlagsGroup, Target_AddCommonCompileFlag) {
  constexpr const char *const NAME = "AddCommonCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_commonflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");
    simple.AddCommonCompileFlag("-g");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());

    mock().checkExpectations();

    // Verify binary
    buildcc::internal::TargetSerialization serialization(
        simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
    bool loaded = serialization.LoadFromFile();
    CHECK_TRUE(loaded);

    CHECK_EQUAL(serialization.GetLoad().common_compile_flags.size(), 2);
  }

  // Trigger Rebuild for Common Compile flag
  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());

    mock().checkExpectations();

    // Verify binary
    buildcc::internal::TargetSerialization serialization(
        simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
    bool loaded = serialization.LoadFromFile();
    CHECK_TRUE(loaded);

    CHECK_EQUAL(serialization.GetLoad().common_compile_flags.size(), 1);
  }
}

TEST(TargetTestCommonCompileFlagsGroup, Target_ChangedCommonCompileFlag) {
  constexpr const char *const NAME = "ChangedCommonCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_commonflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");
    simple.AddCommonCompileFlag("-g");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");
    simple.AddCommonCompileFlag("-g");

    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_FALSE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCommonCompileFlag("-O0");
    simple.AddCommonCompileFlag("-g");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

// ------------- ASM COMPILE FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestAsmCompileFlagGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const fs::path target_asmflag_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestAsmCompileFlagGroup, Target_AddCompileFlag) {
  constexpr const char *const NAME = "AddAsmCompileFlag.exe";
  constexpr const char *const EMPTY_ASM = "asm/empty_asm.s";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_asmflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.AddSource(EMPTY_ASM);
  simple.AddAsmCompileFlag("-O0");
  simple.AddAsmCompileFlag("-g");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  simple.Build();
  buildcc::m::TargetRunner(simple);
  CHECK_TRUE(simple.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().asm_compile_flags.size(), 2);
}

TEST(TargetTestAsmCompileFlagGroup, Target_ChangedCompileFlag) {
  constexpr const char *const NAME = "ChangedAsmCompileFlag.exe";
  constexpr const char *const EMPTY_ASM = "asm/empty_asm.s";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_asmflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(EMPTY_ASM);
    simple.AddAsmCompileFlag("-O0");
    simple.AddAsmCompileFlag("-g");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    // * No Change
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(EMPTY_ASM);
    simple.AddAsmCompileFlag("-O0");
    simple.AddAsmCompileFlag("-g");

    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_FALSE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(EMPTY_ASM);
    simple.AddAsmCompileFlag("-O0");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(EMPTY_ASM);
    simple.AddAsmCompileFlag("-O0");
    simple.AddAsmCompileFlag("-g");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

// ------------- C COMPILE FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestCCompileFlagsGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const fs::path target_cflag_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestCCompileFlagsGroup, Target_AddCompileFlag) {
  constexpr const char *const NAME = "AddCCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.c";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_cflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddCCompileFlag("-std=c11");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  simple.Build();
  buildcc::m::TargetRunner(simple);
  CHECK_TRUE(simple.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().c_compile_flags.size(), 1);
}

TEST(TargetTestCCompileFlagsGroup, Target_ChangedCompileFlag) {
  constexpr const char *const NAME = "ChangedCCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.c";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_cflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c11");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c11");
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

// ------------- CPP COMPILE FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestCppCompileFlagsGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const fs::path target_cppflags_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestCppCompileFlagsGroup, Target_AddCompileFlag) {
  constexpr const char *const NAME = "AddCppCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_cppflags_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCppCompileFlag("-std=c++17");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());

    mock().checkExpectations();

    // Verify binary
    buildcc::internal::TargetSerialization serialization(
        simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
    bool loaded = serialization.LoadFromFile();
    CHECK_TRUE(loaded);

    CHECK_EQUAL(serialization.GetLoad().cpp_compile_flags.size(), 1);
  }

  // Trigger rebuild for Cpp Compile flag
  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCppCompileFlag("-std=c++20");

    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());

    mock().checkExpectations();

    // Verify binary
    buildcc::internal::TargetSerialization serialization(
        simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
    bool loaded = serialization.LoadFromFile();
    CHECK_TRUE(loaded);

    CHECK_EQUAL(serialization.GetLoad().cpp_compile_flags.size(), 1);
  }
}

TEST(TargetTestCppCompileFlagsGroup, Target_ChangedCompileFlag) {
  constexpr const char *const NAME = "ChangedCppCompileFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_cppflags_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c++17");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddCCompileFlag("-std=c++17");
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

// ------------- LINK FLAGS ---------------

// clang-format off
TEST_GROUP(TargetTestLinkFlagsGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static const fs::path target_linkflag_intermediate_path =
    fs::path(BUILD_TARGET_FLAG_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestLinkFlagsGroup, Target_AddLinkFlag) {
  constexpr const char *const NAME = "AddLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_linkflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.AddSource(DUMMY_MAIN);
  simple.AddLinkFlag("-lm");

  buildcc::env::m::CommandExpect_Execute(1, true);
  buildcc::env::m::CommandExpect_Execute(1, true);
  simple.Build();
  buildcc::m::TargetRunner(simple);
  CHECK_TRUE(simple.IsBuilt());

  mock().checkExpectations();

  // Verify binary
  buildcc::internal::TargetSerialization serialization(
      simple.GetTargetBuildDir() / (std::string(NAME) + ".bin"));
  bool loaded = serialization.LoadFromFile();
  CHECK_TRUE(loaded);

  CHECK_EQUAL(serialization.GetLoad().link_flags.size(), 1);
}

TEST(TargetTestLinkFlagsGroup, Target_ChangedLinkFlag) {
  constexpr const char *const NAME = "ChangedLinkFlag.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_linkflag_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }
  {
    // * Remove flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  {
    // * Add flag
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN);
    simple.AddLinkFlag("-lm");
    buildcc::m::TargetExpect_FlagChanged(1, &simple);
    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);
    CHECK_TRUE(simple.IsBuilt());
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_FLAG_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
