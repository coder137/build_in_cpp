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
TEST_GROUP(TargetTestSourceGroup)
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
    fs::path(BUILD_TARGET_SOURCE_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestSourceGroup, Target_SourceTypes) {
  constexpr const char *const NAME = "SourceTypes.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;
  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  simple.AddSource("fileext/c_file.c");
  simple.AddSource("fileext/cpp_file1.cpp");
  simple.AddSource("fileext/cpp_file2.cxx");
  simple.AddSource("fileext/cpp_file3.cc");
  simple.AddSource("fileext/asm_file1.s");
  simple.AddSource("fileext/asm_file2.S");
  simple.AddSource("fileext/asm_file3.asm");

  CHECK_EQUAL(simple.GetSourceFiles().size(), 7);
  CHECK_THROWS(std::exception, simple.AddSource("fileext/header_file1.h"));
  CHECK_THROWS(std::exception, simple.AddSource("fileext/header_file2.hpp"));
  CHECK_THROWS(std::exception,
               simple.AddSource("fileext/invalid_file.invalid"));
}

TEST(TargetTestSourceGroup, Target_AddSource) {
  constexpr const char *const NAME = "AddSource.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.AddSource(DUMMY_MAIN);
}

TEST(TargetTestSourceGroup, Target_GlobSource) {
  constexpr const char *const NAME = "GlobSource.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");
  simple.GlobSources("");
  CHECK_EQUAL(simple.GetSourceFiles().size(), 6);
}

TEST(TargetTestSourceGroup, Target_Build_SourceCompile) {
  constexpr const char *const NAME = "Compile.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  simple.AddSource(DUMMY_MAIN);
  simple.Build();

  buildcc::env::m::CommandExpect_Execute(1, true); // compile
  buildcc::env::m::CommandExpect_Execute(1, true); // link
  buildcc::m::TargetRunner(simple);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);

  mock().checkExpectations();

  buildcc::internal::TargetSerialization serialization(simple.GetBinaryPath());
  bool is_loaded = serialization.LoadFromFile();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources =
      serialization.GetLoad().sources.GetUnorderedPathInfos();
  CHECK_EQUAL(loaded_sources.size(), 1);
  auto dummy_file = buildcc::internal::PathInfo::ToPathString(
      fs::path(source_path / DUMMY_MAIN).string());
  CHECK_FALSE(loaded_sources.find(dummy_file) == loaded_sources.end());
}

TEST(TargetTestSourceGroup, Target_Build_SourceRecompile) {
  constexpr const char *const NAME = "Recompile.exe";
  constexpr const char *const DUMMY_MAIN_CPP = "dummy_main.cpp";
  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const NEW_SOURCE = "new_source.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  auto dummy_c_file = buildcc::internal::PathInfo::ToPathString(
      (source_path / DUMMY_MAIN_C).string());
  auto dummy_cpp_file = buildcc::internal::PathInfo::ToPathString(
      (source_path / DUMMY_MAIN_CPP).string());
  auto new_source_file = buildcc::internal::PathInfo::ToPathString(
      (source_path / NEW_SOURCE).string());

  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");

    // * Test C compile
    simple.AddSource(DUMMY_MAIN_C);
    simple.AddSource(NEW_SOURCE);

    buildcc::env::m::CommandExpect_Execute(1, true); // compile
    buildcc::env::m::CommandExpect_Execute(1, true); // compile
    buildcc::env::m::CommandExpect_Execute(1, true); // link
    simple.Build();
    buildcc::m::TargetRunner(simple);

    buildcc::internal::TargetSerialization serialization(
        simple.GetBinaryPath());
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources =
        serialization.GetLoad().sources.GetUnorderedPathInfos();
    CHECK_EQUAL(loaded_sources.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    // * Remove C source
    // * Add CPP source
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);

    buildcc::m::TargetExpect_SourceRemoved(1, &simple);

    // Added and compiled
    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::TargetExpect_SourceAdded(1, &simple);

    // Rebuild target
    buildcc::env::m::CommandExpect_Execute(1, true);

    // Run the second Build to test Recompile
    simple.Build();
    buildcc::m::TargetRunner(simple);

    buildcc::internal::TargetSerialization serialization(
        simple.GetBinaryPath());
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources =
        serialization.GetLoad().sources.GetUnorderedPathInfos();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(dummy_cpp_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    buildcc::m::blocking_sleep(1);

    // * Force copy to trigger recompile for NEW_SOURCE
    // *2 Current file is updated
    auto file_path = source_path / NEW_SOURCE;
    buildcc::env::save_file(file_path.string().c_str(), std::string{""}, false);

    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile

    buildcc::env::m::CommandExpect_Execute(1, true);
    buildcc::m::TargetExpect_SourceUpdated(1, &simple);

    buildcc::env::m::CommandExpect_Execute(1, true);
    simple.Build();
    buildcc::m::TargetRunner(simple);

    buildcc::internal::TargetSerialization serialization(
        simple.GetBinaryPath());
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources =
        serialization.GetLoad().sources.GetUnorderedPathInfos();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(dummy_cpp_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }

  mock().checkExpectations();
}

TEST(TargetTestSourceGroup, Target_CompileCommand_Throws) {
  constexpr const char *const NAME = "CompileCommand_Throws.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  {
    buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                               "data");
    simple.AddSource("dummy_main.c");

    auto p = simple.GetTargetRootDir() / "dummy_main.c";
    p.make_preferred();

    // Throws when you call CompileCommand before Build
    CHECK_THROWS(std::exception, simple.GetCompileCommand(p));
    // Link Command will be empty before Build
    STRCMP_EQUAL(simple.GetLinkCommand().c_str(), "");
  }
}

int main(int ac, char **av) {
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_SOURCE_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
