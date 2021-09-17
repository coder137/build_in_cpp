#include "command/command.h"

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
  buildcc::Command command;
  CHECK_THROWS(std::exception, command.Construct("{test}"));
}

TEST(CommandTestGroup, Construct_Basic) {
  buildcc::Command command;
  std::string s = command.Construct("{}", {{"", "hi"}});
  STRCMP_EQUAL(s.c_str(), "hi");
}

TEST(CommandTestGroup, Construct_MultipleArgs) {
  buildcc::Command command;
  command.AddDefaultArguments({{
      {"h", "hello"},
      {"w", "world"},
  }});
  std::string s = command.Construct("{h} {w} {f} {c}", {
                                                           {"f", "from"},
                                                           {"c", "coder137"},
                                                       });
  STRCMP_EQUAL(s.c_str(), "hello world from coder137");
}

TEST(CommandTestGroup, BadDefaultArguments) {
  buildcc::Command command;
  command.AddDefaultArguments({{nullptr, "hi"}});
  CHECK_THROWS(std::exception, command.Construct("{}"));
}

TEST(CommandTestGroup, Construct_BadArguments) {
  buildcc::Command command;
  CHECK_THROWS(std::exception, command.Construct("{}", {{nullptr, "hi"}}));
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
