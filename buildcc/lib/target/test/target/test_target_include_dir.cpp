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
TEST_GROUP(TargetTestIncludeDirGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

static buildcc::Toolchain gcc(buildcc::ToolchainId::Gcc, "gcc",
                              buildcc::ToolchainExecutables("as", "gcc", "g++",
                                                            "ar", "ld"));

static const fs::path target_include_dir_intermediate_path =
    fs::path(BUILD_TARGET_INCLUDE_DIR_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestIncludeDirGroup, Target_HeaderTypes) {
  constexpr const char *const NAME = "HeaderTypes.exe";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;
  buildcc::BaseTarget simple(NAME, buildcc::TargetType::Executable, gcc,
                             "data");

  simple.AddHeader("fileext/header_file1.h");
  simple.AddHeader("fileext/header_file2.hpp");

  CHECK_EQUAL(simple.GetHeaderFiles().size(), 2);

  CHECK_THROWS(std::exception, simple.AddHeader("fileext/c_file.c"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/cpp_file1.cpp"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/cpp_file2.cxx"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/cpp_file3.cc"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/asm_file1.s"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/asm_file2.S"));
  CHECK_THROWS(std::exception, simple.AddHeader("fileext/asm_file3.asm"));
  CHECK_THROWS(std::exception,
               simple.AddHeader("fileext/invalid_file.invalid"));
}

TEST(TargetTestIncludeDirGroup, TargetGlobHeader) {
  constexpr const char *const NAME = "GlobHeader.exe";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  buildcc::BaseTarget globHeader(NAME, buildcc::TargetType::Executable, gcc,
                                 "data");
  globHeader.GlobHeaders("include");
  globHeader.GlobHeaders("");
  CHECK_EQUAL(globHeader.GetHeaderFiles().size(), 1);
}

TEST(TargetTestIncludeDirGroup, TargetGlobThroughIncludeDir) {
  constexpr const char *const NAME = "GlobThroughIncludeDir.exe";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  buildcc::BaseTarget globIncludeDir(NAME, buildcc::TargetType::Executable, gcc,
                                     "data");
  globIncludeDir.AddIncludeDir("include", true);
  globIncludeDir.AddIncludeDir("", true);
  CHECK_EQUAL(globIncludeDir.GetHeaderFiles().size(), 1);
}

TEST(TargetTestIncludeDirGroup, TargetBuildIncludeDir) {
  constexpr const char *const NAME = "IncludeDir.exe";

  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const RELATIVE_INCLUDE_DIR = "include";
  constexpr const char *const INCLUDE_HEADER_SOURCE = "include_header.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  auto dummy_c_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN_C).make_preferred().string());
  auto include_header_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / INCLUDE_HEADER_SOURCE).make_preferred().string());
  auto include_header_path =
      (source_path / RELATIVE_INCLUDE_DIR).make_preferred();

  {
    buildcc::BaseTarget include_compile(NAME, buildcc::TargetType::Executable,
                                        gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
    // Duplicate include directory (will be reflected)
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    include_compile.Build();
    buildcc::m::TargetRunner(include_compile);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = serialization.GetLoad().internal_sources;
    const auto &loaded_dirs = serialization.GetLoad().include_dirs.GetPaths();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());

    std::unordered_set<std::string> unordered_loaded_dirs(loaded_dirs.begin(),
                                                          loaded_dirs.end());
    CHECK_FALSE(unordered_loaded_dirs.find(include_header_path.string()) ==
                unordered_loaded_dirs.end());
  }
  {
    // * 1 Adding new include directory
    buildcc::BaseTarget include_compile(NAME, buildcc::TargetType::Executable,
                                        gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
    // Adds the data directory
    include_compile.AddIncludeDir("");

    buildcc::m::TargetExpect_DirChanged(1, &include_compile);
    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    include_compile.Build();
    buildcc::m::TargetRunner(include_compile);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = serialization.GetLoad().internal_sources;
    const auto &loaded_dirs = serialization.GetLoad().include_dirs.GetPaths();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());

    std::unordered_set<std::string> unordered_loaded_dirs(loaded_dirs.begin(),
                                                          loaded_dirs.end());
    CHECK_FALSE(unordered_loaded_dirs.find(include_header_path.string()) ==
                unordered_loaded_dirs.end());
  }
  {
    // * Remove include directory
    buildcc::BaseTarget include_compile(NAME, buildcc::TargetType::Executable,
                                        gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::m::TargetExpect_DirChanged(1, &include_compile);
    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    include_compile.Build();
    buildcc::m::TargetRunner(include_compile);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = serialization.GetLoad().internal_sources;
    const auto &loaded_dirs = serialization.GetLoad().include_dirs.GetPaths();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 1);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());

    std::unordered_set<std::string> unordered_loaded_dirs(loaded_dirs.begin(),
                                                          loaded_dirs.end());
    CHECK_FALSE(unordered_loaded_dirs.find(include_header_path.string()) ==
                unordered_loaded_dirs.end());
  }

  mock().checkExpectations();
}

TEST(TargetTestIncludeDirGroup, TargetBuildHeaderFile) {
  constexpr const char *const NAME = "AddHeader.exe";

  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const RELATIVE_HEADER_FILE = "include/include_header.h";
  constexpr const char *const RELATIVE_INCLUDE_DIR = "include";
  constexpr const char *const INCLUDE_HEADER_SOURCE = "include_header.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  auto dummy_c_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN_C));
  auto include_header_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / INCLUDE_HEADER_SOURCE));
  auto include_header_path =
      (source_path / RELATIVE_INCLUDE_DIR).make_preferred();

  // Initial build
  {
    buildcc::BaseTarget add_header(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    add_header.Build();
    buildcc::m::TargetRunner(add_header);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(serialization.GetLoad().internal_sources.size(), 2);
    CHECK_EQUAL(serialization.GetLoad().include_dirs.GetPaths().size(), 1);
    CHECK_EQUAL(serialization.GetLoad().internal_headers.size(), 0);
  }

  // Add header
  {
    buildcc::BaseTarget add_header(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddHeader(RELATIVE_HEADER_FILE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::m::TargetExpect_PathAdded(1, &add_header);
    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    add_header.Build();
    buildcc::m::TargetRunner(add_header);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(serialization.GetLoad().internal_sources.size(), 2);
    CHECK_EQUAL(serialization.GetLoad().include_dirs.GetPaths().size(), 1);
    CHECK_EQUAL(serialization.GetLoad().internal_headers.size(), 1);
  }

  // Update header

  {
    const fs::path absolute_header_path =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / RELATIVE_HEADER_FILE;
    buildcc::env::save_file(absolute_header_path.string().c_str(),
                            std::string{""}, false);

    buildcc::BaseTarget add_header(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddHeader(RELATIVE_HEADER_FILE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::m::TargetExpect_PathUpdated(1, &add_header);
    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    add_header.Build();
    buildcc::m::TargetRunner(add_header);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(serialization.GetLoad().internal_sources.size(), 2);
    CHECK_EQUAL(serialization.GetLoad().include_dirs.GetPaths().size(), 1);
    CHECK_EQUAL(serialization.GetLoad().internal_headers.size(), 1);
  }

  // Remove header
  {
    buildcc::BaseTarget add_header(NAME, buildcc::TargetType::Executable, gcc,
                                   "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::m::TargetExpect_PathRemoved(1, &add_header);
    buildcc::env::m::CommandExpect_Execute(2, true);
    buildcc::env::m::CommandExpect_Execute(1, true);
    add_header.Build();
    buildcc::m::TargetRunner(add_header);

    buildcc::internal::TargetSerialization serialization(
        intermediate_path / (std::string(NAME) + ".bin"));
    bool is_loaded = serialization.LoadFromFile();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(serialization.GetLoad().internal_sources.size(), 2);
    CHECK_EQUAL(serialization.GetLoad().include_dirs.GetPaths().size(), 1);
    CHECK_EQUAL(serialization.GetLoad().internal_headers.size(), 0);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::Project::Init(BUILD_SCRIPT_SOURCE,
                         BUILD_TARGET_INCLUDE_DIR_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
