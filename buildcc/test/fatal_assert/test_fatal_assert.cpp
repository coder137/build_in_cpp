#include "fatal_assert.h"
#include "spdlog/spdlog.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(FatalAssertTestGroup)
{
};
// clang-format on

TEST(FatalAssertTestGroup, FatalAssertTest_NoThrow) {
  buildcc::assert_fatal(false, false, "Not valid");
  buildcc::assert_fatal(true, true, "Not valid");
  buildcc::assert_fatal("true", "true", "Not valid");
  buildcc::assert_fatal(10, 10, "Not valid");
  buildcc::assert_fatal(10.10, 10.10, "Not valid");
}

TEST(FatalAssertTestGroup, FatalAssertTest_Throw) {
  MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();

  CHECK_THROWS(std::string, buildcc::assert_fatal(false, true, "Not valid"));
  CHECK_THROWS(std::string, buildcc::assert_fatal(true, false, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::assert_fatal("false", "true", "Not valid"));
  CHECK_THROWS(std::string, buildcc::assert_fatal(10, 12, "Not valid"));
  CHECK_THROWS(std::string, buildcc::assert_fatal(10.10, 12.12, "Not valid"));

  MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
}

TEST(FatalAssertTestGroup, FatalAssertTrueTest_NoThrow) {
  buildcc::assert_fatal_true(false == false, "Not valid");
  buildcc::assert_fatal_true(true, "Not valid");
  buildcc::assert_fatal_true("true" == "true", "Not valid");
  buildcc::assert_fatal_true(10 == 10, "Not valid");
  buildcc::assert_fatal_true(10.10 == 10.10, "Not valid");
}

TEST(FatalAssertTestGroup, FatalAssertTrueTest_Throw) {
  MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();

  CHECK_THROWS(std::string, buildcc::assert_fatal_true(false, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::assert_fatal_true(true == false, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::assert_fatal_true("false" == "true", "Not valid"));
  CHECK_THROWS(std::string, buildcc::assert_fatal_true(10 == 12, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::assert_fatal_true(10.10 == 12.12, "Not valid"));

  MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
