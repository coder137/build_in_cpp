#include "env/task_state.h"

#include "taskflow/taskflow.hpp"

// NOTE, Make sure all these includes are AFTER the system and header includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/Utest.h"

// clang-format off
TEST_GROUP(TaskStateTestGroup)
{
  void setup() {
    buildcc::env::set_task_state(buildcc::env::TaskState::SUCCESS);
  }
};
// clang-format on

TEST(TaskStateTestGroup, OneTask) {
  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);

  tf::Taskflow tf;
  bool completed = false;
  tf.emplace([&]() {
    buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);
    completed = true;
  });
  tf::Executor executor(2);
  executor.run(tf);
  executor.wait_for_all();

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
  CHECK_TRUE(completed);
}

TEST(TaskStateTestGroup, MultipleTasks) {
  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::SUCCESS);

  tf::Taskflow tf;
  bool completed1 = false;
  tf.emplace([&]() {
    buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);
    completed1 = true;
  });

  bool completed2 = false;
  tf.emplace([&]() {
    buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);
    completed2 = true;
  });

  bool completed3 = false;
  tf.emplace([&]() {
    buildcc::env::set_task_state(buildcc::env::TaskState::FAILURE);
    completed3 = true;
  });

  tf::Executor executor(2);
  executor.run(tf);
  executor.wait_for_all();

  CHECK(buildcc::env::get_task_state() == buildcc::env::TaskState::FAILURE);
  CHECK_TRUE(completed1);
  CHECK_TRUE(completed2);
  CHECK_TRUE(completed3);
}

int main(int ac, char **av) {
  return CommandLineTestRunner::RunAllTests(ac, av);
}
