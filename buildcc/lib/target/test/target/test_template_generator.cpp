#include "target/custom_generator.h"

#include "expect_command.h"
#include "expect_custom_generator.h"
#include "test_target_util.h"

#include "flatbuffers/flexbuffers.h"

#include <memory>

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

TEST(TemplateGeneratorTestGroup, Basic) {}

int main(int ac, char **av) {
  fs::remove_all(BUILD_DIR);
  buildcc::Project::Init(fs::current_path() / "data", BUILD_DIR);
  return CommandLineTestRunner::RunAllTests(ac, av);
}
