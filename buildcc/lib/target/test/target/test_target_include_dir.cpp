#include "constants.h"

#include "expect_target.h"
#include "target.h"

#include "env.h"

// Third Party
#include "flatbuffers/util.h"

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

static const buildcc::base::Toolchain gcc("gcc", "as", "gcc", "g++", "ar",
                                          "ld");
static const fs::path target_include_dir_intermediate_path =
    fs::path(BUILD_TARGET_INCLUDE_DIR_INTERMEDIATE_DIR) / gcc.GetName();

TEST(TargetTestIncludeDirGroup, Target_HeaderTypes) {
  constexpr const char *const NAME = "HeaderTypes.exe";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;
  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable, gcc,
                               "data");

  simple.AddHeader("fileext/header_file1.h");
  simple.AddHeader("fileext/header_file2.hpp");

  CHECK_EQUAL(simple.GetCurrentHeaderFiles().size(), 2);

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
  buildcc::base::Target globHeader(NAME, buildcc::base::TargetType::Executable,
                                   gcc, "data");
  globHeader.GlobHeaders("include");
  globHeader.GlobHeaders("");
  CHECK_EQUAL(globHeader.GetCurrentHeaderFiles().size(), 1);
}

TEST(TargetTestIncludeDirGroup, TargetGlobThroughIncludeDir) {
  constexpr const char *const NAME = "GlobThroughIncludeDir.exe";
  auto intermediate_path = target_include_dir_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  buildcc::base::Target globIncludeDir(
      NAME, buildcc::base::TargetType::Executable, gcc, "data");
  globIncludeDir.AddIncludeDir("include", true);
  globIncludeDir.AddIncludeDir("", true);
  CHECK_EQUAL(globIncludeDir.GetCurrentHeaderFiles().size(), 1);
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

  auto dummy_c_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN_C));
  auto include_header_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / INCLUDE_HEADER_SOURCE));
  auto include_header_path =
      (source_path / RELATIVE_INCLUDE_DIR).make_preferred();

  {
    buildcc::base::Target include_compile(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
    // Duplicate include directory
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    include_compile.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = loader.GetLoadedSources();
    const auto &loaded_dirs = loader.GetLoadedIncludeDirs();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 1);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());

    CHECK_FALSE(loaded_dirs.find(include_header_path.string()) ==
                loaded_dirs.end());
  }
  {
    // * 1 Adding new include directory
    buildcc::base::Target include_compile(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
    // Adds the data directory
    include_compile.AddIncludeDir("");

    buildcc::base::m::TargetExpect_DirChanged(1, &include_compile);
    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    include_compile.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = loader.GetLoadedSources();
    const auto &loaded_dirs = loader.GetLoadedIncludeDirs();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());
    CHECK_FALSE(loaded_dirs.find(include_header_path.string()) ==
                loaded_dirs.end());
  }
  {
    // * Remove include directory
    buildcc::base::Target include_compile(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::base::m::TargetExpect_DirChanged(1, &include_compile);
    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    include_compile.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    const auto &loaded_sources = loader.GetLoadedSources();
    const auto &loaded_dirs = loader.GetLoadedIncludeDirs();

    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_EQUAL(loaded_dirs.size(), 1);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(include_header_file) ==
                loaded_sources.end());
    CHECK_FALSE(loaded_dirs.find(include_header_path.string()) ==
                loaded_dirs.end());
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
    buildcc::base::Target add_header(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    add_header.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(loader.GetLoadedSources().size(), 2);
    CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 1);
    CHECK_EQUAL(loader.GetLoadedHeaders().size(), 0);
  }

  // Add header
  {
    buildcc::base::Target add_header(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddHeader(RELATIVE_HEADER_FILE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::base::m::TargetExpect_PathAdded(1, &add_header);
    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    add_header.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(loader.GetLoadedSources().size(), 2);
    CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 1);
    CHECK_EQUAL(loader.GetLoadedHeaders().size(), 1);
  }

  // Update header

  {
    const fs::path absolute_header_path =
        fs::path(BUILD_SCRIPT_SOURCE) / "data" / RELATIVE_HEADER_FILE;
    flatbuffers::SaveFile(absolute_header_path.string().c_str(),
                          std::string{""}, false);

    buildcc::base::Target add_header(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddHeader(RELATIVE_HEADER_FILE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::base::m::TargetExpect_PathUpdated(1, &add_header);
    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    add_header.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(loader.GetLoadedSources().size(), 2);
    CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 1);
    CHECK_EQUAL(loader.GetLoadedHeaders().size(), 1);
  }

  // Remove header
  {
    buildcc::base::Target add_header(
        NAME, buildcc::base::TargetType::Executable, gcc, "data");
    add_header.AddSource(DUMMY_MAIN_C);
    add_header.AddSource(INCLUDE_HEADER_SOURCE);
    add_header.AddIncludeDir(RELATIVE_INCLUDE_DIR);

    buildcc::base::m::TargetExpect_PathRemoved(1, &add_header);
    buildcc::internal::m::Expect_command(2, true);
    buildcc::internal::m::Expect_command(1, true);
    add_header.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);
    CHECK_EQUAL(loader.GetLoadedSources().size(), 2);
    CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 1);
    CHECK_EQUAL(loader.GetLoadedHeaders().size(), 0);
  }

  mock().checkExpectations();
}

int main(int ac, char **av) {
  buildcc::env::init(BUILD_SCRIPT_SOURCE,
                     BUILD_TARGET_INCLUDE_DIR_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
