#include "env/command.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(CommandTestGroup)
{
};
// clang-format on

TEST(CommandTestGroup, Construct_InvalidFormat) {
  buildcc::env::Command command;
  CHECK_THROWS(std::exception, command.Construct("{test}"));
}

TEST(CommandTestGroup, Construct_Basic) {
  buildcc::env::Command command;
  std::string s = command.Construct("{}", {{"", "hi"}});
  STRCMP_EQUAL(s.c_str(), "hi");
}

TEST(CommandTestGroup, Construct_MultipleArgs) {
  buildcc::env::Command command;
  command.AddDefaultArguments({
      {"h", "hello"},
      {"w", "world"},
  });
  std::string s = command.Construct("{h} {w} {f} {c}", {
                                                           {"f", "from"},
                                                           {"c", "coder137"},
                                                       });
  STRCMP_EQUAL(s.c_str(), "hello world from coder137");
}

TEST(CommandTestGroup, Construct_BadArguments) {
  buildcc::env::Command command;
  CHECK_THROWS(std::exception, command.Construct("{}", {{nullptr, "hi"}}));
}

TEST(CommandTestGroup, GetDefaultValueByKey) {
  buildcc::env::Command command;
  command.AddDefaultArgument("key", "value");
  const std::string &value = command.GetDefaultValueByKey("key");

  STRCMP_EQUAL(value.c_str(), "value");
}

TEST(CommandTestGroup, GetDefaultValueByKey_BadKey) {
  buildcc::env::Command command;
  command.AddDefaultArgument("key", "value");
  CHECK_THROWS(std::exception, command.GetDefaultValueByKey("bad_key"));
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
