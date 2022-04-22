#include <thread>

#include "taskflow/taskflow.hpp"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"
#include "CppUTestExt/MockSupport.h"

namespace {

std::thread::id my_main_thread = std::this_thread::get_id();

bool IsRunningInThread() {
  bool threaded = true;
  if (std::this_thread::get_id() == my_main_thread) {
    threaded = false;
  }
  return threaded;
}

void assert_handle_fatal() {
  if (IsRunningInThread()) {
    mock().actualCall("assert_handle_fatal_threaded");
  } else {
    mock().actualCall("assert_handle_fatal_main");
  }
}

} // namespace

// clang-format off
TEST_GROUP(AssertFatalTestGroup)
{
  void teardown() {
      mock().clear();
  }
};
// clang-format on

TEST(AssertFatalTestGroup, AssertFatal_IsThreadedCheck) {
  CHECK_FALSE(IsRunningInThread());

  tf::Taskflow tf;
  tf.emplace([]() { CHECK_TRUE(IsRunningInThread()); });

  tf::Executor ex(1);
  ex.run(tf);
  ex.wait_for_all();
}

TEST(AssertFatalTestGroup, AssertFatal_Threaded) {
  mock().expectOneCall("assert_handle_fatal_threaded");

  tf::Taskflow tf;
  tf.emplace([]() { assert_handle_fatal(); });

  tf::Executor ex(1);
  ex.run(tf);
  ex.wait_for_all();
}

TEST(AssertFatalTestGroup, AssertFatal_NotThreaded) {
  mock().expectOneCall("assert_handle_fatal_main");
  assert_handle_fatal();
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
