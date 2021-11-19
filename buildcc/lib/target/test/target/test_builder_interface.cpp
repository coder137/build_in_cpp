#include "target/interface/builder_interface.h"

#include "target/common/path.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// TestBuilderInterface

class TestBuilderInterface : public buildcc::base::BuilderInterface {
public:
  void Build() override {}

  void RecheckChangedIncorrectCb() {
    BuilderInterface::RecheckChanged(previous_, current_,
                                     std::function<void(void)>());
  }
  void RecheckPathIncorrectRemoveCb() {
    BuilderInterface::RecheckPaths(previous_, current_,
                                   std::function<void(void)>());
  }
  void RecheckPathIncorrectAddCb() {
    BuilderInterface::RecheckPaths(
        previous_, current_, []() {}, std::function<void(void)>());
  }
  void RecheckPathIncorrectUpdateCb() {
    BuilderInterface::RecheckPaths(
        previous_, current_, []() {}, []() {}, std::function<void(void)>());
  }

private:
  bool Store() override { return false; }
  buildcc::internal::path_unordered_set previous_;
  buildcc::internal::path_unordered_set current_;
};

// clang-format off
TEST_GROUP(TestBuilderInterfaceGroup)
{
};
// clang-format on

TEST(TestBuilderInterfaceGroup, IncorrectRecheckChangedCb) {
  TestBuilderInterface tbi;
  CHECK_THROWS(std::exception, tbi.RecheckChangedIncorrectCb());
}

TEST(TestBuilderInterfaceGroup, IncorrectRemoveRecheckCb) {
  TestBuilderInterface tbi;
  CHECK_THROWS(std::exception, tbi.RecheckPathIncorrectRemoveCb());
}

TEST(TestBuilderInterfaceGroup, IncorrectAddRecheckCb) {
  TestBuilderInterface tbi;
  CHECK_THROWS(std::exception, tbi.RecheckPathIncorrectAddCb());
}

TEST(TestBuilderInterfaceGroup, IncorrectUpdateRecheckCb) {
  TestBuilderInterface tbi;
  CHECK_THROWS(std::exception, tbi.RecheckPathIncorrectUpdateCb());
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
