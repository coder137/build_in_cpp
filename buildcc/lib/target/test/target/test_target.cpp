#include "constants.h"
#include "target.h"

#include "env.h"

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

TEST(TargetTestGroup, TargetType) {
  constexpr const char *const EXE_NAME = "ExeTest.exe";
  constexpr const char *const STATIC_NAME = "StaticTest.a";
  constexpr const char *const SHARED_NAME = "SharedTest.so";
  constexpr const char *const INVALID_NAME = "Invalid.random";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_INTERMEDIATE_DIR);

  auto intermediate_path = fs::path(BUILD_INTERMEDIATE_DIR);

  fs::remove(intermediate_path / EXE_NAME / (std::string(EXE_NAME) + ".bin"));
  fs::remove(intermediate_path / STATIC_NAME /
             (std::string(STATIC_NAME) + ".bin"));
  fs::remove(intermediate_path / SHARED_NAME /
             (std::string(SHARED_NAME) + ".bin"));
  fs::remove(intermediate_path / INVALID_NAME /
             (std::string(INVALID_NAME) + ".bin"));

  {
    buildcc::Target exe_target(EXE_NAME, buildcc::TargetType::Executable,
                               buildcc::Toolchain("gcc", "gcc", "g++"), "");
    exe_target.AddSource("data/dummy_main.cpp");
    exe_target.Build();
  }

  {
    buildcc::Target static_target(STATIC_NAME,
                                  buildcc::TargetType::StaticLibrary,
                                  buildcc::Toolchain("gcc", "gcc", "g++"), "");
    static_target.AddSource("data/dummy_main.cpp");
    static_target.Build();
  }

  {
    buildcc::Target shared_target(SHARED_NAME,
                                  buildcc::TargetType::DynamicLibrary,
                                  buildcc::Toolchain("gcc", "gcc", "g++"), "");
    shared_target.AddSource("data/dummy_main.cpp");
    shared_target.Build();
  }

  {
    buildcc::Target invalid_target(INVALID_NAME, (buildcc::TargetType)3,
                                   buildcc::Toolchain("gcc", "gcc", "g++"), "");
    invalid_target.AddSource("data/dummy_main.cpp");
    CHECK_THROWS(std::string, invalid_target.Build());
  }

  buildcc::env::deinit();
}

TEST(TargetTestGroup, TargetInit) {
  constexpr const char *const NAME = "Init.exe";
  constexpr const char *const BIN = "AddSource.exe.bin";

  CHECK_THROWS(std::string,
               buildcc::Target(NAME, buildcc::TargetType::Executable,
                               buildcc::Toolchain("gcc", "gcc", "g++"),
                               "data"));
}

TEST(TargetTestGroup, TargetAddSource) {
  constexpr const char *const NAME = "AddSource.exe";
  constexpr const char *const BIN = "AddSource.exe.bin";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";
  constexpr const char *const NO_FILE = "no_file.cpp";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_INTERMEDIATE_DIR);
  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = fs::path(BUILD_INTERMEDIATE_DIR) / NAME;

  // Delete
  fs::remove(intermediate_path / BIN);

  buildcc::Target simple(NAME, buildcc::TargetType::Executable,
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

  buildcc::env::deinit();
}

TEST(TargetTestGroup, TargetBuildSourceCompile) {
  constexpr const char *const NAME = "Compile.exe";
  constexpr const char *const BIN = "Compile.exe.bin";
  constexpr const char *const DUMMY_MAIN = "dummy_main.cpp";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_INTERMEDIATE_DIR);
  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = fs::path(BUILD_INTERMEDIATE_DIR) / NAME;

  // Delete
  fs::remove(intermediate_path / BIN);

  buildcc::Target simple(NAME, buildcc::TargetType::Executable,
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

  buildcc::env::deinit();
}

TEST(TargetTestGroup, TargetBuildSourceRecompile) {
  constexpr const char *const NAME = "Recompile.exe";
  constexpr const char *const BIN = "Recompile.exe.bin";
  constexpr const char *const DUMMY_MAIN_CPP = "dummy_main.cpp";
  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const NEW_SOURCE = "new_source.cpp";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_INTERMEDIATE_DIR);
  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = fs::path(BUILD_INTERMEDIATE_DIR) / NAME;

  // Delete
  fs::remove(intermediate_path / BIN);
  auto dummy_c_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN_C));
  auto dummy_cpp_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / DUMMY_MAIN_CPP));
  auto new_source_file =
      buildcc::internal::Path::CreateExistingPath((source_path / NEW_SOURCE));

  {
    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"), "data");
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
    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"), "data");
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

    buildcc::Target simple(NAME, buildcc::TargetType::Executable,
                           buildcc::Toolchain("gcc", "gcc", "g++"), "data");
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

  buildcc::env::deinit();
}

TEST(TargetTestGroup, TargetBuildIncludeDir) {
  constexpr const char *const NAME = "IncludeCompile.exe";
  constexpr const char *const BIN = "IncludeCompile.exe.bin";

  constexpr const char *const DUMMY_MAIN_C = "dummy_main.c";
  constexpr const char *const RELATIVE_INCLUDE_DIR = "include";
  constexpr const char *const INCLUDE_HEADER_SOURCE = "include_header.cpp";
  constexpr const char *const INCLUDE_HEADER_INCLUDE = "include_header.h";

  buildcc::env::init(BUILD_SCRIPT_SOURCE, BUILD_INTERMEDIATE_DIR);
  auto source_path = fs::path(BUILD_SCRIPT_SOURCE) / "data";
  auto intermediate_path = fs::path(BUILD_INTERMEDIATE_DIR) / NAME;

  // Delete
  fs::remove(intermediate_path / BIN);

  auto dummy_c_file =
      buildcc::internal::Path::CreateExistingPath((source_path / DUMMY_MAIN_C));
  auto include_header_file = buildcc::internal::Path::CreateExistingPath(
      (source_path / INCLUDE_HEADER_SOURCE));
  auto include_header_path = buildcc::internal::Path::CreateExistingPath(
      (source_path / RELATIVE_INCLUDE_DIR));

  {
    buildcc::Target include_compile(NAME, buildcc::TargetType::Executable,
                                    buildcc::Toolchain("gcc", "gcc", "g++"),
                                    "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
    // Duplicate include directory
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
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
    CHECK_FALSE(loaded_dirs.find(include_header_path) == loaded_dirs.end());
  }
  {
    // * Force copy to trigger recompile when HEADER changes
    // *2 Current file is updated
    auto file_path =
        source_path / RELATIVE_INCLUDE_DIR / INCLUDE_HEADER_INCLUDE;
    flatbuffers::SaveFile(file_path.string().c_str(), std::string{""}, false);

    buildcc::Target include_compile(NAME, buildcc::TargetType::Executable,
                                    buildcc::Toolchain("gcc", "gcc", "g++"),
                                    "data");
    include_compile.AddSource(DUMMY_MAIN_C);
    include_compile.AddSource(INCLUDE_HEADER_SOURCE);
    include_compile.AddIncludeDir(RELATIVE_INCLUDE_DIR);
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
    CHECK_FALSE(loaded_dirs.find(include_header_path) == loaded_dirs.end());
  }

  buildcc::env::deinit();
}

// TODO, Check toolchain change
// There are few parameters that must NOT be changed after the initial buildcc
// project is generated
// NOTE, Throw an error if these options are changed by the user

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
