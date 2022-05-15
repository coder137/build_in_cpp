#include "target/template_generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"
#include "test_target_util.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(TemplateGeneratorTestGroup)
{
    void teardown() {
      mock().checkExpectations();
      mock().clear();
      buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
    }
};
// clang-format on

fs::path BUILD_DIR = fs::current_path() / "intermediate" / "template_generator";

TEST(TemplateGeneratorTestGroup, Basic) {
  buildcc::TemplateGenerator generator("basic", "");
  generator.Build();

  buildcc::m::CustomGeneratorRunner(generator);
}

TEST(TemplateGeneratorTestGroup, Basic_Parse) {
  buildcc::TemplateGenerator generator("basic_parse", "");
  generator.AddPatterns({
      {"hello", "Hello"},
      {"world", "World"},
  });
  std::string parsed = generator.Parse("{hello} {world}");
  STRCMP_EQUAL(parsed.c_str(), "Hello World");
}

TEST(TemplateGeneratorTestGroup, Basic_InputParse) {
  buildcc::TemplateGenerator generator("basic_inputparse", "");
  generator.AddPatterns({
      {"hello", "Hello"},
      {"world", "World"},
  });
  generator.AddTemplate("template/default_values.txt.in", "default_values.txt");
  generator.AddTemplate("template/hello_world.txt.in", "hello_world.txt");
  generator.Build();

  buildcc::m::CustomGeneratorRunner(generator);

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);
}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
