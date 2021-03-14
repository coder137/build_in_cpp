// Internal
#include "internal/path.h"

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

TEST(PathTestGroup, Path_ExistingPathStaticConstructor) {
  auto existing_path =
      buildcc::internal::Path::CreateExistingPath("path_main.cpp");
  STRCMP_EQUAL(existing_path.GetPathname().c_str(), "path_main.cpp");
  // * NOTE, Last write timestamp changes whenever we resave or re-download
  // This would not work well with Git
  //   UNSIGNED_LONGLONGS_EQUAL(existing_path.GetLastWriteTimestamp(),
  //                            13623997187709551616ULL);
}

TEST(PathTestGroup, Path_ExistingPathStaticConstructor_ThrowFileException) {
  CHECK_THROWS(
      std::filesystem::filesystem_error,
      buildcc::internal::Path::CreateExistingPath("random_path_main.cpp"));

  try {
    auto existing_filename =
        buildcc::internal::Path::CreateExistingPath("random_path_main.cpp");
  } catch (const std::filesystem::filesystem_error &e) {
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
  STRCMP_EQUAL(p.GetPathname().c_str(), "random_path_main.cpp");
  UNSIGNED_LONGLONGS_EQUAL(p.GetLastWriteTimestamp(), 12345ULL);
}

TEST(PathTestGroup, Path_EqualityOperator) {
  buildcc::internal::Path p =
      buildcc::internal::Path::CreateExistingPath("path_main.cpp");
  STRCMP_EQUAL(p.GetPathname().c_str(), "path_main.cpp");

  buildcc::internal::Path newp =
      buildcc::internal::Path::CreateNewPath("path_main.cpp", 12345ULL);

  // NOTE, Equality does not match the last_write_timestamp
  // ONLY matches the string
  CHECK(p == newp);
  CHECK(p == std::string("path_main.cpp"));
  CHECK(p == "path_main.cpp");
}

TEST(PathTestGroup, Path_UnorderedSet) {
  std::unordered_set<buildcc::internal::Path, buildcc::internal::PathHash>
      unique_paths;

  // Check inserts
  CHECK_TRUE(
      unique_paths
          .insert(buildcc::internal::Path::CreateExistingPath("path_main.cpp"))
          .second);
  CHECK_FALSE(unique_paths
                  .insert(buildcc::internal::Path::CreateNewPath(
                      "path_main.cpp", 12345ULL))
                  .second);
  CHECK_TRUE(unique_paths
                 .insert(buildcc::internal::Path::CreateNewPath(
                     "random_path_main.cpp", 98765ULL))
                 .second);

  // Check finds
  // * NOTE, Only matches pathname
  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateExistingPath(
                  "path_main.cpp")) == unique_paths.end());

  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                  "path_main.cpp", 1111ULL)) == unique_paths.end());
  CHECK_FALSE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                  "random_path_main.cpp", 12345ULL)) == unique_paths.end());
  CHECK_TRUE(unique_paths.find(buildcc::internal::Path::CreateNewPath(
                 "incorrect_path_main.cpp", 0000ULL)) == unique_paths.end());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
