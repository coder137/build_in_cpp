#include "constants.h"

#include <unistd.h>

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
TEST_GROUP(TargetTestSourceGroup)
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
    fs::path(BUILD_TARGET_SOURCE_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestSourceGroup, Target_SourceTypes) {
  constexpr const char *const NAME = "SourceTypes.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;
  buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");

  simple.AddSource("fileext/c_file.c");
  simple.AddSource("fileext/cpp_file1.cpp");
  simple.AddSource("fileext/cpp_file2.cxx");
  simple.AddSource("fileext/cpp_file3.cc");
  simple.AddSource("fileext/asm_file1.s");
  simple.AddSource("fileext/asm_file2.S");
  simple.AddSource("fileext/asm_file3.asm");

  CHECK_EQUAL(simple.GetCurrentSourceFiles().size(), 7);
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

  buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");
  simple.AddSource(DUMMY_MAIN);
}

TEST(TargetTestSourceGroup, Target_GlobSource) {
  constexpr const char *const NAME = "GlobSource.exe";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");
  simple.GlobSources("");
  CHECK_EQUAL(simple.GetCurrentSourceFiles().size(), 6);
}

TEST(TargetTestSourceGroup, Target_Build_SourceCompile) {
  constexpr const char *const NAME = "Compile.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                               gcc, "data");

  buildcc::m::CommandExpect_Execute(1, true); // compile
  buildcc::m::CommandExpect_Execute(1, true); // link

  simple.AddSource(DUMMY_MAIN);
  simple.Build();

  mock().checkExpectations();

  buildcc::internal::TargetLoader loader(NAME, intermediate_path);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
  auto dummy_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN).make_preferred().string());
  CHECK_FALSE(loaded_sources.find(dummy_file) == loaded_sources.end());
}

TEST(TargetTestSourceGroup, Target_Build_SourceCompileError) {
  constexpr const char *const NAME = "CompileError.exe";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete

  {
    fs::remove_all(intermediate_path);
    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");

    simple.AddSource(DUMMY_MAIN);
    buildcc::m::CommandExpect_Execute(1, false); // compile
    CHECK_THROWS(std::exception, simple.Build());
  }

  {
    fs::remove_all(intermediate_path);
    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");

    simple.AddSource(DUMMY_MAIN);
    buildcc::m::CommandExpect_Execute(1, true);  // compile
    buildcc::m::CommandExpect_Execute(1, false); // compile
    CHECK_THROWS(std::exception, simple.Build());
  }

  mock().checkExpectations();
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
  auto dummy_c_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN_C).make_preferred().string());
  auto dummy_cpp_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN_CPP).make_preferred().string());
  auto new_source_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / NEW_SOURCE).make_preferred().string());

  {
    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");

    // * Test C compile
    simple.AddSource(DUMMY_MAIN_C);
    simple.AddSource(NEW_SOURCE);

    buildcc::m::CommandExpect_Execute(1, true); // compile
    buildcc::m::CommandExpect_Execute(1, true); // compile
    buildcc::m::CommandExpect_Execute(1, true); // link
    simple.Build();

    buildcc::internal::TargetLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    // * Remove C source
    // * Add CPP source
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);

    buildcc::base::m::TargetExpect_SourceRemoved(1, &simple);

    // Added and compiled
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::base::m::TargetExpect_SourceAdded(1, &simple);

    // Rebuild target
    buildcc::m::CommandExpect_Execute(1, true);

    // Run the second Build to test Recompile
    simple.Build();

    buildcc::internal::TargetLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(dummy_cpp_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    sleep(1);

    // * Force copy to trigger recompile for NEW_SOURCE
    // *2 Current file is updated
    auto file_path = source_path / NEW_SOURCE;
    buildcc::env::SaveFile(file_path.string().c_str(), std::string{""}, false);

    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::base::m::TargetExpect_SourceUpdated(1, &simple);

    buildcc::m::CommandExpect_Execute(1, true);
    simple.Build();

    buildcc::internal::TargetLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
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
    buildcc::base::Target simple(NAME, buildcc::base::Target::Type::Executable,
                                 gcc, "data");
    simple.AddSource("dummy_main.c");

    auto p = simple.GetTargetRootDir() / "dummy_main.c";
    p.make_preferred();

    // Throws when you call CompileCommand before Build
    CHECK_THROWS(std::exception, simple.GetCompileCommand(p));
  }
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_SOURCE_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
