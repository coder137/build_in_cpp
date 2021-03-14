#include "buildcc.h"
#include "constants.h"

// Third Party
#include "flatbuffers/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetTestGroup)
{
};
// clang-format on

TEST(TargetTestGroup, TargetAddSource) {
  constexpr const char *const NAME = "AddSource.exe";
  constexpr const char *const BIN = "AddSource.exe.bin";
  constexpr const char *const DUMMY_MAIN = "data/dummy_main.cpp";
  constexpr const char *const NO_FILE = "data/no_file.cpp";

  // Delete
  fs::remove(std::string(BUILD_SCRIPT_SOURCE) + "/" + BIN);

  buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                         buildcc::Toolchain("gcc", "gcc", "g++"),
                         BUILD_SCRIPT_SOURCE);
  simple.AddSource(DUMMY_MAIN);
  // File does not exist
  CHECK_THROWS(std::string, simple.AddSource(NO_FILE));
  // Duplicate file added
  CHECK_THROWS(std::string, simple.AddSource(DUMMY_MAIN));
  simple.Build();

  buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
  CHECK_FALSE(loaded_sources.find(buildcc::internal::Path::CreateExistingPath(
                  std::string(BUILD_SCRIPT_SOURCE) + "/" + DUMMY_MAIN)) ==
              loaded_sources.end());
}

TEST(TargetTestGroup, TargetBuildCompile) {
  constexpr const char *const NAME = "Compile.exe";
  constexpr const char *const BIN = "Compile.exe.bin";
  constexpr const char *const DUMMY_MAIN = "data/dummy_main.cpp";

  // Delete
  fs::remove(std::string(BUILD_SCRIPT_SOURCE) + "/" + BIN);

  buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                         buildcc::Toolchain("gcc", "gcc", "g++"),
                         BUILD_SCRIPT_SOURCE);
  simple.AddSource(DUMMY_MAIN);
  simple.Build();

  buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
  CHECK_FALSE(loaded_sources.find(buildcc::internal::Path::CreateExistingPath(
                  std::string(BUILD_SCRIPT_SOURCE) + "/" + DUMMY_MAIN)) ==
              loaded_sources.end());
}

TEST(TargetTestGroup, TargetBuildRecompile) {
  constexpr const char *const NAME = "Recompile.exe";
  constexpr const char *const BIN = "Recompile.exe.bin";
  constexpr const char *const DUMMY_MAIN_CPP = "data/dummy_main.cpp";
  constexpr const char *const DUMMY_MAIN_C = "data/dummy_main.c";
  constexpr const char *const NEW_SOURCE = "data/new_source.cpp";

  // Delete
  fs::remove(std::string(BUILD_SCRIPT_SOURCE) + "/" + BIN);

  {
    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"),
                           BUILD_SCRIPT_SOURCE);
    // * Test C compile
    simple.AddSource(DUMMY_MAIN_C);
    simple.AddSource(NEW_SOURCE);
    simple.Build();

    // * Simple recompile
    simple.Build();

    buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(buildcc::internal::Path::CreateExistingPath(
                    std::string(BUILD_SCRIPT_SOURCE) + "/" + DUMMY_MAIN_C)) ==
                loaded_sources.end());
  }
  {
    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"),
                           BUILD_SCRIPT_SOURCE);
    // * Remove C source
    // * Add CPP source
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile
    simple.Build();

    buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(buildcc::internal::Path::CreateExistingPath(
                    std::string(BUILD_SCRIPT_SOURCE) + "/" + DUMMY_MAIN_CPP)) ==
                loaded_sources.end());
  }
  {
    // * Force copy to trigger recompile for NEW_SOURCE
    // *2 Current file is updated
    auto file_path = fs::path(BUILD_SCRIPT_SOURCE) / NEW_SOURCE;
    flatbuffers::SaveFile(file_path.string().c_str(), std::string{""}, false);

    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"),
                           BUILD_SCRIPT_SOURCE);
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile
    simple.Build();

    buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(buildcc::internal::Path::CreateExistingPath(
                    std::string(BUILD_SCRIPT_SOURCE) + "/" + DUMMY_MAIN_CPP)) ==
                loaded_sources.end());
  }
}

// TODO, Check toolchain change
// There are few parameters that must NOT be changed after the initial buildcc
// project is generated
// NOTE, Throw an error if these options are changed by the user

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
