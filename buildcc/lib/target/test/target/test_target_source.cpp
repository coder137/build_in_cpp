#include "constants.h"

#include "base/target.h"

#include "env.h"

// Third Party
#include "flatbuffers/util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TargetTestSourceGroup)
{
};
// clang-format on

static fs::path target_source_intermediate_path =
    fs::path(BUILD_TARGET_SOURCE_INTERMEDIATE_DIR);

TEST(TargetTestSourceGroup, TargetAddSource) {
  constexpr const char *const NAME = "AddSource.exe";
  constexpr const char *const BIN = "AddSource.exe.bin";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";
  constexpr const char *const NO_FILE = "no_file.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                               buildcc::Toolchain("gcc", "gcc", "g++"), "data");
  simple.AddSource(DUMMY_MAIN);
  // File does not exist
  CHECK_THROWS(std::string, simple.AddSource(NO_FILE));
  // Duplicate file added
  CHECK_THROWS(std::string, simple.AddSource(DUMMY_MAIN));
  simple.Build();

  buildcc::internal::FbsLoader loader(NAME, intermediate_path);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
  auto dummy_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN));
  CHECK_FALSE(loaded_sources.find(dummy_file) == loaded_sources.end());
}

TEST(TargetTestSourceGroup, TargetBuildSourceCompile) {
  constexpr const char *const NAME = "Compile.exe";
  constexpr const char *const BIN = "Compile.exe.bin";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);

  buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                               buildcc::Toolchain("gcc", "gcc", "g++"), "data");
  simple.AddSource(DUMMY_MAIN);
  simple.Build();

  buildcc::internal::FbsLoader loader(NAME, intermediate_path);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  const auto &loaded_sources = loader.GetLoadedSources();
  CHECK_EQUAL(loaded_sources.size(), 1);
  auto dummy_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN).string());
  CHECK_FALSE(loaded_sources.find(dummy_file) == loaded_sources.end());
}

TEST(TargetTestSourceGroup, TargetBuildSourceRecompile) {
  constexpr const char *const NAME = "Recompile.exe";
  constexpr const char *const BIN = "Recompile.exe.bin";
  constexpr const char *const DUMMY_MAIN_CPP = "dummy_main.cpp";
  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const NEW_SOURCE = "new_source.cpp";

  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = target_source_intermediate_path / NAME;

  // Delete
  fs::remove_all(intermediate_path);
  auto dummy_c_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN_C));
  auto dummy_cpp_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN_CPP));
  auto new_source_file =
      buildcc::internal::Path::CreateExistingPath((source_path / NEW_SOURCE));

  {
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 buildcc::Toolchain("gcc", "gcc", "g++"),
                                 "data");
    // * Test C compile
    simple.AddSource(DUMMY_MAIN_C);
    simple.AddSource(NEW_SOURCE);
    simple.Build();

    // * Simple recompile
    simple.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);

    CHECK_FALSE(loaded_sources.find(dummy_c_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 buildcc::Toolchain("gcc", "gcc", "g++"),
                                 "data");
    // * Remove C source
    // * Add CPP source
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile
    simple.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(dummy_cpp_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
  {
    // * Force copy to trigger recompile for NEW_SOURCE
    // *2 Current file is updated
    auto file_path = source_path / NEW_SOURCE;
    flatbuffers::SaveFile(file_path.string().c_str(), std::string{""}, false);

    buildcc::base::Target simple(NAME, buildcc::base::TargetType::Executable,
                                 buildcc::Toolchain("gcc", "gcc", "g++"),
                                 "data");
    simple.AddSource(DUMMY_MAIN_CPP);
    simple.AddSource(NEW_SOURCE);
    // Run the second Build to test Recompile
    simple.Build();

    buildcc::internal::FbsLoader loader(NAME, intermediate_path);
    bool is_loaded = loader.Load();
    CHECK_TRUE(is_loaded);

    const auto &loaded_sources = loader.GetLoadedSources();
    CHECK_EQUAL(loaded_sources.size(), 2);
    CHECK_FALSE(loaded_sources.find(dummy_cpp_file) == loaded_sources.end());
    CHECK_FALSE(loaded_sources.find(new_source_file) == loaded_sources.end());
  }
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_TARGET_SOURCE_INTERMEDIATE_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
