#include "target/interface/builder_interface.h"

#include "schema/path.h"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// TestBuilderInterface

class TestBuilderInterface : public buildcc::internal::BuilderInterface {
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

TEST(TestBuilderInterfaceGroup, CheckChanged) {
  std::vector<std::string> previous{"", "v1"};
  std::vector<std::string> current{"", "v1"};
  CHECK_FALSE(buildcc::internal::CheckChanged(previous, current));

  current.clear();
  current.push_back("");
  CHECK_TRUE(buildcc::internal::CheckChanged(previous, current));
}

TEST(TestBuilderInterfaceGroup, CheckPaths) {
  {
    auto pathstate = buildcc::internal::CheckPaths(
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
        },
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
        });
    CHECK_TRUE(pathstate == buildcc::internal::PathState::kNoChange);
  }

  {
    auto pathstate = buildcc::internal::CheckPaths(
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
        },
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
        });
    CHECK_TRUE(pathstate == buildcc::internal::PathState::kRemoved);
  }

  {
    auto pathstate = buildcc::internal::CheckPaths(
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
        },
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
            buildcc::internal::Path::CreateNewPath("v2", 0x3456),
        });
    CHECK_TRUE(pathstate == buildcc::internal::PathState::kAdded);
  }

  {
    auto pathstate = buildcc::internal::CheckPaths(
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x2345),
        },
        {
            buildcc::internal::Path::CreateNewPath("", 0x1234),
            buildcc::internal::Path::CreateNewPath("v1", 0x3456),
        });
    CHECK_TRUE(pathstate == buildcc::internal::PathState::kUpdated);
  }
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
