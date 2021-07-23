// Internal
#include "internal/path.h"

#include "env/assert_fatal.h"

#include "constants.h"

#include <cstdint>
#include <unordered_set>

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(PathTestGroup)
{
};
// clang-format on

static const auto current_file_path =
    (fs::path(BUILD_SCRIPT_SOURCE) / "path_main.cpp").make_preferred();

TEST(PathTestGroup, Path_ExistingPathStaticConstructor) {
  auto existing_path =
      buildcc::internal::Path::CreateExistingPath(current_file_path);
  STRCMP_EQUAL(existing_path.GetPathname().string().c_str(),
               current_file_path.string().c_str());
  // * NOTE, Last write timestamp changes whenever we resave or re-download
  // This would not work well with Git
  //   UNSIGNED_LONGLONGS_EQUAL(existing_path.GetLastWriteTimestamp(),
  //                            13623997187709551616ULL);
}

TEST(PathTestGroup, Path_ExistingPathStaticConstructor_ThrowFileException) {
  CHECK_THROWS(
      buildcc::env::assert_exception,
      buildcc::internal::Path::CreateExistingPath("random_path_main.cpp"));

  try {
    auto existing_filename =
        buildcc::internal::Path::CreateExistingPath("random_path_main.cpp");
  } catch (const buildcc::env::assert_exception &e) {
    // * NOTE, We cannot check this since different compilers might
    // have different error messages
    // STRCMP_EQUAL(
    //     e.what(),
    //     "filesystem error: cannot get file time: No such file or directory "
    //     "[random_path_main.cpp]");
  }
}

TEST(PathTestGroup, PathConstructor_NewPathStaticConstructor) {
  buildcc::internal::Path p =
      buildcc::internal::Path::CreateNewPath("random_path_main.cpp", 12345ULL);
  STRCMP_EQUAL(p.GetPathname().string().c_str(), "random_path_main.cpp");
  UNSIGNED_LONGLONGS_EQUAL(p.GetLastWriteTimestamp(), 12345ULL);
}

TEST(PathTestGroup, Path_EqualityOperator) {
  buildcc::internal::Path p =
      buildcc::internal::Path::CreateExistingPath(current_file_path);
  STRCMP_EQUAL(p.GetPathname().string().c_str(),
               current_file_path.string().c_str());

  buildcc::internal::Path newp =
      buildcc::internal::Path::CreateNewPath(current_file_path, 12345ULL);

  // NOTE, Equality does not match the last_write_timestamp
  // ONLY matches the string
  CHECK(p == newp);
  CHECK(p == current_file_path);
  CHECK(p == current_file_path);
}

TEST(PathTestGroup, Path_UnorderedSet) {
  std::unordered_set<buildcc::internal::Path, buildcc::internal::PathHash>
      unique_paths;

  // Check inserts
  CHECK_TRUE(unique_paths
                 .insert(buildcc::internal::Path::CreateExistingPath(
                     current_file_path))
                 .second);
  CHECK_FALSE(unique_paths
                  .insert(buildcc::internal::Path::CreateNewPath(
                      current_file_path, 12345ULL))
                  .second);
  CHECK_TRUE(unique_paths
                 .insert(buildcc::internal::Path::CreateNewPath(
                     "random_path_main.cpp", 98765ULL))
                 .second);

  // Check finds
  // * NOTE, Only matches pathname
  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateExistingPath(
                  current_file_path)) == unique_paths.end());

  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                  current_file_path, 1111ULL)) == unique_paths.end());
  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                  "random_path_main.cpp", 12345ULL)) == unique_paths.end());
  CHECK_TRUE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                 "incorrect_path_main.cpp", 0000ULL)) == unique_paths.end());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
