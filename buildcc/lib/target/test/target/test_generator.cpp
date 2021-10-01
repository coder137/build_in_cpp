#include "target/generator.h"

#include <unistd.h>

#include "expect_command.h"
#include "expect_generator.h"

#include "taskflow/taskflow.hpp"

#include "env/util.h"

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

TEST(GeneratorTestGroup, Generator_AddPregenerateCb) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "AddPregenerateCb";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    CHECK_THROWS(std::exception,
                 generator.AddPregenerateCb(std::function<void(void)>()));
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddPregenerateCb([]() {});
  }
}

TEST(GeneratorTestGroup, Generator_AddPostgenerateCb) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "AddPostgenerateCb";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    CHECK_THROWS(std::exception,
                 generator.AddPostgenerateCb(std::function<void(void)>()));
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddPostgenerateCb([]() {});
  }
}

TEST(GeneratorTestGroup, Generator_AddInfo) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "AddInfo";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
  generator.AddGenInfo("gcc_1",
                       {
                           "data/dummy_main.c",
                       },
                       {TEST_BUILD_DIR / "dummy_main.exe"},
                       {"gcc -o intermediate/generator/AddInfo/dummy_main.exe "
                        "data/dummy_main.c"},
                       true);

  CHECK_THROWS(std::exception, generator.AddGenInfo("gcc_1", {}, {}, {}, true));
}

TEST(GeneratorTestGroup, Generator_Build_FlowBuilderOverload) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Build_FlowBuilderOverload";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
  generator.AddGenInfo("gcc_1",
                       {
                           "data/dummy_main.c",
                       },
                       {TEST_BUILD_DIR / "dummy_main.exe"},
                       {"gcc -o intermediate/generator/Build/dummy_main.exe "
                        "data/dummy_main.c"},
                       true);

  buildcc::m::CommandExpect_Execute(1, true);
  // Empty taskflow
  tf::Taskflow tf;
  generator.Build(tf);

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Build) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Build";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
  generator.AddGenInfo("gcc_1",
                       {
                           "data/dummy_main.c",
                       },
                       {TEST_BUILD_DIR / "dummy_main.exe"},
                       {"gcc -o intermediate/generator/Build/dummy_main.exe "
                        "data/dummy_main.c"},
                       true);

  buildcc::m::CommandExpect_Execute(1, true);
  generator.Build();

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_CurrentNotFound) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild_CurrentNotFound";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_CurrentNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    generator.AddGenInfo(
        "gcc_2",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_CurrentNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_CurrentNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    // gcc_2 missing

    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_PreviousNotFound) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild_PreviousNotFound";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_PreviousNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_PreviousNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    // Current Info is NEWER than old
    generator.AddGenInfo(
        "gcc_2",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o "
         "intermediate/generator/Rebuild_PreviousNotFound/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Inputs) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild_Inputs";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild_Inputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
            "data/new_source.cpp",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild_Inputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::base::m::GeneratorExpect_InputAdded(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/new_source.cpp",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild_Inputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::base::m::GeneratorExpect_InputRemoved(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  sleep(1);
  bool saved = buildcc::env::SaveFile("data/new_source.cpp", "", false);
  CHECK_TRUE(saved);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/new_source.cpp",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild_Inputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::base::m::GeneratorExpect_InputUpdated(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Outputs) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild_Outputs";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/Rebuild_Outputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {TEST_BUILD_DIR / "dummy_main.exe"},
        {"gcc -o intermediate/generator/Rebuild_Outputs/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::base::m::GeneratorExpect_OutputChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_AddCustomRegenerate) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "AddCustomRegenerate";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/AddCustomRegenerate/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/AddCustomRegenerate/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    // TODO, Improve
    generator.AddCustomRegenerateCb(
        [](const buildcc::internal::geninfo_unordered_map &previous_info,
           const buildcc::internal::geninfo_unordered_map &current_info,
           std::vector<const buildcc::internal::GenInfo *> &generated_files,
           std::vector<const buildcc::internal::GenInfo *>
               &dummy_generated_files) {
          (void)previous_info;
          (void)dummy_generated_files;

          for (const auto &ci : current_info) {
            generated_files.push_back(&(ci.second));
          }

          return true;
        });

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/AddCustomRegenerate/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    // TODO, Improve
    generator.AddCustomRegenerateCb(
        [](const buildcc::internal::geninfo_unordered_map &previous_info,
           const buildcc::internal::geninfo_unordered_map &current_info,
           std::vector<const buildcc::internal::GenInfo *> &generated_files,
           std::vector<const buildcc::internal::GenInfo *>
               &dummy_generated_files) {
          (void)previous_info;
          (void)current_info;
          (void)generated_files;
          (void)dummy_generated_files;
          return false;
        });

    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/AddCustomRegenerate/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    CHECK_THROWS(std::exception,
                 generator.AddCustomRegenerateCb(
                     buildcc::base::custom_regenerate_cb_params()));
    generator.Build();
  }

  mock().checkExpectations();
}

TEST(GeneratorTestGroup, Generator_Rebuild_Commands) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "Rebuild_Commands";
  fs::create_directories(TEST_BUILD_DIR);

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {"gcc -o intermediate/generator/Rebuild_Commands/dummy_main.exe "
         "data/dummy_main.c"},
        true);

    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  {
    buildcc::base::Generator generator("custom_file_generator", TEST_BUILD_DIR);
    generator.AddGenInfo(
        "gcc_1",
        {
            "data/dummy_main.c",
        },
        {
            TEST_BUILD_DIR / "dummy_main.o",
            TEST_BUILD_DIR / "dummy_main.exe",
        },
        {
            "gcc -c -o intermediate/generator/Rebuild_Commands/dummy_main.o "
            "data/dummy_main.c",
            "gcc -o intermediate/generator/Rebuild_Commands/dummy_main.exe "
            "intermediate/generator/Rebuild_Commands/dummy_main.o",
        },
        true);

    buildcc::base::m::GeneratorExpect_CommandChanged(1, &generator);
    buildcc::m::CommandExpect_Execute(1, true);
    buildcc::m::CommandExpect_Execute(1, true);
    generator.Build();
  }

  mock().checkExpectations();
}

// Input for second generator is output of first generator
TEST(GeneratorTestGroup, Generator_DoubleDependency) {
  fs::path TEST_BUILD_DIR = BUILD_DIR / "DoubleDependency";
  fs::create_directories(TEST_BUILD_DIR);

  buildcc::base::Generator ogen("custom_object_generator", TEST_BUILD_DIR);
  ogen.AddGenInfo(
      "gcc_1",
      {
          "data/dummy_main.c",
      },
      {TEST_BUILD_DIR / "dummy_main.o"},
      {"gcc -c -o intermediate/generator/DoubleDependency/dummy_main.o "
       "data/dummy_main.c"},
      true);

  buildcc::m::CommandExpect_Execute(1, true);
  ogen.Build();

  // * NOTE, dummy_main.o is an input to `custom_exe_generator`
  buildcc::env::SaveFile((TEST_BUILD_DIR / "dummy_main.o").string().c_str(), "",
                         false);

  buildcc::base::Generator egen("custom_exe_generator", TEST_BUILD_DIR);
  egen.AddGenInfo(
      "gcc_1",
      {
          TEST_BUILD_DIR / "dummy_main.o",
      },
      {TEST_BUILD_DIR / "dummy_main.o"},
      {"gcc -o intermediate/generator/DoubleDependency/dummy_main.exe "
       "intermediate/generator/DoubleDependency/dummy_main.o"},
      true);

  buildcc::m::CommandExpect_Execute(1, true);
  egen.Build();

  mock().checkExpectations();
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  fs::create_directories(BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
