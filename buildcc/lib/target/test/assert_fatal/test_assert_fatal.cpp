// Internal
#include "internal/assert_fatal.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(AssertFatalTestGroup)
{
};
// clang-format on

TEST(AssertFatalTestGroup, AssertFatalTest_NoThrow) {
  buildcc::internal::assert_fatal(false, false, "Not valid");
  buildcc::internal::assert_fatal(true, true, "Not valid");
  buildcc::internal::assert_fatal("true", "true", "Not valid");
  buildcc::internal::assert_fatal(10, 10, "Not valid");
  buildcc::internal::assert_fatal(10.10, 10.10, "Not valid");
}

TEST(AssertFatalTestGroup, AssertFatalTest_Throw) {
  MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();

  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal(false, true, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal(true, false, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal("false", "true", "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal(10, 12, "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal(10.10, 12.12, "Not valid"));

  MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
}

TEST(AssertFatalTestGroup, AssertFatalTrueTest_NoThrow) {
  buildcc::internal::assert_fatal_true(false == false, "Not valid");
  buildcc::internal::assert_fatal_true(true, "Not valid");
  buildcc::internal::assert_fatal_true("true" == "true", "Not valid");
  buildcc::internal::assert_fatal_true(10 == 10, "Not valid");
  buildcc::internal::assert_fatal_true(10.10 == 10.10, "Not valid");
}

TEST(AssertFatalTestGroup, AssertFatalTrueTest_Throw) {
  MemoryLeakWarningPlugin::saveAndDisableNewDeleteOverloads();

  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal_true(false, "Not valid"));
  CHECK_THROWS(std::string, buildcc::internal::assert_fatal_true(true == false,
                                                                 "Not valid"));
  CHECK_THROWS(std::string, buildcc::internal::assert_fatal_true(
                                "false" == "true", "Not valid"));
  CHECK_THROWS(std::string,
               buildcc::internal::assert_fatal_true(10 == 12, "Not valid"));
  CHECK_THROWS(std::string, buildcc::internal::assert_fatal_true(10.10 == 12.12,
                                                                 "Not valid"));

  MemoryLeakWarningPlugin::restoreNewDeleteOverloads();
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
