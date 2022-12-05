// Internal
#include "schema/path.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(PathSchemaTestGroup)
{
};
// clang-format on

TEST(PathSchemaTestGroup, PathList) { buildcc::internal::PathList paths; }

TEST(PathSchemaTestGroup, PathInfoList_OrderedEmplace) {
  buildcc::internal::PathInfoList path_infos;

  path_infos.Emplace("hello/world/first_file.txt", "");
  path_infos.Emplace("hello/world/second_file.txt", "");
  path_infos.Emplace("hello/world/third_file.txt", "");
  path_infos.Emplace("hello/world/fourth_file.txt", "");

  std::vector<std::string> paths = {
      "hello/world/first_file.txt",
      "hello/world/second_file.txt",
      "hello/world/third_file.txt",
      "hello/world/fourth_file.txt",
  };

  auto inserted_paths = path_infos.GetPaths();
  for (std::size_t i = 0; i < inserted_paths.size(); i++) {
    STRCMP_EQUAL(paths[i].c_str(), inserted_paths[i].c_str());
  }

  json j = path_infos;
  UT_PRINT(j.dump().c_str());
}

TEST(PathSchemaTestGroup, PathInfoList_GetChanged) {
  {
    buildcc::internal::PathInfoList pinfolist1{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "3"},
    };
    buildcc::internal::PathInfoList pinfolist2{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "3"},
    };
    CHECK_TRUE(pinfolist1.IsEqual(pinfolist2));
  }

  // Missing path info
  {
    buildcc::internal::PathInfoList pinfolist1{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "3"},
    };
    buildcc::internal::PathInfoList pinfolist2{
        {"first.txt", "1"}, {"second.txt", "2"},
        //   {"third.txt", "3"},
    };
    CHECK_FALSE(pinfolist1.IsEqual(pinfolist2));
  }

  // Wrong hash
  {
    buildcc::internal::PathInfoList pinfolist1{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "3"},
    };
    buildcc::internal::PathInfoList pinfolist2{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "4"},
    };
    CHECK_FALSE(pinfolist1.IsEqual(pinfolist2));
  }

  // Wrong order
  {
    buildcc::internal::PathInfoList pinfolist1{
        {"first.txt", "1"},
        {"second.txt", "2"},
        {"third.txt", "3"},
    };
    buildcc::internal::PathInfoList pinfolist2{
        {"first.txt", "1"},
        {"third.txt", "3"},
        {"second.txt", "2"},
    };
    CHECK_FALSE(pinfolist1.IsEqual(pinfolist2));
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
