#include "args/persistent_storage.h"

#include "target/generator.h"
#include "target/target.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

// clang-format off
TEST_GROUP(PersistentStorageTestGroup)
{
};
// clang-format on

buildcc::BaseToolchain gcc(buildcc::ToolchainId::Gcc, "gcc", "as", "gcc", "g++",
                           "ar", "ld");

TEST(PersistentStorageTestGroup, BasicUsage) {
  buildcc::PersistentStorage persistent;
  persistent.Add<buildcc::BaseTarget>("target_identifier", "target_name",
                                      buildcc::TargetType::Executable, gcc, "");
  persistent.Add<buildcc::BaseGenerator>("generator_identifier",
                                         "generator_name", "");

  // Usage
  persistent.ConstRef<buildcc::BaseTarget>("target_identifier").GetName();
  persistent.Ref<buildcc::BaseGenerator>("generator_identifier").GetTaskflow();

  // Automatic cleanup here
}

TEST(PersistentStorageTestGroup, IncorrectUsage) {
  buildcc::PersistentStorage persistent;
  persistent.Add<buildcc::BaseTarget>("target_identifier", "target_name",
                                      buildcc::TargetType::Executable, gcc, "");

  // We try to cast to a different type!
  CHECK_THROWS(std::exception,
               persistent.Ref<buildcc::BaseGenerator>("target_identifier"));

  // We use a wrong identifier
  CHECK_THROWS(std::exception,
               persistent.Ref<buildcc::BaseTarget>("generator_identifier"));
}

int main(int ac, char **av) {
  buildcc::env::init(fs::current_path(), fs::current_path());
  return CommandLineTestRunner::RunAllTests(ac, av);
}
