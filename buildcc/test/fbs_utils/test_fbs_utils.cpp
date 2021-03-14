#include "constants.h"

// Internal
#include "internal/fbs_utils.h"
#include "internal/schema_loader.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(FbsUtilsTestGroup)
{

   void teardown()
    {
        mock().clear();
    }

};
// clang-format on

TEST(FbsUtilsTestGroup, FbsTargetTypeTest) {
  {
    constexpr const char *const NAME = "CreateTargetTypeExecutable.exe";

    // Store
    buildcc::internal::fbs_utils_store_target(
        NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::Executable,
        buildcc::Toolchain("gcc", "gcc", "g++"),
        buildcc::internal::path_unordered_set(),
        buildcc::internal::path_unordered_set());

    // TODO, Use Schema Loader to verify TargetType
  }

  {
    constexpr const char *const NAME = "CreateTargetTypeStaticLibrary.a";

    // Store
    buildcc::internal::fbs_utils_store_target(
        NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::StaticLibrary,
        buildcc::Toolchain("gcc", "gcc", "g++"),
        buildcc::internal::path_unordered_set(),
        buildcc::internal::path_unordered_set());

    // TODO, Use Schema Loader to verify TargetType
  }

  {
    constexpr const char *const NAME = "CreateTargetTypeDynamicLibrary.so";

    // Store
    buildcc::internal::fbs_utils_store_target(
        NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::DynamicLibrary,
        buildcc::Toolchain("gcc", "gcc", "g++"),
        buildcc::internal::path_unordered_set(),
        buildcc::internal::path_unordered_set());

    // TODO, Use Schema Loader to verify TargetType
  }

  {
    constexpr const char *const NAME = "CreateTargetTypeError";

    // Store
    CHECK_THROWS(std::string,
                 buildcc::internal::fbs_utils_store_target(
                     NAME, BUILD_SCRIPT_SOURCE, (buildcc::TargetType)3,
                     buildcc::Toolchain("gcc", "gcc", "g++"),
                     buildcc::internal::path_unordered_set(),
                     buildcc::internal::path_unordered_set()));
  }
}

TEST(FbsUtilsTestGroup, FbsTargetStoreEmptyTest) {
  constexpr const char *const NAME = "CreateEmpty.exe";
  // Store
  buildcc::internal::fbs_utils_store_target(
      NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::Executable,
      buildcc::Toolchain("gcc", "gcc", "g++"),
      buildcc::internal::path_unordered_set(),
      buildcc::internal::path_unordered_set());

  // Load
  buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  CHECK_EQUAL(loader.GetLoadedSources().size(), 0);
  CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 0);
}

TEST(FbsUtilsTestGroup, FbsTargetStoreSourceTest) {
  constexpr const char *const NAME = "CreateStoreSource.exe";

  buildcc::internal::path_unordered_set sources;
  sources.insert(buildcc::internal::Path::CreateNewPath("p1.cpp"));
  sources.insert(buildcc::internal::Path::CreateNewPath("p2.cpp"));
  sources.insert(buildcc::internal::Path::CreateNewPath("p3.cpp"));

  // Store
  buildcc::internal::fbs_utils_store_target(
      NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::Executable,
      buildcc::Toolchain("gcc", "gcc", "g++"), sources,
      buildcc::internal::path_unordered_set());

  // Load
  buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 0);
  CHECK_EQUAL(loader.GetLoadedSources().size(), 3);

  // Check if the above 3 paths are present
  CHECK_FALSE(loader.GetLoadedSources().find(
                  buildcc::internal::Path::CreateNewPath("p1.cpp")) ==
              loader.GetLoadedSources().end());
  CHECK_FALSE(loader.GetLoadedSources().find(
                  buildcc::internal::Path::CreateNewPath("p2.cpp")) ==
              loader.GetLoadedSources().end());
  CHECK_FALSE(loader.GetLoadedSources().find(
                  buildcc::internal::Path::CreateNewPath("p3.cpp")) ==
              loader.GetLoadedSources().end());
}

TEST(FbsUtilsTestGroup, FbsTargetStoreIncludeDirsTest) {
  constexpr const char *const NAME = "CreateStoreIncludeDir.exe";

  buildcc::internal::path_unordered_set include_dirs;
  include_dirs.insert(
      buildcc::internal::Path::CreateExistingPath(BUILD_SCRIPT_EXE));
  include_dirs.insert(
      buildcc::internal::Path::CreateExistingPath(BUILD_SCRIPT_SOURCE));

  // Store
  buildcc::internal::fbs_utils_store_target(
      NAME, BUILD_SCRIPT_SOURCE, buildcc::TargetType::Executable,
      buildcc::Toolchain("gcc", "gcc", "g++"),
      buildcc::internal::path_unordered_set(), include_dirs);

  // Load
  buildcc::internal::SchemaLoader loader(NAME, BUILD_SCRIPT_SOURCE);
  bool is_loaded = loader.Load();
  CHECK_TRUE(is_loaded);

  CHECK_EQUAL(loader.GetLoadedIncludeDirs().size(), 2);
  CHECK_EQUAL(loader.GetLoadedSources().size(), 0);

  // Check if the above 3 paths are present
  CHECK_FALSE(loader.GetLoadedIncludeDirs().find(
                  buildcc::internal::Path::CreateNewPath(BUILD_SCRIPT_EXE)) ==
              loader.GetLoadedIncludeDirs().end());
  CHECK_FALSE(
      loader.GetLoadedIncludeDirs().find(buildcc::internal::Path::CreateNewPath(
          BUILD_SCRIPT_SOURCE)) == loader.GetLoadedIncludeDirs().end());
}

int main(int ac, char **av) {
  MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
  return CommandLineTestRunner::RunAllTests(ac, av);
}
