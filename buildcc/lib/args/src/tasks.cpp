#include "args/register.h"

namespace buildcc {

tf::Task Register::BuildTask(base::Target &target) {
  return taskflow_.composed_of(target.GetTaskflow()).name("Task");
}

void Register::RunBuild() {
  executor_.run(taskflow_);
  executor_.wait_for_all();
}

} // namespace buildcc
