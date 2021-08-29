#include "target/generator.h"

#include "expect_command.h"
#include "expect_generator.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(GeneratorTestGroup)
{
    void teardown() {
      mock().clear();
    }
};
// clang-format on

fs::path BUILD_DIR = "intermediate/generator";

TEST(GeneratorTestGroup, Generator_AddInfo) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "AddInfo";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
  generator.AddGenInfo(buildcc::base::UserGenInfo(
      "gcc_1",
      {
          "data/dummy_main.c",
      },
      {TEST_BUILD_DIR / "dummy_main.exe"},
      {"gcc -o intermediate/generator/AddInfo/dummy_main.exe "
       "data/dummy_main.c"},
      true));

  CHECK_THROWS(buildcc::env::assert_exception,
               generator.AddGenInfo(
                   buildcc::base::UserGenInfo("gcc_1", {}, {}, {}, true)));
}

TEST(GeneratorTestGroup, Generator_Build) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Build";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
  generator.AddGenInfo(buildcc::base::UserGenInfo(
      "gcc_1",
      {
          "data/dummy_main.c",
      },
      {TEST_BUILD_DIR / "dummy_main.exe"},
      {"gcc -o intermediate/generator/AddInfo/dummy_main.exe "
       "data/dummy_main.c"},
      true));

  buildcc::m::CommandExpect_Execute(1, true);
  generator.Build();

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(buildcc::base::UserGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/AddInfo/dummy_main.exe "
         "data/dummy_main.c"},
        true));

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(buildcc::base::UserGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/AddInfo/dummy_main.exe "
         "data/dummy_main.c"},
        true));

    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_PreviousNotFound) {}
TEST(GeneratorTestGroup, Generator_Rebuild_Remove) {}
TEST(GeneratorTestGroup, Generator_Rebuild_Add) {}
TEST(GeneratorTestGroup, Generator_Rebuild_Update) {}

// Input for second generator is output of first generator
TEST(GeneratorTestGroup, Generator_DoubleDependency) {}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  fs::create_directories(BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
